//
//  CEESourceBuffer.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/19.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESourceBuffer.h"


NSNotificationName CEENotificationSourceBufferChangeState = @"CEENotificationSourceBufferChangeState";
NSNotificationName CEENotificationSourceBufferReload = @"CEENotificationSourceBufferReload";

@interface CEESourceBuffer()
@property CEEList* symbol_wrappers;
@property (strong) NSDate* fileLastModifiedDate;
@property (strong) NSDate* lastModifiedDate;
@property (readonly) NSInteger referenceCount;
@property CEEList* symbol_caches;

- (instancetype)initWithFilePath:(nullable NSString*)filePath;
- (void)saveAtFilePath:(NSString*)filePath;
@end

@implementation CEESourceBuffer

@synthesize state = _state;

- (instancetype)initWithFilePath:(nullable NSString*)filePath {
    self = [super init];
    if (!self)
        return nil;
    
    _referenceCount = 0;
    
    if (!filePath) {
        _filePath = nil;
        _type = kCEESourceBufferTypeText;
        _storage = cee_text_storage_create((const cee_uchar*)"");
        _state = kCEESourceBufferStateFileTemporary;
    }
    else {
        _filePath = filePath;
        [self loadContent];
        _state = kCEESourceBufferStateNormal;
    }
    
    _fileLastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
    return self;
}

- (void)reload {
    [self loadContent];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferReload object:self];
    if ([self stateSet:kCEESourceBufferStateShouldSyncToFile])
        [self clearState:kCEESourceBufferStateShouldSyncToFile];
}

- (void)loadContent {
    [self cleanParsedContext];
    
    if (_storage) {
        if (_type == kCEESourceBufferTypeBinary)
            cee_binary_storage_free(_storage);
        else if (_type == kCEESourceBufferTypeText)
            cee_text_storage_free(_storage);
        _storage = NULL;
    }
    
    cee_uchar* content = NULL;
    cee_ulong length = 0;
    cee_file_contents_get([_filePath UTF8String], &content, &length);
    if (!content)
        return;
    
    if (cee_codec_has_bom(content)) {
        _type = kCEESourceBufferTypeText;
        cee_uchar* bom = NULL;
        cee_ulong bom_size = 0;
        cee_codec_bom_export(content, &bom, &bom_size);
        cee_uchar* raw_utf8 =
            cee_codec_convert_to_utf8_with_bom(content,
                                               length,
                                               bom,
                                               bom_size);
        cee_free(content);
        content = raw_utf8;
        length = strlen((cee_char*)content);
        
        _storage = cee_text_storage_create(content);
        cee_text_storage_bom_set(_storage, bom, bom_size);
        if (bom)
            cee_free(bom);
    }
    else if (cee_codec_is_binary(content, length)) {
        _type = kCEESourceBufferTypeBinary;
        _storage = cee_binary_storage_create(content, length);
    }
    else {
        _type = kCEESourceBufferTypeText;
        _storage = cee_text_storage_create(content);
    }
    
    cee_free(content);
    
    [self parse];
}

- (void)cleanParsedContext {
    if (_comment_fregment)
        cee_source_fregment_free_full(_comment_fregment);
    _comment_fregment = NULL;
    
    if (_prep_directive_fregment)
        cee_source_fregment_free_full(_prep_directive_fregment);
    _prep_directive_fregment = NULL;
    
    if (_statement_fregment)
        cee_source_fregment_free_full(_statement_fregment);
    _statement_fregment = NULL;
        
    if (_tokens)
        cee_list_free_full(_tokens, cee_token_free);
    _tokens = NULL;
    
    if (_source_token_map)
        cee_source_token_map_free(_source_token_map);
    _source_token_map = NULL;
    
    if (_prep_directive_symbol_tree)
        cee_tree_free(_prep_directive_symbol_tree);
    _prep_directive_symbol_tree = NULL;
    
    if (_statement_symbol_tree)
        cee_tree_free(_statement_symbol_tree);
    _statement_symbol_tree = NULL;
    
    if (_symbol_wrappers)
        cee_list_free_full(_symbol_wrappers, cee_source_symbol_wrapper_free);
    _symbol_wrappers = NULL;
}

