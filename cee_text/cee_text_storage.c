#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "cee_text_storage.h"
#include "cee_bst.h"

typedef struct _CEETextStorage {
    cee_uchar* buffer;
    cee_ulong size;
    cee_ulong nb_paragraph;
    cee_ulong max_paragraph_length;
    CEETextStorageLineBreakType line_break_type;
    cee_pointer observer;
    void (*update_notify)(cee_pointer,
                          CEETextStorageRef,
                          CEERange,
                          CEERange);
    CEETextModifyLoggerRef modify_logger;
    CEEList* paragraph_indexes;
} CEETextStorage;

static cee_uchar line_break_codepoint_get(CEETextStorage* storage);
static void storage_buffer_decode(const cee_uchar* buffer,
                                  cee_long current,
                                  CEEUnicodePoint* codepoint,
                                  cee_ulong* length,
                                  cee_long* next);
static void storage_buffer_decode_reversed(const cee_uchar* buffer,
                                           cee_long current,
                                           CEEUnicodePoint* codepoint,
                                           cee_ulong* length,
                                           cee_long* prev);
static cee_long storage_paragraph_beginning_get(const cee_uchar* buffer,
                                                cee_long offset);
static cee_long storage_paragraph_end_get(const cee_uchar* buffer,
                                          cee_long offset);
static CEETextStorageLineBreakType line_break_type_default(void);
static CEETextStorageLineBreakType line_break_type_check(const cee_uchar* buffer);
static cee_uchar* replace_line_break(const cee_uchar* buffer,
                                     CEETextStorageLineBreakType target_type);
static cee_ulong paragraph_count(const cee_uchar* buffer,
                                 CEERange range);
static cee_ulong max_paragraph_length(const cee_uchar* buffer,
                                      CEERange range);
static cee_ulong buffer_size(const cee_uchar* buffer);
static CEEList* paragraph_indexes_create(const cee_uchar* buffer);

static cee_uchar line_break_codepoint_get(CEETextStorage* storage)
{
    cee_uchar ln = '\n';
    if (storage->line_break_type == kCEETextStorageLineBreakTypeLF ||
        storage->line_break_type == kCEETextStorageLineBreakTypeCRLF)
        ln = '\n';
    else if (storage->line_break_type == kCEETextStorageLineBreakTypeCR)
        ln = '\r';
    
    return ln;
}

static void storage_buffer_decode(const cee_uchar* buffer,
                                  cee_long current,
                                  CEEUnicodePoint* codepoint,
                                  cee_ulong* length,
                                  cee_long* next)
{
    CEEUnicodePoint _codepoint = CEE_UNICODE_POINT_INVALID;
    cee_size _length = 0;
    cee_long _next = -1;
    
    cee_codec_utf8_decode(buffer, current, &_codepoint, &_length, &_next);
    
    /** "current" is the end of storage  */
    if (_next == -1)
        goto exit;
    
    if (_codepoint == CEE_UNICODE_POINT_CR) {
        cee_long _offset = -1;
        cee_ulong lf_length = _length;
        
        cee_codec_utf8_decode(buffer, _next, &_codepoint, &_length, &_offset);
        if (_codepoint == CEE_UNICODE_POINT_LF) {
            /** CRLF linefeed */
            _next = _offset;
            lf_length += _length;
        }
        /**
         *  we use CEE_UNICODE_POINT_LF as the linefeed codepoint, no mater
         *  the actually linefeed type is "CR", "LF" or "CRLF"
         */
        _codepoint = CEE_UNICODE_POINT_LF;
        _length = lf_length;
    }
    
exit:
    
    if (next)
        *next = _next;
    
    if (codepoint)
        *codepoint = _codepoint;
    
    if (length)
        *length = _length;
    
    return;
}

