#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <iconv.h>
#include "cee_codec.h"

cee_boolean cee_codec_is_encoded_utf8(const cee_uchar* subject,
                                      cee_ulong length)
{
    if(!subject)
        return FALSE;
    
    cee_uchar* bytes = (cee_uchar*)subject;
    
    cee_uint i = 0;
    while (i < length) {
        if (0x0 <= bytes[i+0] && bytes[i+0] <= 0x7F) {
            i += 1;
            continue;
        }
        
        if((0xC0 <= bytes[i+0] && bytes[i+0] <= 0xDF) &&
           (0x80 <= bytes[i+1] && bytes[i+1] <= 0xBF)) {
            i += 2;
            continue;
        }
        
        if((0xE0 <= bytes[i+0] && bytes[i+0] <= 0xEF) &&
           (0x80 <= bytes[i+1] && bytes[i+1] <= 0xBF) &&
           (0x80 <= bytes[i+2] && bytes[i+2] <= 0xBF)) {
            i += 3;
            continue;
        }
        
        if ((0xF0 <= bytes[i+0] && bytes[i+0] <= 0xF7) &&
            (0x80 <= bytes[i+1] && bytes[i+1] <= 0xBF) &&
            (0x80 <= bytes[i+1] && bytes[i+1] <= 0xBF) &&
            (0x80 <= bytes[i+1] && bytes[i+1] <= 0xBF)) {
            i += 4;
            continue;
        }
        
        return FALSE;
    }
    return TRUE;
}

cee_boolean cee_codec_is_binary(const cee_uchar* subject,
                                cee_ulong length)
{
    if(!subject)
        return FALSE;
    
    cee_uchar* bytes = (cee_uchar*)subject;
    
    cee_uint i = 0;
    while (i < length) {
        if (bytes[i] == 0)
            return TRUE;
        i ++;
    }
    return FALSE;
}

/**
 * when return value equals -1, that means "offset" is the tail of storage buffer,
 * the "codepoint" is CEE_UNICODE_POINT_NUL
 */
void cee_codec_utf8_decode(const cee_uchar* subject,
                           cee_long offset,
                           CEEUnicodePoint* codepoint,
                           cee_size* length,
                           cee_long* next)
{
    const cee_uchar* ptr = &subject[offset];
    cee_size _length = 0;
    CEEUnicodePoint _codepoint = CEE_UNICODE_POINT_INVALID;
    cee_long _next = 0;
    
    if (offset < 0) {
        _next = -1;
        goto exit;
    }
    
    if (*ptr == 0) {
        _codepoint = CEE_UNICODE_POINT_NUL;
        _length = 1;
    }
    else if (!(ptr[0] & 0x80)) {
        /** encode in 1 byte */
        _codepoint = ptr[0] & 0x7F;
        _length = 1;
    }
    else if ((ptr[0] >> 5) == 0x6) {
        /** 0b1100 : 2 bytes */
        _codepoint = ((ptr[0] & 0x1F) << 6) |
                        (ptr[1] & 0x3F);
        _length = 2;
    }
    else if ((ptr[0] >> 4) == 0xe) {
        /** 0b1110 : 3 bytes */
        _codepoint = ((ptr[0] & 0x0F) << 12) |
                        ((ptr[1] & 0x3F) << 6) |
                        (ptr[2] & 0x3F);
        _length = 3;
    }
    else if ((ptr[0] >> 3) == 0x1e) {
        /** 0b1111 : 4 bytes */
        _codepoint = ((ptr[0] & 0x07) << 18) |
                        ((ptr[1] & 0x3F) << 12) |
                        ((ptr[2] & 0x3F) << 6) |
                        (ptr[3] & 0x3F);
        _length = 4;
    }
    else {
        _codepoint = ptr[0];
        _length = 1;
    }
    
    if (_codepoint == CEE_UNICODE_POINT_NUL)
        _next = -1;
    else
        _next = offset + _length;

exit:
    
    if (next)
        *next = _next;
    
    if (codepoint)
        *codepoint = _codepoint;
    
    if (length)
        *length = _length;
    
    return;
}

/**
 * when return value equals -1, that means "offset" is the head of storage buffer,
 * the "codepoint" is CEE_UNICODE_POINT_INVALID
 */
