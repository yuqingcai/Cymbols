//
//  CEESourceBuffer.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/19.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEESourceBuffer.h"

NSNotificationName CEENotificationSourceBufferStateChanged = @"CEENotificationSourceBufferStateChanged";
NSNotificationName CEENotificationSourceBufferHighlightSymbol = @"CEENotificationSourceBufferHighlightSymbol";
NSNotificationName CEENotificationSourceBufferSymbolSelected = @"CEENotificationSourceBufferSymbolSelected";
NSNotificationName CEENotificationSourceBufferSymbolSearched = @"CEENotificationSourceBufferSymbolSearched";

@interface CEESourceBuffer()

@property (strong) NSString* modificationDate;

@end

@implementation CEESourceBuffer

static CEESymbolWrapper* symbol_wrapper_create(CEESourceSymbol* symbol,
                                               cee_int index,
                                               cee_uint level)
{
    CEESymbolWrapper* wrapper = cee_malloc0(sizeof(CEESymbolWrapper));
    wrapper->symbol_ref = symbol;
    wrapper->index = index;
    wrapper->level = level;
    return wrapper;
}

static void symbol_wrapper_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEESymbolWrapper* wrapper = data;
    cee_free(wrapper);
}

static void symbol_tree_dump(CEETree* tree, 
                             int n, 
                             CEEList** list)
{
    CEEList* p = NULL;    
    CEESourceSymbol* symbol = tree->data;
    if (symbol) {
        CEESymbolWrapper* wrapper = symbol_wrapper_create(symbol, 0, n);
        *list = cee_list_prepend(*list, wrapper);
    }
    
    n ++;
    if (tree->children) {
        p = CEE_TREE_CHILDREN_FIRST(tree);
        while (p) {
            tree = p->data;
            symbol_tree_dump(tree, n, list);
            p = CEE_TREE_NODE_NEXT(p);
        }
    }
}

static cee_int symbol_wrapper_compare(const cee_pointer a,
                                      const cee_pointer b)
{
    CEESymbolWrapper* wrapper0 = (CEESymbolWrapper*)a;
    CEESymbolWrapper* wrapper1 = (CEESymbolWrapper*)b;
    CEESourceSymbol* symbol0 = wrapper0->symbol_ref;
    CEESourceSymbol* symbol1 = wrapper1->symbol_ref;
    return cee_symbol_location_compare(symbol0, symbol1);
}

static void buffer_parse(CEESourceBuffer* buffer)
{
    CEESourceParserRef parser = buffer.parser_ref;
    if (!parser)
        return;
    
    cee_ulong m0 = cee_timestamp_ms();
    
    const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
    CEESourceFregment* statement = NULL;
    CEESourceFregment* prep_directive = NULL;
    CEESourceFregment* comment = NULL;
    CEEList* tokens_ref = NULL;
    CEESourceTokenMap* source_token_map = NULL;
    
    if (buffer.statement)
        cee_source_fregment_free_full(buffer.statement);
    
    if (buffer.prep_directive)
        cee_source_fregment_free_full(buffer.prep_directive);
    
    if (buffer.comment)
        cee_source_fregment_free_full(buffer.comment);
    
    if (buffer.tokens_ref)
        cee_list_free(buffer.tokens_ref);
    
    if (buffer.source_token_map)
        cee_source_token_map_free(buffer.source_token_map);
    
    cee_source_symbol_parse(parser, 
                            (const cee_uchar*)[buffer.filePath UTF8String],
                            subject, 
                            &statement, 
                            &prep_directive,
                            &comment, 
                            &tokens_ref, 
                            &source_token_map);
    
    buffer.statement = statement;
    buffer.prep_directive = prep_directive;
    buffer.comment = comment;
    buffer.tokens_ref = tokens_ref;
    buffer.source_token_map = source_token_map;
    
    cee_ulong m1 = cee_timestamp_ms();
    fprintf(stdout, "\nbuffer_parse cost: %lu ms\n", m1 - m0);
    
    m0 = cee_timestamp_ms();
    
    if (buffer.symbol_wrappers)
        cee_list_free_full(buffer.symbol_wrappers, symbol_wrapper_free);
    
    CEEList* symbol_wrappers = NULL;
    CEETree* tree = NULL;
    if (buffer.prep_directive) {
        tree = cee_source_fregment_symbol_tree_create(buffer.prep_directive);
        symbol_tree_dump(tree, 0, &symbol_wrappers);
        cee_tree_free(tree);
    }
    if (buffer.statement) {
        tree = cee_source_fregment_symbol_tree_create(buffer.statement);
        symbol_tree_dump(tree, 0, &symbol_wrappers);
        cee_tree_free(tree);
    }
    symbol_wrappers = cee_list_sort(symbol_wrappers, symbol_wrapper_compare);
    CEEList* p = symbol_wrappers;
    int i = 0;
    while (p) {
        CEESymbolWrapper* wrapper = p->data;
        wrapper->index = i;
        i ++;
        p = p->next;
    }
    
    
    buffer.symbol_wrappers = symbol_wrappers;
    m1 = cee_timestamp_ms();
    
    fprintf(stdout, "\nsymbols wrappers create cost: %lu ms\n", m1 - m0);
    
}