static void storage_buffer_decode_reversed(const cee_uchar* buffer,
                                           cee_long current,
                                           CEEUnicodePoint* codepoint,
                                           cee_ulong* length,
                                           cee_long* prev)
{
    CEEUnicodePoint _codepoint = CEE_UNICODE_POINT_INVALID;
    cee_size _length = 0;
    cee_long _prev = -1;
    
    cee_codec_utf8_decode_reversed(buffer, current, &_codepoint, &_length, &_prev);
    
    /** "current" is the beginning of storage  */
    if (_prev == -1)
        goto exit;
    
    if (_codepoint == CEE_UNICODE_POINT_LF) {
        cee_long _offset = -1;
        cee_ulong lf_length = _length;
        
        cee_codec_utf8_decode_reversed(buffer, _prev, &_codepoint, &_length, &_offset);
        if (_codepoint == CEE_UNICODE_POINT_CR) {
            /** CRLF linefeed */
            _prev = _offset;
            lf_length += _length;
        }
        
        /**
         *  we use CEE_UNICODE_POINT_LF as the linefeed codepoint, no mater
         *  the actually linefeed type is "CR", "LF" or "CRLF"
         */
        _codepoint = CEE_UNICODE_POINT_LF;
        _length = lf_length;
    }
    else if (_codepoint == CEE_UNICODE_POINT_CR) {
        _codepoint = CEE_UNICODE_POINT_LF;
    }
    
exit:
    
    if (prev)
        *prev = _prev;
    
    if (codepoint)
        *codepoint = _codepoint;
    
    if (length)
        *length = _length;
    
    return;
}

static cee_long storage_paragraph_beginning_get(const cee_uchar* buffer,
                                                cee_long offset)
{
    cee_long current = -1;
    cee_long prev = -1;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    
    current = cee_codec_utf8_encoded_byte0_get(buffer, offset);
    while (TRUE) {
        storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, &prev);
        /** 
         * "prev" equals -1 means "current" is the beginning of buffer
         */
        if (prev == -1 || codepoint == CEE_UNICODE_POINT_LF)
            break;
        
        current = prev;
    }
    
    return current;
}

static cee_long storage_paragraph_end_get(const cee_uchar* buffer,
                                          cee_long offset)
{
    cee_long current = -1;
    cee_long next = -1;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    
    current = cee_codec_utf8_encoded_byte0_get(buffer, offset);
    while (TRUE) {
        storage_buffer_decode(buffer, current, &codepoint, NULL, &next);
        /**
         * "next" equals -1 means "current" is the end of buffer,
         * codepoint is CEE_UNICODE_POINT_NUL
         */
        if (next == -1 || codepoint == CEE_UNICODE_POINT_LF)
            break;
        
        current = next;
    }
    
    return current;
}

static CEETextStorageLineBreakType line_break_type_default(void)
{
#ifdef __WIN32__
    return kCEETextStorageLineBreakTypeCRLF;
#elif __linux__
    return kCEETextStorageLineBreakTypeLF;
#elif __APPLE__
    return kCEETextStorageLineBreakTypeLF;
#endif
}

static CEETextStorageLineBreakType line_break_type_check(const cee_uchar* buffer)
{
    const cee_uchar* ptr = buffer;
    cee_ulong i = 0;
    while (ptr[i]) {
        if (ptr[i] == '\r') {
            if (ptr[i+1] && ptr[i+1] == '\n') /** CRLF */
                return kCEETextStorageLineBreakTypeCRLF;
            else /** CR */
                return kCEETextStorageLineBreakTypeCR;
        }
        
        if (ptr[i] == '\n')
            return kCEETextStorageLineBreakTypeLF;
        
        i ++;
    }
    
    return line_break_type_default();
}

static cee_uchar* replace_line_break(const cee_uchar* buffer,
                                     CEETextStorageLineBreakType target_type)
{
    CEETextStorageLineBreakType type = line_break_type_check(buffer);
    cee_char* str = NULL;
    
    if (type == target_type)
        str = cee_strdup((cee_char*)buffer);
    else {
        if (type == kCEETextStorageLineBreakTypeLF) {
            if (target_type == kCEETextStorageLineBreakTypeCR)
                str = cee_str_replace_occurrence((cee_char*)buffer, "\n", "\r");
            else if (target_type == kCEETextStorageLineBreakTypeCRLF)
                str = cee_str_replace_occurrence((cee_char*)buffer, "\n", "\r\n");
        }
        else if (type == kCEETextStorageLineBreakTypeCR) {
            if (target_type == kCEETextStorageLineBreakTypeCRLF)
                str = cee_str_replace_occurrence((cee_char*)buffer, "\r", "\r\n");
            else if (target_type == kCEETextStorageLineBreakTypeLF)
                str = cee_str_replace_occurrence((cee_char*)buffer, "\r", "\n");
        }
        else if (type == kCEETextStorageLineBreakTypeCRLF) {
            if (target_type == kCEETextStorageLineBreakTypeCR)
                str = cee_str_replace_occurrence((cee_char*)buffer, "\r\n", "\r");
            else if (target_type == kCEETextStorageLineBreakTypeLF)
                str = cee_str_replace_occurrence((cee_char*)buffer, "\r\n", "\n");
        }
    }
    
    return (cee_uchar*)str;
}