void cee_codec_utf8_decode_reversed(const cee_uchar* subject,
                                    cee_long offset,
                                    CEEUnicodePoint* codepoint,
                                    cee_ulong* length,
                                    cee_long* prev)
{
    cee_ulong _length = 0;
    CEEUnicodePoint _codepoint = CEE_UNICODE_POINT_INVALID;
    cee_long _prev = 0;
    const cee_uchar* ptr = NULL;
    
    if (offset <= 0) {
        _prev = -1;
        goto exit;
    }
    
    if (offset >= 4) {
        if ((subject[offset-4] >> 3) == 0x1e) {
            _length = 4;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ((ptr[0] & 0x07) << 18) |
                        ((ptr[1] & 0x3F) << 12) |
                        ((ptr[2] & 0x3F) << 6) |
                        (ptr[3] & 0x3F);
        }
        else if ((subject[offset-3] >> 4) == 0xe) {
            _length = 3;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ((ptr[0] & 0x0F) << 12) |
                            ((ptr[1] & 0x3F) << 6) |
                            (ptr[2] & 0x3F);
        }
        else if ((subject[offset-2] >> 5) == 0x6) {
            _length = 2;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ((ptr[0] & 0x1F) << 6) |
                            (ptr[1] & 0x3F);
            
        }
        else if (!(subject[offset-1] & 0x80)) {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ptr[0] & 0x7F;
        }
        else {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[offset];
            _codepoint = ptr[0];
        }
    }
    else if (offset == 3) {
        if ((subject[offset-3] >> 4) == 0xe) {
            _length = 3;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ((ptr[0] & 0x0F) << 12) |
                            ((ptr[1] & 0x3F) << 6) |
                            (ptr[2] & 0x3F);
        }
        else if ((subject[offset-2] >> 5) == 0x6) {
            _length = 2;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ((ptr[0] & 0x1F) << 6) |
                            (ptr[1] & 0x3F);
            
        }
        else if (!(subject[offset-1] & 0x80)) {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ptr[0] & 0x7F;
        }
        else {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[offset];
            _codepoint = ptr[0];
        }
    }
    else if (offset == 2) {
        if ((subject[offset-2] >> 5) == 0x6) {
            _length = 2;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ((ptr[0] & 0x1F) << 6) |
                            (ptr[1] & 0x3F);
            
        }
        else if (!(subject[offset-1] & 0x80)) {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ptr[0] & 0x7F;
        }
        else {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[offset];
            _codepoint = ptr[0];
        }
    }
    else if (offset == 1) {
        if (!(subject[offset-1] & 0x80)) {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[_prev];
            _codepoint = ptr[0] & 0x7F;
        }
        else {
            _length = 1;
            _prev = offset - _length;
            ptr = &subject[offset];
            _codepoint = ptr[0];
        }
    }
    
exit:
    if (codepoint)
        *codepoint = _codepoint;
    
    if (length)
        *length = _length;
    
    if (prev)
        *prev = _prev;
    
    return;
}

cee_ulong cee_codec_utf8_decode_length(const cee_uchar* subject,
                                      cee_long offset)
{
    const cee_uchar* ptr = &subject[offset];
    cee_size length = 0;
    
    if (offset < 0)
        return 0;
        
    if (*ptr == 0)
        length = 1;
    else if (!(ptr[0] & 0x80))
        /** encode in 1 byte */
        length = 1;
    else if ((ptr[0] >> 5) == 0x6)
        /** 0b1100 : 2 bytes */
        length = 2;
    else if ((ptr[0] >> 4) == 0xe)
        /** 0b1110 : 3 bytes */
        length = 3;
    else if ((ptr[0] >> 3) == 0x1e)
        /** 0b1111 : 4 bytes */
        length = 4;
    
    return length;
}