static void text_buffer_modified(cee_pointer buffer, 
                                 CEETextStorageRef storage,
                                 CEERange original,
                                 CEERange replacement) 
{
    buffer_parse((__bridge CEESourceBuffer*)buffer);
    [(__bridge CEESourceBuffer*)buffer setState:kCEESourceBufferStateModified];
}

static void binary_buffer_modified(cee_pointer buffer, 
                                   CEEBinaryStorageRef storage,
                                   CEERange original,
                                   CEERange replacement)
{
    
}

- (NSString*)UTCStringFromDate:(NSDate*)date {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    NSTimeZone *timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    [formatter setTimeZone:timeZone];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *string = [formatter stringFromDate:date];
    return string;
}

- (NSDate*)dateFromUTFString:(NSString*)string {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    NSTimeZone *timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    [formatter setTimeZone:timeZone];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSDate* date = [formatter dateFromString:string];
    return date;
}

- (instancetype)initWithFilePath:(nullable NSString*)filePath {
    
    self = [super init];
    if (!self)
        return nil;

    cee_ulong length = 0;
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    _project_ref = [delegate currentProject];
    _referenceCount = 0;
    
    if (!filePath) {
        _filePath = nil;
        _type = kCEEBufferTypeUTF8;
        _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                           (const cee_uchar*)"",
                                           text_buffer_modified);
        _states = kCEESourceBufferStateFileUntitled;
    }
    else {
        _filePath = filePath;
        _symbol_cache = cee_symbol_cache_create();
        cee_uchar* content = NULL;
        cee_file_contents_get([_filePath UTF8String], &content, &length);
        
        if (cee_codec_encoding_utf8(content, length)) {
            _type = kCEEBufferTypeUTF8;
            _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                               content,
                                               text_buffer_modified);
            _parser_ref = cee_source_parser_get([_filePath UTF8String]);
        }
        else {
            _type = kCEEBufferTypeBinary;
            _storage = cee_binary_storage_create((__bridge cee_pointer)self,
                                                 content,
                                                 length,
                                                 binary_buffer_modified);
        }
        _states = kCEESourceBufferStateNormal;
        buffer_parse(self);
        
        cee_free(content);
    }
    
    NSDate* modificationDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
    _modificationDate = [self UTCStringFromDate:modificationDate];
    return self;
}

- (void)referenceIncrease {
    _referenceCount ++;
}

- (void)referenceDecrease {
    _referenceCount --;
}

- (void)dealloc {
    
    if (_statement)
        cee_source_fregment_free_full(_statement);
    
    if (_prep_directive)
        cee_source_fregment_free_full(_prep_directive);
    
    if (_comment)
        cee_source_fregment_free_full(_comment);
    
    if (_tokens_ref)
        cee_list_free(_tokens_ref);
    
    if (_source_token_map)
        cee_source_token_map_free(_source_token_map);
    
    if (_storage)
        cee_text_storage_free(_storage);
    
    if (_symbolsSearched)
        cee_list_free_full(_symbolsSearched, cee_symbol_free);
    
    if (_symbol_cache)
        cee_symbol_cache_free(_symbol_cache);
}

- (void)setState:(CEESourceBufferState)state {
    _states |= state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferStateChanged object:self];
}

- (void)clearState:(CEESourceBufferState)state {
    _states &= ~state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferStateChanged object:self];
}

- (BOOL)stateSet:(CEESourceBufferState)state {
    return _states & state;
}