static cee_ulong paragraph_count(const cee_uchar* buffer,
                                 CEERange range)
{
    if (!buffer || !range.length)
        return 0;
    
    cee_ulong count = 0;
    cee_long current = range.location;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    
    while (TRUE) {
        storage_buffer_decode(buffer, current, &codepoint, NULL, &current);
        
        if (codepoint == CEE_UNICODE_POINT_LF)
            count ++;
        
        if (current >= range.location + range.length || current == -1)
            break;
    }
        
    return count;
}

static cee_ulong max_paragraph_length(const cee_uchar* buffer,
                                      CEERange range)
{
    if (!buffer || !range.length)
        return 0;
    
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    cee_long beginning = -1;
    cee_long end = -1;
    cee_long current = -1;
    cee_ulong length = 0;
    cee_ulong i = 0;
    
    beginning = storage_paragraph_beginning_get(buffer, range.location);
    end = storage_paragraph_end_get(buffer, range.location + range.length - 1);
    
    current = beginning;
    
    while (TRUE) {
        storage_buffer_decode(buffer, current, &codepoint, NULL, &current);
        i ++;
        
        if (codepoint == CEE_UNICODE_POINT_LF) {
            i > length ? length = i : 0;
            i = 0;
        }
        
        if (current > end || current == -1) {
            i > length ? length = i : 0;
            break;
        }
    }
    
    return length;
}

static cee_ulong buffer_size(const cee_uchar* buffer)
{    
    return strlen((cee_char*)buffer);
}

void cee_text_storage_buffer_set(CEETextStorageRef storage,
                                 const cee_uchar* string)
{
    if (storage->buffer)
        cee_free(storage->buffer);
    
    if (storage->paragraph_indexes)
        cee_list_free(storage->paragraph_indexes);
    
    cee_text_storage_modify_clear(storage);
    
    CEERange range;
    storage->buffer = (cee_uchar*)cee_strdup((cee_char*)string);
    storage->line_break_type = line_break_type_check(storage->buffer);
    storage->size = buffer_size(storage->buffer);
    range = cee_range_make(0, storage->size);
    storage->nb_paragraph = paragraph_count(storage->buffer, range) + 1;
    storage->max_paragraph_length = max_paragraph_length(storage->buffer, range);
    storage->paragraph_indexes = paragraph_indexes_create(storage->buffer);
}

CEETextStorageRef cee_text_storage_create(cee_pointer observer,
                                          const cee_uchar* string,
                                          void (*update_notify)(cee_pointer,
                                                                CEETextStorageRef,
                                                                CEERange,
                                                                CEERange)
                                          )
{
    CEETextStorage* storage = (CEETextStorage*)cee_malloc0(sizeof(CEETextStorage));
    storage->observer = observer;
    storage->update_notify = update_notify;
    cee_text_storage_buffer_set(storage, string);
    return storage;
}

void cee_text_storage_free(CEETextStorageRef data)
{
    if (!data)
        return;
    
    CEETextStorage* storage = (CEETextStorage*)data;
    
    if (storage->modify_logger)
        cee_text_modify_logger_free(storage->modify_logger);
    
    if (storage->buffer)
        cee_free(storage->buffer);
    
    if (storage->paragraph_indexes)
        cee_list_free(storage->paragraph_indexes);
    
    cee_free(storage);
}