CEEUnicodePointType cee_codec_unicode_point_type(CEEUnicodePoint codepoint)
{
    if ((codepoint >= 0x0000 && codepoint <= 0x0020) || (codepoint == 0x007F))
        return kCEEUnicodePointTypeControl;
    else if ((codepoint >= 0x0021 && codepoint <= 0x002F) || /** ! ~ / */
             (codepoint >= 0x003A && codepoint <= 0x0040) || /** : ~ @ */
             (codepoint >= 0x005B && codepoint <= 0x005E) || /** [ ~ ^ */
             (codepoint == 0x0060) ||                        /** `     */
             (codepoint >= 0x007B && codepoint <= 0x007E) || /** { ~ ~ */
             (codepoint >= 0x0080 && codepoint <= 0x00FF))   /** Extended ASCII */
        return kCEEUnicodePointTypeASCIIPunctuation;
    else if ((codepoint == 0x005F) ||                        /** _ */
             (codepoint >= 0x0030 && codepoint <= 0x0039) || /** 0 ~ 9 */
             (codepoint >= 0x0041 && codepoint <= 0x005A) || /** A ~ Z */
             (codepoint >= 0x0061 && codepoint <= 0x007A))   /** a ~ z */
        return kCEEUnicodePointTypeASCIIAlphabet;
    else 
        return kCEEUnicodePointTypeWorldWideCharacter;
}

cee_ulong cee_codec_utf8_nb_codepoint(const cee_uchar* subject,
                                      cee_ulong length)
{
    cee_ulong nb_decoded = 0;
    cee_long current = 0;
    cee_long next = 0;
    cee_ulong total_decode_length = 0;
    
    while (TRUE) {
        CEEUnicodePoint codepoint;
        cee_ulong decode_length;
        cee_codec_utf8_decode(subject,
                              current,
                              &codepoint,
                              &decode_length,
                              &next);
        total_decode_length += decode_length;
        
        if (codepoint == CEE_UNICODE_POINT_NUL)
            break;
        
        nb_decoded ++;
        
        if (length && total_decode_length >= length)
            break;
        
        current = next;
    }
    return nb_decoded;
}

cee_boolean cee_codec_has_bom(const cee_uchar* buffer)
{
    if (buffer[0] == 0xEF &&
        buffer[1] == 0xBB &&
        buffer[2] == 0xBF)
        return TRUE; /** UTF-8 */
    else if (buffer[0] == 0xFE &&
             buffer[1] == 0xFF)
        return TRUE; /** UTF-16 (big-endian) */
    else if (buffer[0] == 0xFF &&
             buffer[1] == 0xFE)
        return TRUE; /** UTF-16 (little-endian) */
    else if (buffer[0] == 0x00 &&
             buffer[1] == 0x00 &&
             buffer[2] == 0xFE &&
             buffer[3] == 0xFF)
        return TRUE; /** UTF-32 (big-endian) */
    else if (buffer[0] == 0xFF &&
             buffer[1] == 0xFE &&
             buffer[2] == 0x00 &&
             buffer[3] == 0x00)
        return TRUE; /** UTF-32 (little-endian) */
    
    return FALSE;
}

void cee_codec_bom_export(const cee_uchar* buffer,
                          cee_uchar** bom,
                          cee_ulong* size)
{
    cee_uchar* _bom = NULL;
    cee_ulong _size = 0;
    
    if (buffer[0] == 0xEF &&
        buffer[1] == 0xBB &&
        buffer[2] == 0xBF) {
        _size = 3;
        _bom = cee_malloc0(sizeof(cee_uchar)*_size);
        /** UTF-8 */
        _bom[0] = buffer[0];
        _bom[1] = buffer[1];
        _bom[2] = buffer[2];
    }
    else if (buffer[0] == 0xFE &&
             buffer[1] == 0xFF) {
        _size = 2;
        _bom = cee_malloc0(sizeof(cee_uchar)*_size);
        /** UTF-16 (big-endian) */
        _bom[0] = buffer[0];
        _bom[1] = buffer[1];
    }
    else if (buffer[0] == 0xFF &&
             buffer[1] == 0xFE) {
        _size = 2;
        _bom = cee_malloc0(sizeof(cee_uchar)*_size);
        /** UTF-16 (little-endian) */
        _bom[0] = buffer[0];
        _bom[1] = buffer[1];
    }
    else if (buffer[0] == 0x00 &&
             buffer[1] == 0x00 &&
             buffer[2] == 0xFE &&
             buffer[3] == 0xFF) {
        _size = 4;
        _bom = cee_malloc0(sizeof(cee_uchar)*_size);
        /** UTF-32 (big-endian) */
        _bom[0] = buffer[0];
        _bom[1] = buffer[1];
        _bom[2] = buffer[2];
        _bom[3] = buffer[3];
    }
    else if (buffer[0] == 0xFF &&
             buffer[1] == 0xFE &&
             buffer[2] == 0x00 &&
             buffer[3] == 0x00) {
        _size = 4;
        _bom = cee_malloc0(sizeof(cee_uchar)*_size);
        /** UTF-32 (little-endian) */
        _bom[0] = buffer[0];
        _bom[1] = buffer[1];
        _bom[2] = buffer[2];
        _bom[3] = buffer[3];
    }
    
    if (bom)
        *bom = _bom;
    else
        cee_free(_bom);
    
    if (size)
        *size = _size;
}

