//
//  CEESourceBuffer.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/19.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESourceBuffer.h"

NSNotificationName CEENotificationSourceBufferStateChanged = @"CEENotificationSourceBufferStateChanged";

@interface CEESourceBuffer()
@property (strong) NSString* modificationDate;
@property (readonly) NSUInteger referenceCount;
@end

@implementation CEESourceBuffer

@synthesize state = _state;

static CEESymbolWrapper* symbol_wrapper_create(CEESourceSymbol* symbol,
                                            cee_uint level)
{
    CEESymbolWrapper* wrapper = cee_malloc0(sizeof(CEESymbolWrapper));
    wrapper->symbol_ref = symbol;
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

static void symbol_tree_dump_to_wrappers(CEETree* tree, 
                                         int n, 
                                         CEEList** list)
{
    if (!tree)
        return;
    
    CEEList* p = NULL;    
    CEESourceSymbol* symbol = tree->data;
    if (symbol) {
        CEESymbolWrapper* wrapper = symbol_wrapper_create(symbol, n);
        *list = cee_list_prepend(*list, wrapper);
    }
    
    n ++;
    if (tree->children) {
        p = CEE_TREE_CHILDREN_FIRST(tree);
        while (p) {
            tree = p->data;
            symbol_tree_dump_to_wrappers(tree, n, list);
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
    return cee_source_symbol_location_compare(symbol0, symbol1);
}

void cee_source_buffer_parse(CEESourceBuffer* buffer,
                             CEESourceBufferParserOption options)
{
    CEESourceParserRef parser = buffer.parser_ref;
    if (!parser)
        return;
    
    cee_ulong m0 = 0;
    const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
    CEESourceFregment* statement = NULL;
    CEESourceFregment* prep_directive = NULL;
    CEESourceFregment* comment = NULL;
    CEEList* tokens_ref = NULL;
    CEESourceTokenMap* source_token_map = NULL;
    
    m0 = cee_timestamp_ms();
    /** parse buffer begin */
    if (buffer.comment)
        cee_source_fregment_free_full(buffer.comment);

    if (buffer.prep_directive)
        cee_source_fregment_free_full(buffer.prep_directive);
    
    if (buffer.statement)
        cee_source_fregment_free_full(buffer.statement);
    
    if (buffer.tokens_ref)
        cee_list_free(buffer.tokens_ref);
    
    if (buffer.source_token_map)
        cee_source_token_map_free(buffer.source_token_map);
    
    if (buffer.prep_directive_symbol_tree)
        cee_tree_free(buffer.prep_directive_symbol_tree);
    
    if (buffer.statement_symbol_tree)
        cee_tree_free(buffer.statement_symbol_tree);
    
    cee_source_symbol_parse(parser, 
                            (const cee_uchar*)[buffer.filePath UTF8String],
                            subject, 
                            &prep_directive,
                            &statement, 
                            &comment, 
                            &tokens_ref, 
                            &source_token_map);
    
    buffer.comment = comment;
    buffer.prep_directive = prep_directive;
    buffer.statement = statement;
    buffer.tokens_ref = tokens_ref;
    buffer.source_token_map = source_token_map;
    buffer.prep_directive_symbol_tree = cee_source_fregment_symbol_tree_create(buffer.prep_directive);
    buffer.statement_symbol_tree = cee_source_fregment_symbol_tree_create(buffer.statement);
    
    /** parse buffer end */
    cee_ulong m1 = cee_timestamp_ms();
    fprintf(stdout, "\nbuffer_parse cost: %lu ms\n", m1 - m0);
    
    if (options & kCEESourceBufferParserOptionCreateSymbolWrapper) {
        /** symbol wrappers create begin */
        if (buffer.symbol_wrappers)
            cee_list_free_full(buffer.symbol_wrappers, symbol_wrapper_free);
        
        CEEList* wrappers = NULL;
        symbol_tree_dump_to_wrappers(buffer.prep_directive_symbol_tree, 0, &wrappers);
        symbol_tree_dump_to_wrappers(buffer.statement_symbol_tree, 0, &wrappers);
        
        wrappers = cee_list_sort(wrappers, symbol_wrapper_compare);
        buffer.symbol_wrappers = wrappers;
        /** symbol wrappers create end */
    }
}

static void text_buffer_modified(cee_pointer buffer, 
                                 CEETextStorageRef storage,
                                 CEERange original,
                                 CEERange replacement) 
{
    cee_source_buffer_parse((__bridge CEESourceBuffer*)buffer,
                            kCEESourceBufferParserOptionCreateSymbolWrapper);
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
    _referenceCount = 0;
    
    if (!filePath) {
        _filePath = nil;
        _type = kCEEBufferTypeUTF8;
        _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                           (const cee_uchar*)"",
                                           text_buffer_modified);
        _state = kCEESourceBufferStateFileUntitled;
    }
    else {
        _filePath = filePath;
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
        _state = kCEESourceBufferStateNormal;
        
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
    if (_comment)
        cee_source_fregment_free_full(_comment);
    
    if (_prep_directive)
        cee_source_fregment_free_full(_prep_directive);
    
    if (_statement)
        cee_source_fregment_free_full(_statement);
    
    if (_tokens_ref)
        cee_list_free(_tokens_ref);
    
    if (_source_token_map)
        cee_source_token_map_free(_source_token_map);
    
    if (_symbol_wrappers)
        cee_list_free_full(_symbol_wrappers, symbol_wrapper_free);
    
    if (_prep_directive_symbol_tree)
        cee_tree_free(_prep_directive_symbol_tree);
    
    if (_statement_symbol_tree)
        cee_tree_free(_statement_symbol_tree);
    
    if (_storage)
        cee_text_storage_free(_storage);
}

- (void)setState:(CEESourceBufferState)state {
    _state |= state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferStateChanged object:self];
}

- (CEESourceBufferState)state {
    return _state;
}

- (void)clearState:(CEESourceBufferState)state {
    _state &= ~state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferStateChanged object:self];
}

- (BOOL)stateSet:(CEESourceBufferState)state {
    return _state & state;
}

- (void)reload {
    cee_uchar* content = NULL;
    cee_file_contents_get([_filePath UTF8String], &content, NULL);
    cee_text_storage_buffer_set(_storage, content);
    cee_source_buffer_parse(self, kCEESourceBufferParserOptionCreateSymbolWrapper);
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

- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filePath {
    NSString* filePathBeforeSave = buffer.filePath;
    
    [[NSString stringWithUTF8String:(const char *)cee_text_storage_buffer_get(buffer.storage)] writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    [buffer updateFileModifiedDate];
    
    if (![filePathBeforeSave isEqualToString:filePath]) {
        buffer.filePath = filePath;
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

- (CEESourceBuffer*)getSourceBufferWithFilePath:(NSString*)filePath {
    for (CEESourceBuffer* buffer in _buffers)
        if ([buffer.filePath compare:filePath options:NSLiteralSearch] == NSOrderedSame)
            return buffer;
    return nil;
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

- (BOOL)hasModifiedBuffer {
    for (CEESourceBuffer* buffer in _buffers) {
        if (buffer.state & kCEESourceBufferStateModified)
            return YES;
    }
    return NO;
}

@end