void cee_text_storage_buffer_replace(CEETextStorageRef storage,
                                     CEERange range,
                                     const cee_uchar* in,
                                     cee_uchar** out,
                                     cee_ulong* length,
                                     cee_boolean enable_update_notify)
{
    if (!storage || (!storage->buffer && !in))
        goto exit;
    
    cee_ulong replace_paragraph_count = 0;
    cee_ulong src_paragraph_count = 0;
    cee_ulong max_length = 0;
    cee_uchar* replacement = NULL;
    cee_ulong replacement_length = 0;
    CEERange replacement_range = cee_range_make(range.location, 0);
    cee_ulong nb_paragraph = storage->nb_paragraph;
    
    /** make the replacement buffer has the same line break character with the storage buffer */
    replacement = replace_line_break(in, storage->line_break_type);
    if (!replacement)
        goto exit;
    
    replacement_length = strlen((cee_char*)replacement);
        
    replace_paragraph_count = paragraph_count(replacement, cee_range_make(0, replacement_length));
    src_paragraph_count = paragraph_count(storage->buffer, range);
    
    /** do replacement */
    storage->buffer = (cee_uchar*)cee_str_replace((cee_char*)storage->buffer,
                                                  range.location,
                                                  range.length,
                                                  (cee_char*)replacement);
    
    
    /** update storage size and paragraph count  */
    if (range.length && !replacement_length) {
        /** delete */
        storage->size -= range.length;
        storage->nb_paragraph -= src_paragraph_count;
        replacement_range = cee_range_make(range.location, 0);
    }
    else if (!range.length && replacement_length) {
        /** insert */
        storage->size += replacement_length;
        storage->nb_paragraph += replace_paragraph_count;
        replacement_range = cee_range_make(range.location, replacement_length);
        max_length = max_paragraph_length(storage->buffer, replacement_range);
    }
    else if (range.length && replacement_length) {
        /** replace */
        storage->size += replacement_length - range.length;
        storage->nb_paragraph += replace_paragraph_count - src_paragraph_count;
        replacement_range = cee_range_make(range.location, replacement_length);
        max_length = max_paragraph_length(storage->buffer, replacement_range);
    }
    
    /** update storage max paragraph length */
    if (max_length > storage->max_paragraph_length)
        storage->max_paragraph_length = max_length;
    
    if (storage->update_notify && enable_update_notify) {
        storage->update_notify(storage->observer, storage, range, replacement_range);
    }
        
    if (!out)
        cee_free(replacement);
    else
        *out = replacement;
    
    if (length)
        *length = replacement_length;
    
    /** recreate pagraph indexes */
    if (nb_paragraph != storage->nb_paragraph) {
        if (storage->paragraph_indexes)
            cee_list_free(storage->paragraph_indexes);
        storage->paragraph_indexes = paragraph_indexes_create(storage->buffer);
    }
        
exit:
    return;
}

const cee_uchar* cee_text_storage_buffer_get(CEETextStorageRef storage)
{
    if (!storage || !storage->buffer)
        return NULL;
    
    return storage->buffer;
}

cee_ulong cee_text_storage_size_get(CEETextStorageRef storage)
{
    if (!storage || !storage->buffer)
        return 0;
    return storage->size;
}

cee_ulong cee_text_storage_character_count_get(CEETextStorageRef storage) {
    if (!storage || !storage->buffer)
        return 0;
    return cee_codec_utf8_nb_codepoint(storage->buffer, 0);
}

cee_ulong cee_text_storage_max_paragraph_length_get(CEETextStorageRef storage)
{
    if (!storage || !storage->buffer)
        return 0;
    return storage->max_paragraph_length;
}

cee_ulong cee_text_storage_paragraph_count_get(CEETextStorageRef storage)
{
    if (!storage || !storage->buffer)
        return 0;
    return storage->nb_paragraph;
}

cee_ulong cee_text_storage_line_break_length_get(CEETextStorageRef storage)
{
    if (!storage || !storage->buffer)
        return 0;
    
    switch (storage->line_break_type) {
        case kCEETextStorageLineBreakTypeCR:
        case kCEETextStorageLineBreakTypeLF:
            return 1;
        case kCEETextStorageLineBreakTypeCRLF:
            return 2;
        default:
            return 0;
    }
    return 0;
}