const cee_char* cee_codec_encoded_type_from_bom(const cee_uchar* bom,
                                                cee_ulong bom_size)
{
    if (bom_size == 3) {
        if (bom[0] == 0xEF &&
            bom[1] == 0xBB &&
            bom[2] == 0xBF) {
            /** UTF-8 */
            return "UTF-8";
        }
    }
    else if (bom_size == 2) {
        if (bom[0] == 0xFE &&
            bom[1] == 0xFF) {
            /** UTF-16 (big-endian) */
            return "UTF-16BE";
        }
        else if (bom[0] == 0xFF &&
                 bom[1] == 0xFE) {
            /** UTF-16 (little-endian) */
            return "UTF-16LE";
        }
    }
    else if (bom_size == 4) {
        if (bom[0] == 0x00 &&
            bom[1] == 0x00 &&
            bom[2] == 0xFE &&
            bom[3] == 0xFF) {
            /** UTF-32 (big-endian) */
            return "UTF-32BE";
        }
        else if (bom[0] == 0xFF &&
                 bom[1] == 0xFE &&
                 bom[2] == 0x00 &&
                 bom[3] == 0x00) {
            /** UTF-32 (little-endian) */
            return "UTF-32LE";
        }
    }
    
    return NULL;
}

cee_uchar* cee_codec_convert_to_utf8_with_bom(const cee_uchar* subject,
                                              cee_size length,
                                              const cee_uchar* bom,
                                              cee_ulong bom_size)
{
    /**
     * subject should begin with BOM
     */
    const cee_char* to = "UTF-8";
    const cee_char* from = cee_codec_encoded_type_from_bom(bom, bom_size);
    if (!from)
        return NULL;
    
    cee_char* origin = (cee_char*)subject;
    cee_size mark_length = 0;
    if (!strcmp(from, "UTF-8")) {
        length -= 3;
        memmove(origin, origin + 3, length);
        origin[length] = '\0';
        return (cee_uchar*)cee_strdup(origin);
    }
    else if (!strcmp(from, "UTF-16BE")) {
        mark_length = 2;
    }
    else if (!strcmp(from, "UTF-16LE")) {
        mark_length = 2;
    }
    else if (!strcmp(from, "UTF-32BE")) {
        from = "UTF-32BE";
        mark_length = 4;
    }
    else if (!strcmp(from, "UTF-32LE")) {
        mark_length = 4;
    }
    
    length -= mark_length;
    memmove(origin, origin + mark_length, length);
    origin[length] = '\0';
    
    if (!from)
        return NULL;
    
    iconv_t cd = iconv_open(to, from);
    if (cd == (iconv_t)-1)
        return NULL;
    
    size_t input_length = length;
    size_t output_length = input_length * 4;
    cee_char* output_bytes = (cee_char*)cee_malloc0(output_length);
    if (!output_bytes)
        return NULL;
    cee_char* output_ptr = output_bytes;
    size_t output_bytes_left = output_length;
    size_t nconv = iconv(cd,
                         &origin,
                         &input_length,
                         &output_ptr,
                         &output_bytes_left);
    if (nconv == (size_t) - 1) {
        cee_free(output_bytes);
        output_bytes = NULL;
    }
    iconv_close(cd);
    output_bytes[output_length - output_bytes_left] = '\0';
        
    return (cee_uchar*)output_bytes;
}