- (void)dealloc {
    [self cleanParsedContext];
    [self cleanSymbolCache];
    if (_storage) {
        if (_type == kCEESourceBufferTypeBinary)
            cee_binary_storage_free(_storage);
        else if (_type == kCEESourceBufferTypeText)
            cee_text_storage_free(_storage);
    }
}

- (void)setState:(CEESourceBufferState)state {
    if (state == kCEESourceBufferStateShouldSyncToFile) {
        _lastModifiedDate = [NSDate date];
        [self parse];
    }
    _state |= state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferChangeState object:self];
}

- (void)clearState:(CEESourceBufferState)state {
    _state &= ~state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferChangeState object:self];
}

- (CEESourceBufferState)state {
    return _state;
}

- (BOOL)stateSet:(CEESourceBufferState)state {
    return (_state & state) != 0;
}

- (BOOL)isFileModified {
    NSDate* date1 = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
    if ([_fileLastModifiedDate compare:date1] != NSOrderedSame)
        return YES;
    return NO;
}

- (void)updateFileModifiedDate {
    _fileLastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
}

- (void)increaseReferenceCount {
    _referenceCount ++;
}

- (void)decreaseReferenceCount {
    if (!_referenceCount)
        return;
    
    _referenceCount --;
}

- (void)saveAtFilePath:(NSString*)filePath {
    const cee_uchar* content = NULL;
    cee_size size = 0;
    NSMutableData* data = NULL;
    const cee_uchar* bom = NULL;
    cee_ulong bom_size = 0;
    
    if (_type == kCEESourceBufferTypeBinary) {
        content = cee_binary_storage_buffer_get(_storage);
        size = cee_binary_storage_size_get(_storage);
        data = [[NSMutableData alloc] initWithBytes:content length:size];
    }
    else if (_type == kCEESourceBufferTypeText) {
        cee_text_storage_bom_get(_storage, &bom, &bom_size);
        content = cee_text_storage_buffer_get(_storage);
        size = cee_text_storage_size_get(_storage);
        
        if (bom) {
            cee_uchar* converted_bytes = NULL;
            cee_size converted_length = 0;
            const cee_char* encode_type =
                cee_codec_encoded_type_from_bom(bom, bom_size);
            cee_codec_convert_from_utf8(content,
                                        size,
                                        encode_type,
                                        &converted_bytes,
                                        &converted_length);
            if (converted_bytes) {
                data = [[NSMutableData alloc] initWithBytes:converted_bytes length:converted_length];
                cee_free(converted_bytes);
            }
            
            NSMutableData* orderMarks = [NSMutableData dataWithBytes:bom length:bom_size];
            [orderMarks appendData:data];
            data = orderMarks;
        }
        else {
            data = [[NSMutableData alloc] initWithBytes:content length:size];
        }
    }
    
    if (!data)
        return;
    
    [data writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO];
}

// Don't post any notification in this functon, this function can be called in
// a async background queue, notification may effect an UI operation which should
// be in main_queue
- (void)parse {
    //cee_ulong m0 = cee_timestamp_ms();
    
    if (_type != kCEESourceBufferTypeText)
        return;
        
    if (!_parser_ref) {
        _parser_ref = cee_source_parser_get([_filePath UTF8String]);
        
        if (!_parser_ref)
            return;
    }
    
    [self cleanParsedContext];
    [self cleanSymbolCache];
    
    const cee_uchar* subject = cee_text_storage_buffer_get(_storage);
    cee_source_symbol_parse(_parser_ref,
                            [_filePath UTF8String],
                            (const cee_char*)subject,
                            &_prep_directive_fregment,
                            &_statement_fregment,
                            &_comment_fregment,
                            &_tokens,
                            &_source_token_map);
        
    _prep_directive_symbol_tree = cee_source_fregment_symbol_tree_create(_prep_directive_fregment);
    _statement_symbol_tree = cee_source_fregment_symbol_tree_create(_statement_fregment);
    
    //cee_ulong m1 = cee_timestamp_ms();
    //fprintf(stdout, "parse cost: %ld ms\n", m1 - m0);
}