CEETextStorageLineBreakType cee_text_storage_line_break_type_get(CEETextStorageRef storage)
{
    if (!storage || !storage->buffer)
        return kCEETextStorageLineBreakTypeUnknow;
    
    return storage->line_break_type;
}
/**
 *  when return value equal -1, that means "current" is the end of storage
 */
void cee_text_storage_buffer_character_next(CEETextStorageRef storage,
                                            cee_long current,
                                            cee_long *next,
                                            CEEUnicodePoint* codepoint,
                                            cee_ulong* length)
{
    if (!storage || !storage->buffer)
        return;
    
    storage_buffer_decode(storage->buffer, current, codepoint, length, next);
}
/**
 *  when return value equal -1, that means "current" is the beginning of storage
 */
void cee_text_storage_buffer_character_prev(CEETextStorageRef storage,
                                            cee_long current,
                                            cee_long* prev,
                                            CEEUnicodePoint* codepoint,
                                            cee_ulong* length)
{
    if (!storage || !storage->buffer)
        return;
    
    storage_buffer_decode_reversed(storage->buffer, current, codepoint, length, prev);
}

void cee_text_storage_buffer_word_next(CEETextStorageRef storage,
                                       cee_long current,
                                       cee_long* next)
{
    if (!storage || !storage->buffer)
        return;
    
    const cee_uchar* buffer = storage->buffer;
    CEEUnicodePoint codepoint;
    CEEUnicodePointType type;
    
    storage_buffer_decode(buffer, current, &codepoint, NULL, next);
    if (*next == -1)
        return;
    
    type = cee_codec_unicode_point_type(codepoint);
    if (type == kCEEUnicodePointTypeWorldWideCharacter)
        return;
    
    current = *next;
    
    if (type == kCEEUnicodePointTypeASCIIPunctuation) {
        while (TRUE) {
            storage_buffer_decode(buffer, current, &codepoint, NULL, next);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeASCIIPunctuation || *next == -1)
                break;
            current = *next;
        }
    }
    else if (type == kCEEUnicodePointTypeControl) {
        while (TRUE) {
            storage_buffer_decode(buffer, current, &codepoint, NULL, next);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeControl || *next == -1)
                break;
            current = *next;
        }
    }
    else if (type == kCEEUnicodePointTypeASCIIAlphabet) {
        while (TRUE) {
            storage_buffer_decode(buffer, current, &codepoint, NULL, next);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeASCIIAlphabet || *next == -1)
                break;
            current = *next;
        }
    }
    return;
}

void cee_text_storage_buffer_word_prev(CEETextStorageRef storage,
                                       cee_long current,
                                       cee_long* prev)
{
    if (!storage || !storage->buffer)
        return;
    
    const cee_uchar* buffer = storage->buffer;
    CEEUnicodePoint codepoint;
    CEEUnicodePointType type;
    
    storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, prev);
    if (*prev == -1)
        return;
    
    type = cee_codec_unicode_point_type(codepoint);
    if (type == kCEEUnicodePointTypeWorldWideCharacter)
        return;
    
    current = *prev;
    
    if (type == kCEEUnicodePointTypeASCIIPunctuation) {
        while (TRUE) {
            storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, prev);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeASCIIPunctuation || *prev == -1)
                break;
            current = *prev;
        }
    }
    else if (type == kCEEUnicodePointTypeControl) {
        while (TRUE) {
            storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, prev);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeControl || *prev == -1)
                break;
            current = *prev;
        }
    }
    else if (type == kCEEUnicodePointTypeASCIIAlphabet) {
        while (TRUE) {
            storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, prev);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeASCIIAlphabet || *prev == -1)
                break;
            current = *prev;
        }
    }
    return;
}

cee_long cee_text_storage_paragraph_index_get(CEETextStorageRef storage,
                                              cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return 0;
    
    cee_uchar ln = line_break_codepoint_get(storage);
    cee_ulong i = 0;
    cee_uchar* p = storage->buffer;
    cee_ulong c = 0;
    
    while (*p) {
        if (*p == ln || *p == '\0')
            i ++;
        
        p ++;
        c ++;
        
        if (c >= buffer_offset)
            break;
    }
    
    return i;
}

