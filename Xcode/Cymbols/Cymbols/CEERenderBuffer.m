//
//  CEERenderBuffer.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/19.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEERenderBuffer.h"
#import "cee_source_parsers.h"
#import "cee_text_storage.h"
#import "cee_binary_storage.h"
#import "CEEProject.h"

@interface CEERenderBuffer()
@property CEEList* references;
@property CEEList* tags;
@property CEEBST* tags_bst;
@end

@implementation CEERenderBuffer
@synthesize sourceBuffer = _sourceBuffer;

static void tags_create(CEERenderBuffer* buffer,
                        CEETextLayoutRef layout,
                        CEERange range)
{
    CEESourceBuffer* source = buffer.sourceBuffer;
    CEETextStorageRef storage = source.storage;
    const cee_uchar* subject = cee_text_storage_buffer_get(storage);
    CEEList* references = NULL;
    CEEList* tags = NULL;
    
    CEESourceParserRef parser = source.parser_ref;
    if (!parser)
        return;
    
    if (buffer.references)
        cee_list_free_full(buffer.references, cee_reference_free);
    buffer.references = NULL;
    
    cee_source_reference_parse(parser,
                               (const cee_uchar*)[source.filePath UTF8String],
                               subject,
                               source.source_token_map,
                               source.statement,
                               source.prep_directive,
                               source.project_ref.database,
                               source.symbol_cache,
                               range,
                               &references);
    buffer.references = references;
    
    if (buffer.tags)
        cee_list_free_full(buffer.tags, cee_tag_free);
    buffer.tags = NULL;
    cee_source_tags_create(parser,
                           (const cee_uchar*)[source.filePath UTF8String],
                           subject,
                           source.source_token_map,
                           source.statement,
                           source.prep_directive,
                           source.project_ref.database,
                           source.symbol_cache,
                           range,
                           references,
                           &tags);
    buffer.tags = tags;
    
    if (buffer.tags_bst)
        cee_bst_free(buffer.tags_bst);
    buffer.tags_bst = cee_bst_create(buffer.tags);
    
    cee_text_layout_tags_bst_set(layout, buffer.tags_bst);
}
void render_buffer_tags_generate(cee_pointer generator,
                                 CEETextLayoutRef layout,
                                 CEERange range)
{
    tags_create((__bridge CEERenderBuffer*)generator, layout, range);
}


- (instancetype)initWithSourceBuffer:(CEESourceBuffer*)sourceBuffer {
    self = [super init];
    if (self) {
        _sourceBuffer = sourceBuffer;
    }
    return self;
}

- (void)setSourceBuffer:(CEESourceBuffer *)sourceBuffer {
    _sourceBuffer = sourceBuffer;
}

- (CEESourceBuffer*)sourceBuffer {
    return _sourceBuffer;
}

- (void)dealloc {
    if (_tags)
        cee_list_free_full(_tags, cee_tag_free);
    
    if (_tags_bst)
        cee_bst_free(_tags_bst);
    
    if (_references)
        cee_list_free_full(_references, cee_reference_free);
    
}

- (void)selectOffset:(cee_long)offset {
    /*CEEList* p = NULL;
    CEESymbolWrapper* wrapper = NULL;
    CEESymbolWrapper* scope = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* ranges = NULL;
    CEERange range;
    
    p = _symbol_wrappers;
    while (p) {
        wrapper = p->data;
        symbol = wrapper->symbol_ref;
        ranges = cee_ranges_from_string(symbol->locations);
        if (ranges) {
            range = cee_range_consistent_from_discrete(ranges);
            if (cee_location_in_range(offset, range)) {
                cee_list_free_full(ranges, cee_range_free);
                scope = p->data;
                break;
            }
            cee_list_free_full(ranges, cee_range_free);
        }
        p = p->next;
    }
    
    if (!p) {
        cee_long diff = LONG_MAX;
        CEEList* q = NULL;
        p = _symbol_wrappers;
        while (p) {
            wrapper = p->data;
            symbol = wrapper->symbol_ref;
            if (symbol->fregment_range) {
                ranges = cee_ranges_from_string(symbol->fregment_range);
                if (ranges) {
                    range = cee_range_consistent_from_discrete(ranges);
                    if (cee_location_in_range(offset, range))
                        q = cee_list_prepend(q, p->data);
                    cee_list_free_full(ranges, cee_range_free);
                }
            }
            p = p->next;
        }
        
        p = q;
        while (p) {
            wrapper = p->data;
            symbol = wrapper->symbol_ref;
            ranges = cee_ranges_from_string(symbol->fregment_range);
            if (ranges) {
                range = cee_range_consistent_from_discrete(ranges);
                if (offset - range.location < diff) {
                    diff = offset - range.location;
                    scope = wrapper;
                }
                cee_list_free_full(ranges, cee_range_free);
            }
            p = p->next;
        }
        cee_list_free(q);
    }
    
    CEESourceReference* reference = [self referenceByBufferOffset:offset];
    if (reference) {
        if (_symbolsSearched)
            cee_list_free_full(_symbolsSearched, cee_symbol_free);
        
        _symbolsSearched = cee_symbol_search_by_reference(reference,
                                                          _statement,
                                                          _prep_directive,
                                                          _project_ref.database,
                                                          _symbol_cache,
                                                          kCEESourceReferenceSearchOptionLocal);
        if (!_symbolsSearched) {
            _symbolsSearched = cee_symbol_search_by_reference(reference,
                                                              _statement,
                                                              _prep_directive,
                                                              _project_ref.database,
                                                              _symbol_cache,
                                                              kCEESourceReferenceSearchOptionGlobal);
        }
        
        if (_symbolsSearched)
            [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferSymbolSearched object:self];
        
    }
    
    if (scope) {
        _selectedSymbolIndex = scope->index;
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferSymbolSelected object:self];
    }*/
}

- (CEESourceReference*)referenceByBufferOffset:(cee_long)offset {
    CEESourceReference* target = NULL;
    /*CEEList* p = NULL;
    CEEList* q = NULL;
    CEESourceReference* reference = NULL;
    CEERange* range = NULL;
    
    p = _references;
    while (p) {
        reference = p->data;
        q = reference->locations;
        while (q) {
            range = q->data;
            if (cee_location_in_range(offset, *range))
                break;
            q = q->next;
        }
        
        if (q) {
            target = p->data;
            break;
        }
        
        p = p->next;
    }
    */
    return target;
}

@end