cee_boolean cee_codec_convert_from_utf8(const cee_uchar* subject,
                                        cee_size length,
                                        const cee_char* encode_type,
                                        cee_uchar** converted_bytes,
                                        cee_size* converted_length)
{
    cee_boolean ret = FALSE;
    const cee_char* from = "UTF-8";
    const cee_char* to = encode_type;
    cee_char* origin = (cee_char*)subject;
    
    if (!from)
        return FALSE;
    
    iconv_t cd = iconv_open(to, from);
    if (cd == (iconv_t)-1)
        return FALSE;
    
    size_t input_length = length;
    size_t output_length = input_length * 4;
    cee_char* output_bytes = (cee_char*)cee_malloc0(output_length);
    if (!output_bytes)
        return FALSE;
    cee_char* output_ptr = output_bytes;
    size_t output_bytes_left = output_length;
    size_t nconv = iconv(cd,
                         &origin,
                         &input_length,
                         &output_ptr,
                         &output_bytes_left);
    if (nconv == (size_t) - 1) {
        cee_free(output_bytes);
        output_bytes = NULL;
        output_length = 0;
        output_bytes_left = 0;
        goto exit;
    }
    
    ret = TRUE;
    
exit:
    iconv_close(cd);
    
    *converted_bytes = (cee_uchar*)output_bytes;
    *converted_length = output_length - output_bytes_left;
    
    return ret;
}