cee_long cee_text_storage_buffer_offset_by_paragraph_index(CEETextStorageRef storage,
                                                           cee_long index)
{
    if (!storage->paragraph_indexes)
        return 0;
    return CEE_POINTER_TO_LONG(cee_list_nth_data(storage->paragraph_indexes, (cee_int)index));
}

cee_long cee_text_storage_paragraph_beginning_get(CEETextStorageRef storage,
                                                  cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return 0;
    return storage_paragraph_beginning_get(storage->buffer, buffer_offset);
}

cee_long cee_text_storage_paragraph_end_get(CEETextStorageRef storage,
                                            cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return 0;
    return storage_paragraph_end_get(storage->buffer, buffer_offset);
}

/**
 * when return value equals -1, that means the paragraph contain buffer_offset
 * is the first paragraph in the storage buffer.
 */
cee_long cee_text_storage_paragraph_prev_get(CEETextStorageRef storage,
                                             cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return 0;
    
    cee_long prev = -1;
    buffer_offset = cee_codec_utf8_encoded_byte0_get(storage->buffer, buffer_offset);
    buffer_offset = storage_paragraph_beginning_get(storage->buffer, buffer_offset);
    if (buffer_offset == 0)
        return -1;
    
    storage_buffer_decode_reversed(storage->buffer, buffer_offset, NULL, NULL, &prev);
    prev = storage_paragraph_beginning_get(storage->buffer, prev);
    return prev;
}

/**
 * when return value equals -1, that means the paragraph contain buffer_offset
 * is the last paragraph in the storage buffer.
 */
cee_long cee_text_storage_paragraph_next_get(CEETextStorageRef storage,
                                             cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return 0;
    
    cee_long next = -1;
    buffer_offset = cee_codec_utf8_encoded_byte0_get(storage->buffer, buffer_offset);
    buffer_offset = storage_paragraph_end_get(storage->buffer, buffer_offset);
    if (buffer_offset == -1)
        return -1;
    
    storage_buffer_decode(storage->buffer, buffer_offset, NULL, NULL, &next);
    if (next == -1)
        return -1;
    
    return next;
}

cee_long cee_text_storage_character_index_in_paragraph(CEETextStorageRef storage,
                                                       cee_long paragraph,
                                                       cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return 0;
    
    cee_long current = paragraph;
    cee_long next = 0;
    cee_long i = 0;
    
    while (TRUE) {
        
        if (buffer_offset == current)
            return i;
        
        storage_buffer_decode(storage->buffer, current, NULL, NULL, &next);
        current = next;
        i ++;
    }
    
    return i;
}

cee_long cee_text_storage_character_count_in_paragraph(CEETextStorageRef storage,
                                                       cee_long paragraph)
{
    if (!storage || !storage->buffer)
        return 0;
    
    cee_long current = paragraph;
    cee_long next = 0;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    cee_long count = 0;
    
    while (TRUE) {
        storage_buffer_decode(storage->buffer, current, &codepoint, NULL, &next);
        count ++;
        
        if (codepoint == CEE_UNICODE_POINT_LF || 
            codepoint == CEE_UNICODE_POINT_NUL)
            break;
        current = next;
    }
    
    return count;
}

cee_long cee_text_storage_buffer_offset_by_character_index(CEETextStorageRef storage,
                                                           cee_long paragraph,
                                                           cee_long index)
{
    if (!storage || !storage->buffer)
        return 0;
    
    cee_long current = paragraph;
    cee_long next = 0;
    cee_long i = 0;
    
    while (TRUE) {
        if (i == index)
            return current;
        
        storage_buffer_decode(storage->buffer, current, NULL, NULL, &next);
        current = next;
        
        i ++;
    }
    return -1;
}

void cee_text_storage_modify_logger_create(CEETextStorageRef storage,
                                           void (*modify_free)(cee_pointer))
{
    if (!storage || !storage->buffer)
        return;
    
    if (!storage->modify_logger)
        storage->modify_logger = cee_text_modify_logger_create(modify_free);
}

void cee_text_storage_modify_prepend(CEETextStorageRef storage,
                                     CEETextModifyRef modify)
{
    if (storage->modify_logger)
        cee_text_modify_prepend(storage->modify_logger, modify);
}