- (void)cleanSymbolWrappers {
    if (_symbol_wrappers)
        cee_list_free_full(_symbol_wrappers, cee_source_symbol_wrapper_free);
    _symbol_wrappers = NULL;
}

- (void)cleanSymbolCache {
    if (_symbol_caches)
        cee_list_free_full(_symbol_caches, cee_source_symbol_cache_free);
    _symbol_caches = NULL;
}

- (void)generateSymbolWrappers {
    
    //cee_ulong m0 = cee_timestamp_ms();
    
    if (_prep_directive_symbol_tree)
        cee_source_symbol_tree_dump_to_wrappers(_prep_directive_symbol_tree, &_symbol_wrappers);
    
    if (_statement_symbol_tree)
        cee_source_symbol_tree_dump_to_wrappers(_statement_symbol_tree, &_symbol_wrappers);
    
    _symbol_wrappers = cee_list_sort(_symbol_wrappers, cee_source_symbol_wrapper_location_compare);
    
    //cee_ulong m1 = cee_timestamp_ms();
    //fprintf(stdout, "generateSymbolWrappers cost: %ld ms\n", m1 - m0);
}

- (CEEList*)symbols {
    if (!_symbol_wrappers)
        [self generateSymbolWrappers];
    return _symbol_wrappers;
}

- (CEEList*)referencesInRange:(CEERange)range {
    //cee_ulong m0 = cee_timestamp_ms();
    CEEList* references = NULL;
    const cee_uchar* subject = cee_text_storage_buffer_get(self.storage);
    cee_source_reference_parse(_parser_ref,
                               [_filePath UTF8String],
                               (const cee_char*)subject,
                               _source_token_map,
                               _prep_directive_fregment,
                               _statement_fregment,
                               range,
                               &references);
    //cee_ulong m1 = cee_timestamp_ms();
    //fprintf(stdout, "references in range cost: %lu ms\n", m1 - m0);
    return references;
}

- (CEEList*)tagsInRange:(CEERange)range withDataBase:(cee_pointer)database {
    //cee_ulong m0 = cee_timestamp_ms();
    CEEList* references = [self referencesInRange:range];
    CEEList* tags = NULL;
    
    tags = cee_source_tags_create(_parser_ref,
                                  _source_token_map,
                                  _prep_directive_fregment,
                                  _statement_fregment,
                                  database,
                                  range,
                                  references,
                                  &_symbol_caches);
    
    if (references)
        cee_list_free_full(references, cee_source_symbol_reference_free);
    
    //cee_ulong m1 = cee_timestamp_ms();
    //fprintf(stdout, "tags in range cost: %lu ms\n", m1 - m0);
    
    return tags;
}

@end

@interface CEESourceBufferManager()
@property NSInteger temporaryIndex;
@end

@implementation CEESourceBufferManager

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    _temporaryIndex = 0;
    _buffers = [[NSMutableArray alloc] init];
    [self createTemporyDirectory];
    return self;
}

- (void)createTemporyDirectory {
    NSArray* searchPaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* supportDirectory = [searchPaths firstObject];
    _temporaryDirectory = [supportDirectory stringByAppendingPathComponent:@"Backups/Untitled"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:_temporaryDirectory isDirectory:nil])
        [[NSFileManager defaultManager] createDirectoryAtPath:_temporaryDirectory withIntermediateDirectories:YES attributes:nil error:nil];
}

- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString*)filePath  {
    BOOL isDirectory = NO;
    
    if (!filePath ||
        ![[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory] ||
        isDirectory)
        return nil;
    
    CEESourceBuffer* target = nil;
    
    for (CEESourceBuffer* buffer in _buffers) {
        if ([buffer.filePath isEqualToString:filePath])
            target = buffer;
    }
    
    if (!target) {
        target = [[CEESourceBuffer alloc] initWithFilePath:filePath];
        if (!target)
            return nil;
        [_buffers addObject:target];
    }
    
    [target increaseReferenceCount];
    
    return target;
}

- (CEESourceBuffer*)openUntitledSourceBuffer {
    NSString* fileName = [NSString stringWithFormat:@"Untitled%ld", (long)_temporaryIndex];
    NSString* filePath = [_temporaryDirectory stringByAppendingPathComponent:fileName];
    CEESourceBuffer* buffer = [[CEESourceBuffer alloc] initWithFilePath:nil];
    
    [buffer setFilePath:filePath];
    [buffer setState:kCEESourceBufferStateFileTemporary];
    
    [[NSString stringWithUTF8String:(const char *)cee_text_storage_buffer_get(buffer.storage)] writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    
    [_buffers addObject:buffer];
    _temporaryIndex ++;
    
    [buffer increaseReferenceCount];
    return buffer;
}

- (void)closeSourceBuffer:(CEESourceBuffer*)buffer {
    [buffer decreaseReferenceCount];
    if (!buffer.referenceCount) {
        if ([_buffers containsObject:buffer])
            [_buffers removeObject:buffer];
    }
}

- (BOOL)saveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath {
    [buffer saveAtFilePath:filePath];
    [buffer updateFileModifiedDate];
    
    if ([buffer stateSet:kCEESourceBufferStateFileTemporary]) {
        [buffer clearState:kCEESourceBufferStateFileTemporary];
        buffer.filePath = filePath;
    }
    
    if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile])
        [buffer clearState:kCEESourceBufferStateShouldSyncToFile];
    
    if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
        [buffer clearState:kCEESourceBufferStateFileDeleted];
            
    return YES;
}

- (void)discardUntitleSourceBuffers {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtPath:_temporaryDirectory];
    NSString *fileName;
    while (fileName = [enumerator nextObject])
        [fileManager removeItemAtPath:[_temporaryDirectory stringByAppendingPathComponent:fileName] error:nil];
}

- (void)syncSourceBuffersFromFiles {
    for (CEESourceBuffer* buffer in _buffers) {
        if (![buffer stateSet:kCEESourceBufferStateFileTemporary]) {
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

- (BOOL)isTemporaryFilePath:(NSString*)filePath {
    NSString* directory = [filePath stringByDeletingLastPathComponent];
    if ([directory isEqualToString:_temporaryDirectory])
        return YES;
    return NO;
}

- (NSString*)sourceType:(CEESourceBuffer*)buffer {
    if (!buffer)
        return @"Binary File";
    
    NSString* fileName = [buffer.filePath lastPathComponent];
    NSString* extension = [buffer.filePath pathExtension];
    
    if (buffer.type == kCEESourceBufferTypeBinary)
        return @"Binary File";
    
    if (![extension isEqualToString:@""]) {
        if ([extension compare:@"c" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"C";
        
        if ([extension compare:@"CC" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
            [extension compare:@"C++" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
            [extension compare:@"CPP" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"C++";
        
        if ([extension compare:@"m" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"Objective C";
        
        if ([extension compare:@"mm" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"Objective C++";
        
        if ([extension compare:@"h" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
            [extension compare:@"hh" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
            [extension compare:@"hpp" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
            [extension compare:@"hxx" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"C Source Header";
        
        if ([extension compare:@"Java" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"Java";
        
        if ([extension compare:@"swift" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"Swift";
        
        if ([extension compare:@"js" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"JavaScript";
        
        if ([extension compare:@"HTML" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"HTML";
        
        if ([extension compare:@"asm" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
            [extension compare:@"s" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"Assembly";
        
        if ([extension compare:@"cs" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"C Sharp";
        
    }
    else {
        if ([fileName compare:@"Makefile" options:NSCaseInsensitiveSearch] == NSOrderedSame)
            return @"Makefile";
    }
    return @"Plain Text";
}

@end