- (void)reload {
    cee_uchar* content = NULL;
    cee_file_contents_get([_filePath UTF8String], &content, NULL);
    cee_text_storage_buffer_set(_storage, content);
    buffer_parse(self);
    cee_free(content);
    [self setState:kCEESourceBufferStateReload];
    [self clearState:kCEESourceBufferStateReload];
}

- (BOOL)isFileModified {
    NSDate* date0 = [self dateFromUTFString:_modificationDate];
    NSDate* date1 = [self dateFromUTFString:[self UTCStringFromDate:[[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate]]];
    
    if ([date0 compare:date1] != NSOrderedSame)
        return YES;
    
    return NO;
}

- (void)updateFileModifiedDate {
    NSDate* modificationDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
    _modificationDate = [self UTCStringFromDate:modificationDate];
}

- (cee_uint)symbolWrapperLength {
    return cee_list_length(_symbol_wrappers);
}

- (CEESymbolWrapper*)symbolWrapperByIndex:(NSInteger)index {
    return cee_list_nth(_symbol_wrappers,  (cee_uint)index)->data;
}

- (void)highlightSymbol:(CEESourceSymbol*)symbol {
    _highlightedSymbol = symbol;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferHighlightSymbol object:self];
}

@end

@interface CEESourceBufferManager()
@property NSUInteger untitledIndex;
@end

@implementation CEESourceBufferManager


- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
        
    _buffers = [[NSMutableArray alloc] init];
    _untitledIndex = 0;
    return self;
}

- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString*)filePath {
    if (!filePath || ![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return nil;
    
    for (CEESourceBuffer* buffer in _buffers) {
        if ([buffer.filePath isEqualToString:filePath]) {
            [buffer referenceIncrease];
            return buffer;
        }
    }
    
    CEESourceBuffer* buffer = [[CEESourceBuffer alloc] initWithFilePath:filePath];
    if (!buffer)
        return nil;
    [buffer referenceIncrease];
    [_buffers addObject:buffer];
    
    return buffer;
}

- (CEESourceBuffer*)openUntitledSourceBuffer {
    CEESourceBuffer* buffer = [[CEESourceBuffer alloc] initWithFilePath:nil];
    [buffer setFilePath:[NSString stringWithFormat:@"Untitled%lu", _untitledIndex]];
    [buffer setState:kCEESourceBufferStateFileUntitled];
    [buffer referenceIncrease];
    [_buffers addObject:buffer];
    _untitledIndex ++;
    return buffer;
}

- (void)closeSourceBuffer:(CEESourceBuffer*)buffer {
    [buffer referenceDecrease];
    if (!buffer.referenceCount)
        [_buffers removeObject:buffer];
}

- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filepath {
    NSString* filePathBeforeSave = buffer.filePath;
    
    [[NSString stringWithUTF8String:(const char *)cee_text_storage_buffer_get(buffer.storage)] writeToURL:[NSURL fileURLWithPath:filepath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    [buffer updateFileModifiedDate];
    
    if (![filePathBeforeSave isEqualToString:filepath]) {
        buffer.filePath = filepath;
        [buffer setState:kCEESourceBufferStateFilePathChanged];
        [buffer clearState:kCEESourceBufferStateFilePathChanged];
    }
        
    if ([buffer stateSet:kCEESourceBufferStateFileUntitled])
        [buffer clearState:kCEESourceBufferStateFileUntitled];
    
    if ([buffer stateSet:kCEESourceBufferStateModified])
        [buffer clearState:kCEESourceBufferStateModified];
    
    if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
        [buffer clearState:kCEESourceBufferStateFileDeleted];
    
}

- (void)updateSourceBuffers {
    for (CEESourceBuffer* buffer in _buffers) {
        if (![buffer stateSet:kCEESourceBufferStateFileUntitled]) {
            if (![[NSFileManager defaultManager] fileExistsAtPath:buffer.filePath]) {
                if (![buffer stateSet:kCEESourceBufferStateFileDeleted])
                    [buffer setState:kCEESourceBufferStateFileDeleted];
            }
            else {
                if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
                    [buffer clearState:kCEESourceBufferStateFileDeleted];
            }
        }
        
        if ([buffer isFileModified]) {
            [buffer updateFileModifiedDate];
            [buffer reload];
        }
    }
}

- (BOOL)fileExistsAtPath:(NSString*)filePath {
    return [[NSFileManager defaultManager] fileExistsAtPath:filePath];
}

@end
