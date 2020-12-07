#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <iconv.h>
#include "cee_codec.h"

cee_boolean cee_codec_encoding_utf8(const cee_uchar* subject,
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

/**
 * when return value equals -1, that means "offset" is the tail of storage buffer,
 * the "codepoint" is CEE_UNICODE_POINT_NUL
 */
cee_long cee_codec_utf8_decode_next(const cee_uchar* subject,
                                    cee_long offset,
                                    CEEUnicodePoint* codepoint,
                                    cee_size* length)
{
    const cee_uchar* ptr = &subject[offset];
    cee_size _length = 0;
    CEEUnicodePoint _codepoint = CEE_UNICODE_POINT_INVALID;
    cee_long next = 0;
    
    if (offset < 0)
        return -1;
    
    assert(!(ptr[0] & 0x80) || 
           (ptr[0] >> 5) == 0x6 || 
           (ptr[0] >> 4) == 0xe ||
           (ptr[0] >> 3) == 0x1e);
    
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
    
    if (_codepoint == CEE_UNICODE_POINT_NUL)
        next = -1;
    else
        next = offset + _length;
    
    if (codepoint)
        *codepoint = _codepoint;
    
    if (length)
        *length = _length;
    
    return next;
}
/**
 * when return value equals -1, that means "offset" is the head of storage buffer,
 * the "codepoint" is CEE_UNICODE_POINT_INVALID
 */
cee_long cee_codec_utf8_decode_prev(const cee_uchar* subject,
                                    cee_long offset,
                                    CEEUnicodePoint* codepoint,
                                    cee_size* length)
{
    if (offset < 0)
        return -1;
    
    if (offset == 0) {

        if (codepoint)
            *codepoint = CEE_UNICODE_POINT_INVALID;
        
        if (length)
            *length = 0;
        
        return -1;
    }
    
    offset --;
    offset = cee_codec_utf8_encoded_byte0_get(subject, offset);
    cee_codec_utf8_decode_next(subject,
                               offset,
                               codepoint,
                               length);
    
    return offset;
}

cee_ulong cee_codec_utf8_decode_length(const cee_uchar* subject,
                                      cee_long offset)
{
    const cee_uchar* ptr = &subject[offset];
    cee_size length = 0;
    
    if (offset < 0)
        return 0;
    
    assert(*ptr == 0 || 
           !(ptr[0] & 0x80) || 
           (ptr[0] >> 5) == 0x6 || 
           (ptr[0] >> 4) == 0xe ||
           (ptr[0] >> 3) == 0x1e);
    
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
    cee_ulong offset = 0;
    cee_ulong total_decode_length = 0;
    
    while (TRUE) {
        CEEUnicodePoint codepoint;
        cee_ulong decode_length;
        offset = cee_codec_utf8_decode_next(subject,
                                            offset,
                                            &codepoint,
                                            &decode_length);
        total_decode_length += decode_length;
        
        if (codepoint == CEE_UNICODE_POINT_NUL)
            break;
        
        nb_decoded ++;
        
        if (length && total_decode_length >= length)
            break;
        
    }
    return nb_decoded;
}

cee_long cee_codec_utf8_encoded_byte0_get(const cee_uchar* subject,
                                          cee_long offset)
{
    while (TRUE) {
        
        if (!(subject[offset] & 0x80) ||
            (subject[offset] >> 5) == 0x6 ||
            (subject[offset] >> 4) == 0xe ||
            (subject[offset] >> 3) == 0x1e)
            break;
            
        offset --;
        
        assert(offset >= 0);
    }
    
    return offset;
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

void cee_codec_export_bom(const cee_uchar* buffer,
                          cee_uchar* bom)
{
    bom[0] = 0x00;
    bom[1] = 0x00;
    bom[2] = 0x00;
    bom[3] = 0x00;
    
    if (buffer[0] == 0xEF &&
        buffer[1] == 0xBB &&
        buffer[2] == 0xBF) {
        /** UTF-8 */
        bom[0] = buffer[0];
        bom[1] = buffer[1];
        bom[2] = buffer[2];
    }
    else if (buffer[0] == 0xFE &&
             buffer[1] == 0xFF) {
        /** UTF-16 (big-endian) */
        bom[0] = buffer[0];
        bom[1] = buffer[1];
    }
    else if (buffer[0] == 0xFF &&
             buffer[1] == 0xFE) {
        /** UTF-16 (little-endian) */
        bom[0] = buffer[0];
        bom[1] = buffer[1];
    }
    else if (buffer[0] == 0x00 &&
             buffer[1] == 0x00 &&
             buffer[2] == 0xFE &&
             buffer[3] == 0xFF) {
        /** UTF-32 (big-endian) */
        bom[0] = buffer[0];
        bom[1] = buffer[1];
        bom[2] = buffer[2];
        bom[3] = buffer[3];
    }
    else if (buffer[0] == 0xFF &&
             buffer[1] == 0xFE &&
             buffer[2] == 0x00 &&
             buffer[3] == 0x00) {
        /** UTF-32 (little-endian) */
        bom[0] = buffer[0];
        bom[1] = buffer[1];
        bom[2] = buffer[2];
        bom[3] = buffer[3];
    }
}

CEEBufferEncodeType cee_codec_type_from_bom(const cee_uchar* bom)
{
    if (bom[0] == 0xEF &&
        bom[1] == 0xBB &&
        bom[2] == 0xBF) {
        /** UTF-8 */
        return kCEEBufferEncodeTypeUTF8;
    }
    else if (bom[0] == 0xFE &&
             bom[1] == 0xFF) {
        /** UTF-16 (big-endian) */
        return kCEEBufferEncodeTypeUTF16BE;
    }
    else if (bom[0] == 0xFF &&
             bom[1] == 0xFE) {
        /** UTF-16 (little-endian) */
        return kCEEBufferEncodeTypeUTF16LE;
    }
    else if (bom[0] == 0x00 &&
             bom[1] == 0x00 &&
             bom[2] == 0xFE &&
             bom[3] == 0xFF) {
        /** UTF-32 (big-endian) */
        return kCEEBufferEncodeTypeUTF32BE;
    }
    else if (bom[0] == 0xFF &&
             bom[1] == 0xFE &&
             bom[2] == 0x00 &&
             bom[3] == 0x00) {
        /** UTF-32 (little-endian) */
        return kCEEBufferEncodeTypeUTF32LE;
    }
    return kCEEBufferEncodeTypeUnknow;
}

cee_uchar* cee_codec_convert_to_utf8(const cee_uchar* subject,
                                     cee_size length,
                                     CEEBufferEncodeType type)
{
    /**
     * subject should begin with BOM
     */
    const cee_char* to = "UTF-8";
    const cee_char* from = NULL;
    cee_char* origin = (cee_char*)subject;
    cee_size mark_length = 0;
    
    if (type == kCEEBufferEncodeTypeUTF8) {
        from = "UTF-8";
        length -= 3;
        memmove(origin, origin + 3, length);
        origin[length] = '\0';
        return (cee_uchar*)cee_strdup(origin);
    }
    else if (type == kCEEBufferEncodeTypeUTF16BE) {
        from = "UTF-16BE";
        mark_length = 2;
    }
    else if (type == kCEEBufferEncodeTypeUTF16LE) {
        from = "UTF-16LE";
        mark_length = 2;
    }
    else if (type == kCEEBufferEncodeTypeUTF32BE) {
        from = "UTF-32BE";
        mark_length = 4;
    }
    else if (type == kCEEBufferEncodeTypeUTF32LE) {
        from = "UTF-32LE";
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
                                       CEEBufferEncodeType type,
                                       cee_uchar** converted_bytes,
                                       cee_size* converted_length)
{
    cee_boolean ret = FALSE;
    const cee_char* from = "UTF-8";
    const cee_char* to = NULL;
    cee_char* origin = (cee_char*)subject;
    
    if (type == kCEEBufferEncodeTypeUTF16BE)
        to = "UTF-16BE";
    else if (type == kCEEBufferEncodeTypeUTF16LE)
        to = "UTF-16LE";
    else if (type == kCEEBufferEncodeTypeUTF32BE)
        to = "UTF-32BE";
    else if (type == kCEEBufferEncodeTypeUTF32LE)
        to = "UTF-32LE";
    
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