const cee_char* encode_types[] = {
    "ANSI_X3.4-1968",
    "ANSI_X3.4-1986",
    "CP367",
    "IBM367",
    "ISO-IR-6",
    "ISO646-US",
    "ISO_646.IRV:1991",
    "US",
    "US-ASCII",
    "CSASCII",
    "ISO-10646-UCS-2",
    "UCS-2",
    "CSUNICODE",
    "UCS-2BE",
    "UNICODE-1-1",
    "UNICODEBIG",
    "CSUNICODE11",
    "UCS-2LE",
    "UNICODELITTLE",
    "ISO-10646-UCS-4",
    "UCS-4",
    "CSUCS4",
    "UCS-4BE",
    "UCS-4LE",
    "UTF-16",
    "UTF-16BE",
    "UTF-16LE",
    "UTF-3",
    "UTF-32BE",
    "UTF-32LE",
    "UNICODE-1-1-UTF-7",
    "UTF-7",
    "CSUNICODE11UTF7",
    "UCS-2-INTERNAL",
    "UCS-2-SWAPPED",
    "UCS-4-INTERNAL",
    "UCS-4-SWAPPED",
    "C99",
    "JAVA",
    "CP819",
    "IBM819",
    "ISO-8859-1",
    "ISO-IR-100",
    "ISO8859-1",
    "ISO_8859-1",
    "ISO_8859-1:1987",
    "L1",
    "LATIN1",
    "CSISOLATIN1",
    "ISO-8859-2",
    "ISO-IR-101",
    "ISO8859-2",
    "ISO_8859-2",
    "ISO_8859-2:1987",
    "L2",
    "LATIN2",
    "CSISOLATIN2",
    "ISO-8859-3",
    "ISO-IR-109",
    "ISO8859-3",
    "ISO_8859-3",
    "ISO_8859-3:1988",
    "L3",
    "LATIN3",
    "CSISOLATIN3",
    "ISO-8859-4",
    "ISO-IR-110",
    "ISO8859-4",
    "ISO_8859-4",
    "ISO_8859-4:1988",
    "L4",
    "LATIN4",
    "CSISOLATIN4",
    "CYRILLIC",
    "ISO-8859-5",
    "ISO-IR-144",
    "ISO8859-5",
    "ISO_8859-5",
    "ISO_8859-5:1988",
    "CSISOLATINCYRILLIC",
    "ARABIC",
    "ASMO-708",
    "ECMA-114",
    "ISO-8859-6",
    "ISO-IR-127",
    "ISO8859-6",
    "ISO_8859-6",
    "ISO_8859-6:1987",
    "CSISOLATINARABIC",
    "ECMA-118",
    "ELOT_928",
    "GREEK",
    "GREEK8",
    "ISO-8859-7",
    "ISO-IR-126",
    "ISO8859-7",
    "ISO_8859-7",
    "ISO_8859-7:1987",
    "ISO_8859-7:2003",
    "CSISOLATINGREEK",
    "HEBREW",
    "ISO-8859-8",
    "ISO-IR-138",
    "ISO8859-8",
    "ISO_8859-8",
    "ISO_8859-8:1988",
    "CSISOLATINHEBREW",
    "ISO-8859-9",
    "ISO-IR-148",
    "ISO8859-9",
    "ISO_8859-9",
    "ISO_8859-9:1989",
    "L5",
    "LATIN5",
    "CSISOLATIN5",
    "ISO-8859-10",
    "ISO-IR-157",
    "ISO8859-10",
    "ISO_8859-10",
    "ISO_8859-10:1992",
    "L6",
    "LATIN6",
    "CSISOLATIN6",
    "ISO-8859-11",
    "ISO8859-11",
    "ISO_8859-11",
    "ISO-8859-13",
    "ISO-IR-179",
    "ISO8859-13",
    "ISO_8859-13",
    "L7",
    "LATIN7",
    "ISO-8859-14",
    "ISO-CELTIC",
    "ISO-IR-199",
    "ISO8859-14",
    "ISO_8859-14",
    "ISO_8859-14:1998",
    "L8",
    "LATIN8",
    "ISO-8859-15",
    "ISO-IR-203",
    "ISO8859-15",
    "ISO_8859-15",
    "ISO_8859-15:1998",
    "LATIN-9",
    "ISO-8859-16",
    "ISO-IR-226",
    "ISO8859-16",
    "ISO_8859-16",
    "ISO_8859-16:2001",
    "L10",
    "LATIN10",
    "KOI8-R",
    "CSKOI8R",
    "KOI8-U",
    "KOI8-RU",
    "CP1250",
    "MS-EE",
    "WINDOWS-1250",
    "CP1251",
    "MS-CYRL",
    "WINDOWS-1251",
    "CP1252",
    "MS-ANSI",
    "WINDOWS-1252",
    "CP1253",
    "MS-GREEK",
    "WINDOWS-1253",
    "CP1254",
    "MS-TURK",
    "WINDOWS-1254",
    "CP1255",
    "MS-HEBR",
    "WINDOWS-1255",
    "CP1256",
    "MS-ARAB",
    "WINDOWS-1256",
    "CP1257",
    "WINBALTRIM",
    "WINDOWS-1257",
    "CP1258",
    "WINDOWS-1258",
    "850",
    "CP850",
    "IBM850",
    "CSPC850MULTILINGUAL",
    "862",
    "CP862",
    "IBM862",
    "CSPC862LATINHEBREW",
    "866",
    "CP866",
    "IBM866",
    "CSIBM866",
    "CP1131",
    "MAC",
    "MACINTOSH",
    "MACROMAN",
    "CSMACINTOSH",
    "MACCENTRALEUROPE",
    "MACICELAND",
    "MACCROATIAN",
    "MACROMANIA",
    "MACCYRILLIC",
    "MACUKRAINE",
    "MACGREEK",
    "MACTURKISH",
    "MACHEBREW",
    "MACARABIC",
    "MACTHAI",
    "HP-ROMAN8",
    "R8",
    "ROMAN8",
    "CSHPROMAN8",
    "NEXTSTEP",
    "ARMSCII-8",
    "GEORGIAN-ACADEMY",
    "GEORGIAN-PS",
    "KOI8-T",
    "CP154",
    "CYRILLIC-ASIAN",
    "PT154",
    "PTCP154",
    "CSPTCP154",
    "KZ-1048",
    "RK1048",
    "STRK1048-2002",
    "CSKZ1048",
    "MULELAO-1",
    "CP1133",
    "IBM-CP1133",
    "ISO-IR-166",
    "TIS-620",
    "TIS620",
    "TIS620-0",
    "TIS620.2529-1",
    "TIS620.2533-0",
    "TIS620.2533-1",
    "CP874",
    "WINDOWS-874",
    "VISCII",
    "VISCII1.1-1",
    "CSVISCII",
    "TCVN",
    "TCVN-5712",
    "TCVN5712-1",
    "TCVN5712-1:1993",
    "ISO-IR-14",
    "ISO646-JP",
    "JIS_C6220-1969-RO",
    "JP",
    "CSISO14JISC6220RO",
    "JISX0201-1976",
    "JIS_X0201",
    "X0201",
    "CSHALFWIDTHKATAKANA",
    "ISO-IR-87",
    "JIS0208",
    "JIS_C6226-1983",
    "JIS_X0208",
    "JIS_X0208-1983",
    "JIS_X0208-1990",
    "X0208",
    "CSISO87JISX0208",
    "ISO-IR-159",
    "JIS_X0212",
    "JIS_X0212-1990",
    "JIS_X0212.1990-0",
    "X0212",
    "CSISO159JISX02121990",
    "CN",
    "GB_1988-80",
    "ISO-IR-57",
    "ISO646-CN",
    "CSISO57GB1988",
    "CHINESE",
    "GB_2312-80",
    "ISO-IR-58",
    "CSISO58GB231280",
    "CN-GB-ISOIR165",
    "ISO-IR-165",
    "ISO-IR-149",
    "KOREAN",
    "KSC_5601",
    "KS_C_5601-1987",
    "KS_C_5601-1989",
    "CSKSC56011987",
    "EUC-JP",
    "EUCJP",
    "EXTENDED_UNIX_CODE_PACKED_FORMAT_FOR_JAPANESE",
    "CSEUCPKDFMTJAPANESE",
    "MS_KANJI",
    "SHIFT-JIS",
    "SHIFT_JIS",
    "SJIS",
    "CSSHIFTJIS",
    "CP932",
    "ISO-2022-JP",
    "CSISO2022JP",
    "ISO-2022-JP-1",
    "ISO-2022-JP-2",
    "CSISO2022JP2",
    "CP50221",
    "ISO-2022-JP-MS",
    "CN-GB",
    "EUC-CN",
    "EUCCN",
    "GB2312",
    "CSGB2312",
    "GBK",
    "CP936",
    "MS936",
    "WINDOWS-936",
    "GB18030",
    "ISO-2022-CN",
    "CSISO2022CN",
    "ISO-2022-CN-EXT",
    "HZ",
    "HZ-GB-2312",
    "EUC-TW",
    "EUCTW",
    "CSEUCTW",
    "BIG-5",
    "BIG-FIVE",
    "BIG5",
    "BIGFIVE",
    "CN-BIG5",
    "CSBIG5",
    "CP950",
    "BIG5-HKSCS:1999",
    "BIG5-HKSCS:2001",
    "BIG5-HKSCS:2004",
    "BIG5-HKSCS",
    "BIG5-HKSCS:2008",
    "BIG5HKSCS",
    "EUC-KR",
    "EUCKR",
    "CSEUCKR",
    "CP949",
    "UHC",
    "CP1361",
    "JOHAB",
    "ISO-2022-KR",
    "CSISO2022KR",
    NULL
};

cee_uchar* cee_codec_convert_to_utf8(const cee_uchar* subject,
                                     const cee_char* encode_type)
{
    cee_boolean ret = FALSE;
    const cee_char* to = "UTF-8";
    cee_char* origin = (cee_char*)subject;
    size_t input_length = strlen((cee_char*)subject);
    size_t output_length = input_length * 4 + 1;
    cee_char* output_bytes = (cee_char*)cee_malloc0(output_length);
    
    if (!output_bytes)
        return NULL;
        
    iconv_t cd = iconv_open(to, encode_type);
    if (cd == (iconv_t)-1)
        goto exit;
    
    cee_char* output_ptr = output_bytes;
    size_t output_bytes_left = output_length;
    size_t nconv = iconv(cd,
                            &origin,
                            &input_length,
                            &output_ptr,
                            &output_bytes_left);
    if (nconv == (size_t) - 1)
        goto exit;
    
    ret = TRUE;
        
exit:
    if (cd != (iconv_t)-1)
        iconv_close(cd);
    
    if (!ret) {
        cee_free(output_bytes);
        output_bytes = NULL;
    }
    
    return (cee_uchar*)output_bytes;
}