CEETextModifyRef cee_text_storage_modify_backward(CEETextStorageRef storage)
{
    if (storage->modify_logger)
        return cee_text_modify_backward(storage->modify_logger);
    return NULL;
}

CEETextModifyRef cee_text_storage_modify_forward(CEETextStorageRef storage)
{
    if (storage->modify_logger)
        return cee_text_modify_forward(storage->modify_logger);
    return NULL;
}

void cee_text_storage_modify_clear(CEETextStorageRef storage)
{
    cee_text_modify_logger_clear(storage->modify_logger);
}

CEERange cee_text_storage_word_get(CEETextStorageRef storage,
                                   cee_long buffer_offset)
{
    if (!storage || !storage->buffer)
        return cee_range_make(0, 0);
    
    CEERange range = cee_range_make(0, 0);
    const cee_uchar* buffer = storage->buffer;
    cee_long current = buffer_offset;
    cee_long prev = -1;
    cee_long next = -1;
    cee_ulong length = 0;
    CEEUnicodePoint codepoint;
    CEEUnicodePointType type;
    
    storage_buffer_decode(buffer, current, &codepoint, &length, &next);
    
    if (codepoint == CEE_UNICODE_POINT_NUL)
        return cee_range_make(buffer_offset, 0);
    
    type = cee_codec_unicode_point_type(codepoint);
    
    if (type == kCEEUnicodePointTypeWorldWideCharacter ||
        type == kCEEUnicodePointTypeASCIIPunctuation) {
        range = cee_range_make(current, length);
    }
    else if (type == kCEEUnicodePointTypeControl) {
        
        if (codepoint == CEE_UNICODE_POINT_SP || 
            codepoint == CEE_UNICODE_POINT_HT) {
            
            current = buffer_offset;
            while (TRUE) {
                storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, &prev);
                if ((codepoint != CEE_UNICODE_POINT_SP && 
                     codepoint != CEE_UNICODE_POINT_HT) || 
                    prev == -1)
                    break;
                
                current = prev;
            }
            range.location = current;
            
            current = buffer_offset;
            while (TRUE) {
                storage_buffer_decode(buffer, current, &codepoint, NULL, &next);
                
                if ((codepoint != CEE_UNICODE_POINT_SP && 
                     codepoint != CEE_UNICODE_POINT_HT) || 
                    next == -1)
                    break;
                
                current = next;
            }
            range.length = current - range.location;
        }
        else {
            range = cee_range_make(current, length);
        }
    }
    else if (type == kCEEUnicodePointTypeASCIIAlphabet) {
        current = buffer_offset;
        while (TRUE) {
            storage_buffer_decode_reversed(buffer, current, &codepoint, NULL, &prev);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeASCIIAlphabet || prev == -1)
                break;
            current = prev;
        }
        range.location = current;
        
        current = buffer_offset;
        while (TRUE) {
            storage_buffer_decode(buffer, current, &codepoint, NULL, &next);
            type = cee_codec_unicode_point_type(codepoint);
            if (type != kCEEUnicodePointTypeASCIIAlphabet || next == -1)
                break;
            current = next;
        }
        range.length = current - range.location;
    }
    
    return range;
}

static CEEList* paragraph_indexes_create(const cee_uchar* buffer)
{
    cee_long current = 0;
    cee_long next = 0;
    cee_long paragraph_buffer_offset = -1;
    CEEUnicodePoint codepoint = CEE_UNICODE_POINT_INVALID;
    CEEList* index_set = NULL;
    
    paragraph_buffer_offset = current;
    while (TRUE) {
        
        storage_buffer_decode(buffer, current, &codepoint, NULL, &next);
        
        if (codepoint == CEE_UNICODE_POINT_NUL) {
            /** paragraph_length contain NULL character */
            index_set = cee_list_prepend(index_set, CEE_LONG_TO_POINTER(paragraph_buffer_offset));
            break;
        }
        
        if (codepoint == CEE_UNICODE_POINT_LF) {
            index_set = cee_list_prepend(index_set, CEE_LONG_TO_POINTER(paragraph_buffer_offset));
            paragraph_buffer_offset = next;
        }
        current = next;
    }
    
    index_set = cee_list_reverse(index_set);
    return index_set;
}
