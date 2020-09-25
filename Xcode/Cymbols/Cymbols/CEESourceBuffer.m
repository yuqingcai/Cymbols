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
@property (strong) NSDate* fileLastModifiedDate;
@property (strong) NSDate* lastModifiedDate;
@end

@implementation CEESourceBuffer

@synthesize state = _state;

void cee_source_buffer_parse(CEESourceBuffer* buffer,
                             CEESourceBufferParserOption options)
{
    CEESourceParserRef parser = buffer.parser_ref;
    if (!parser) {
        buffer.parser_ref = cee_source_parser_get([buffer.filePath UTF8String]);
        if (!parser)
            return;
    }
    
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
            cee_list_free_full(buffer.symbol_wrappers, cee_source_symbol_wrapper_free);
        
        CEEList* wrappers = NULL;
        cee_source_symbol_tree_dump_to_wrappers(buffer.prep_directive_symbol_tree, &wrappers);
        cee_source_symbol_tree_dump_to_wrappers(buffer.statement_symbol_tree, &wrappers);
        wrappers = cee_list_sort(wrappers, cee_source_symbol_wrapper_location_compare);
        buffer.symbol_wrappers = wrappers;
        /** symbol wrappers create end */
    }
}

static void text_buffer_modified(cee_pointer buffer, 
                                 CEETextStorageRef storage,
                                 CEERange original,
                                 CEERange replacement) 
{
    [(__bridge CEESourceBuffer*)buffer setState:kCEESourceBufferStateModified];
}

static void binary_buffer_modified(cee_pointer buffer, 
                                   CEEBinaryStorageRef storage,
                                   CEERange original,
                                   CEERange replacement)
{
    [(__bridge CEESourceBuffer*)buffer setState:kCEESourceBufferStateModified];
}

- (NSString*)UTCStringFromDate:(NSDate*)date {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    NSTimeZone *timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    [formatter setTimeZone:timeZone];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss.SSS"];
    NSString *string = [formatter stringFromDate:date];
    return string;
}

- (NSDate*)dateFromUTFString:(NSString*)string {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    NSTimeZone *timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    [formatter setTimeZone:timeZone];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss.SSS"];
    NSDate* date = [formatter dateFromString:string];
    return date;
}

- (instancetype)initWithFilePath:(nullable NSString*)filePath {
    self = [super init];
    if (!self)
        return nil;

    _referenceCount = 0;
    cee_ulong length = 0;
    
    if (!filePath) {
        _filePath = nil;
        _type = kCEEBufferTypeUTF8;
        _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                           (const cee_uchar*)"",
                                           text_buffer_modified);
        _state = kCEESourceBufferStateFileTemporary;
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
    
    _fileLastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
    return self;
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
        cee_list_free_full(_symbol_wrappers, cee_source_symbol_wrapper_free);
    
    if (_prep_directive_symbol_tree)
        cee_tree_free(_prep_directive_symbol_tree);
    
    if (_statement_symbol_tree)
        cee_tree_free(_statement_symbol_tree);
    
    if (_storage)
        cee_text_storage_free(_storage);
}

- (void)setState:(CEESourceBufferState)state {
    cee_source_buffer_parse(self, kCEESourceBufferParserOptionCreateSymbolWrapper);   
    if (state == kCEESourceBufferStateModified) {
        _lastModifiedDate = [NSDate date];
        if (self.referenceCount == 1)
            [self setState:kCEESourceBufferStateShouldSyncWhenClose];
    }
    _state |= state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferStateChanged object:self];
}

- (void)clearState:(CEESourceBufferState)state {
    cee_source_buffer_parse(self, kCEESourceBufferParserOptionCreateSymbolWrapper);
    _state &= ~state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferStateChanged object:self];
    
    if (state == kCEESourceBufferStateModified) {
        if ([self stateSet:kCEESourceBufferStateShouldSyncWhenClose])
            [self clearState:kCEESourceBufferStateShouldSyncWhenClose];
    }
}

- (CEESourceBufferState)state {
    return _state;
}

- (BOOL)stateSet:(CEESourceBufferState)state {
    return (_state & state) != 0;
}

- (void)reload {
    cee_uchar* content = NULL;
    cee_file_contents_get([_filePath UTF8String], &content, NULL);
    cee_text_storage_buffer_set(_storage, content);
    cee_free(content);
    [self setState:kCEESourceBufferStateReload];
    [self clearState:kCEESourceBufferStateReload];
    
    if ([self stateSet:kCEESourceBufferStateModified])
        [self clearState:kCEESourceBufferStateModified];
}

- (BOOL)isFileModified {
    NSDate* date1 = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
    if ([_fileLastModifiedDate compare:date1] != NSOrderedSame)
        return YES;
    return NO;
}

- (void)updateFileModifiedDate {
    _fileLastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];;
}

- (void)increaseReferenceCount {
    _referenceCount ++;
    if (_referenceCount == 1) {
        if ([self stateSet:kCEESourceBufferStateModified])
            [self setState:kCEESourceBufferStateShouldSyncWhenClose];
    }
    else if (_referenceCount > 1) {
        if ([self stateSet:kCEESourceBufferStateShouldSyncWhenClose])
            [self clearState:kCEESourceBufferStateShouldSyncWhenClose];
    }
}

- (void)decreaseReferenceCount {
    _referenceCount --;
    if (_referenceCount == 1) {
        if ([self stateSet:kCEESourceBufferStateModified])
            [self setState:kCEESourceBufferStateShouldSyncWhenClose];
    }
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
    _temporaryDirectory = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/Cymbols/Backups/Untitled"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:_temporaryDirectory isDirectory:nil])
        [[NSFileManager defaultManager] createDirectoryAtPath:_temporaryDirectory withIntermediateDirectories:YES attributes:nil error:nil];
    return self;
}

- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString*)filePath {
    if (!filePath || ![[NSFileManager defaultManager] fileExistsAtPath:filePath])
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
    if (!buffer.referenceCount)
        [_buffers removeObject:buffer];
}

- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filePath {
    [[NSString stringWithUTF8String:(const char *)cee_text_storage_buffer_get(buffer.storage)] writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    
    [buffer updateFileModifiedDate];
        
    if ([buffer stateSet:kCEESourceBufferStateFileTemporary]) {
        [buffer clearState:kCEESourceBufferStateFileTemporary];
        buffer.filePath = filePath;
    }
    
    if ([buffer stateSet:kCEESourceBufferStateModified])
        [buffer clearState:kCEESourceBufferStateModified];
    
    if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
        [buffer clearState:kCEESourceBufferStateFileDeleted];
}

- (void)discardUntitleSourceBuffers {
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtPath:_temporaryDirectory];
    NSString *file;
    while (file = [enumerator nextObject])
        [fileManager removeItemAtPath:[_temporaryDirectory stringByAppendingPathComponent:file] error:nil];
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

@end
