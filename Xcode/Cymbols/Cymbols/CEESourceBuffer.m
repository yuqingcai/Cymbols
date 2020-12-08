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

@interface CEESourceBuffer() {
    cee_uchar _byteOrderMarks[4];
}
@property (strong) NSDate* fileLastModifiedDate;
@property (strong) NSDate* lastModifiedDate;
- (instancetype)initWithFilePath:(nullable NSString*)filePath;
- (void)saveAtFilePath:(NSString*)filePath;
@end

@implementation CEESourceBuffer

@synthesize state = _state;

// Don't post any notification in this functon, this function can be called in
// a async background queue, notification may effect an UI operation which should 
// be in main_queue
void cee_source_buffer_parse(CEESourceBuffer* buffer,
                             CEESourceBufferParserOption options)
{
    if (!buffer.parser_ref) {
        buffer.parser_ref = cee_source_parser_get([buffer.filePath UTF8String]);
        if (!buffer.parser_ref)
            return;
    }
    
    //cee_ulong m0 = 0;
    const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
    CEESourceFregment* statement = NULL;
    CEESourceFregment* prep_directive = NULL;
    CEESourceFregment* comment = NULL;
    CEEList* tokens = NULL;
    CEESourceTokenMap* source_token_map = NULL;
    
    //m0 = cee_timestamp_ms();
    // parse buffer begin
    if (buffer.comment)
        cee_source_fregment_free_full(buffer.comment);
    
    if (buffer.prep_directive)
        cee_source_fregment_free_full(buffer.prep_directive);
    
    if (buffer.statement)
        cee_source_fregment_free_full(buffer.statement);
        
    if (buffer.tokens)
        cee_list_free_full(buffer.tokens, cee_token_free);
    
    if (buffer.source_token_map)
        cee_source_token_map_free(buffer.source_token_map);
    
    if (buffer.prep_directive_symbol_tree)
        cee_tree_free(buffer.prep_directive_symbol_tree);
    
    if (buffer.statement_symbol_tree)
        cee_tree_free(buffer.statement_symbol_tree);
    
    cee_source_symbol_parse(buffer.parser_ref,
                            [buffer.filePath UTF8String],
                            (const cee_char*)subject,
                            &prep_directive,
                            &statement,
                            &comment,
                            &tokens,
                            &source_token_map);
    
    buffer.comment = comment;
    buffer.prep_directive = prep_directive;
    buffer.statement = statement;
    buffer.tokens = tokens;
    buffer.source_token_map = source_token_map;
    buffer.prep_directive_symbol_tree = 
        cee_source_fregment_symbol_tree_create(buffer.prep_directive);
    buffer.statement_symbol_tree = 
        cee_source_fregment_symbol_tree_create(buffer.statement);
    // parse buffer end
    //cee_ulong m1 = cee_timestamp_ms();
    //fprintf(stdout, "\nbuffer_parse cost: %lu ms\n", m1 - m0);
    
    if (options & kCEESourceBufferParserOptionCreateSymbolWrapper) {
        // symbol wrappers create begin
        if (buffer.symbol_wrappers)
            cee_list_free_full(buffer.symbol_wrappers, 
                               cee_source_symbol_wrapper_free);
        
        CEEList* wrappers = NULL;
        cee_source_symbol_tree_dump_to_wrappers(buffer.prep_directive_symbol_tree, 
                                                &wrappers);
        cee_source_symbol_tree_dump_to_wrappers(buffer.statement_symbol_tree, 
                                                &wrappers);
        wrappers = cee_list_sort(wrappers, 
                                 cee_source_symbol_wrapper_location_compare);
        buffer.symbol_wrappers = wrappers;
        // symbol wrappers create end
    }
    //NSLog(@"+ source_fregment_count: %d", cee_source_fregment_count_get());
    //NSLog(@"+ source_symbol_count: %d", cee_source_symbol_count_get());
    //NSLog(@"+ cee_tree_count_get: %d", cee_tree_count_get());
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
    
    memset(_byteOrderMarks, 0, 4);
    
    _referenceCount = 0;
    
    if (!filePath) {
        _filePath = nil;
        _encodeType = kCEEBufferEncodeTypeUTF8;
        _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                           (const cee_uchar*)"",
                                           text_buffer_modified);
        _state = kCEESourceBufferStateFileTemporary;
    }
    else {
        _filePath = filePath;
        
        cee_uchar* content = NULL;
        cee_ulong length = 0;
        cee_uchar* converted_bytes = NULL;
        
        cee_file_contents_get([_filePath UTF8String], &content, &length);
        
        if (!content)
            return nil;
        
        if (cee_codec_has_bom(content)) {
            cee_codec_export_bom(content, _byteOrderMarks);
            _encodeType = cee_codec_type_from_bom(_byteOrderMarks);
            converted_bytes = cee_codec_convert_to_utf8(content,
                                                        length,
                                                        _encodeType);
            if (converted_bytes) {
                _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                                   converted_bytes,
                                                   text_buffer_modified);
                cee_free(converted_bytes);
            }
        }
        else {
            if (cee_codec_encoding_utf8(content, length)) {
                _encodeType = kCEEBufferEncodeTypeUTF8;
                _storage = cee_text_storage_create((__bridge cee_pointer)self,
                                                   content,
                                                   text_buffer_modified);
            }
            else {
                _encodeType = kCEEBufferEncodeTypeUnknow;
                _storage = cee_binary_storage_create((__bridge cee_pointer)self,
                                                     content,
                                                     length,
                                                     binary_buffer_modified);
            }
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
    
    if (_tokens)
        cee_list_free_full(_tokens, cee_token_free);
    
    if (_source_token_map)
        cee_source_token_map_free(_source_token_map);
    
    if (_symbol_wrappers)
        cee_list_free_full(_symbol_wrappers, cee_source_symbol_wrapper_free);
    
    if (_prep_directive_symbol_tree)
        cee_tree_free(_prep_directive_symbol_tree);
    
    if (_statement_symbol_tree)
        cee_tree_free(_statement_symbol_tree);
    
    if (_storage) {
        if (_encodeType == kCEEBufferEncodeTypeUTF8)
            cee_text_storage_free(_storage);
        else if (_encodeType == kCEEBufferEncodeTypeUnknow)
            cee_binary_storage_free(_storage);
        else
            assert(0);
    }
    
    //NSLog(@"- cee_tree_count_get: %d", cee_tree_count_get());
    //NSLog(@"- source_symbol_count: %d", cee_source_symbol_count_get());
    //NSLog(@"- source_fregment_count: %d", cee_source_fregment_count_get());
}

- (BOOL)withBOM {
    return (_byteOrderMarks[0] != 0 ||
            _byteOrderMarks[1] != 0 ||
            _byteOrderMarks[2] != 0 ||
            _byteOrderMarks[3] != 0);
}

- (void)setState:(CEESourceBufferState)state {
    if (state == kCEESourceBufferStateModified) {
        cee_source_buffer_parse(self, kCEESourceBufferParserOptionCreateSymbolWrapper);
        _lastModifiedDate = [NSDate date];
        if (self.referenceCount == 1)
            [self setState:kCEESourceBufferStateShouldSyncWhenClose];
    }
    _state |= state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferChangeState object:self];
}

- (void)clearState:(CEESourceBufferState)state {
    if (state == kCEESourceBufferStateModified) {
        if ([self stateSet:kCEESourceBufferStateShouldSyncWhenClose])
            [self clearState:kCEESourceBufferStateShouldSyncWhenClose];
    }
    _state &= ~state;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferChangeState object:self];
}

- (CEESourceBufferState)state {
    return _state;
}

- (BOOL)stateSet:(CEESourceBufferState)state {
    return (_state & state) != 0;
}

- (void)reload {
    cee_uchar* content = NULL;
    cee_ulong length = 0;
    cee_uchar* converted_bytes = NULL;
    
    memset(_byteOrderMarks, 0, 4);
    
    cee_file_contents_get([_filePath UTF8String], &content, &length);
    if (!content)
        return;
    
    if (cee_codec_has_bom(content)) {
        cee_codec_export_bom(content, _byteOrderMarks);
        _encodeType = cee_codec_type_from_bom(_byteOrderMarks);
        converted_bytes = cee_codec_convert_to_utf8(content,
                                                    length,
                                                    _encodeType);
        if (converted_bytes) {
            cee_text_storage_buffer_set(_storage, converted_bytes);
            cee_free(converted_bytes);
        }
    }
    else {
        if (cee_codec_encoding_utf8(content, length)) {
            _encodeType = kCEEBufferEncodeTypeUTF8;
            cee_text_storage_buffer_set(_storage, content);
        }
        else {
            _encodeType = kCEEBufferEncodeTypeUnknow;
            cee_text_storage_buffer_set(_storage, content);
        }
    }
    cee_free(content);
    
    /**
     * source file may be updated from another application, when reload
     * the source buffer, parse it.
     */
    cee_source_buffer_parse(self, kCEESourceBufferParserOptionCreateSymbolWrapper);
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSourceBufferReload object:self];
    
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
    _fileLastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:_filePath error:nil] fileModificationDate];
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
    if (!_referenceCount)
        return;
    
    _referenceCount --;
    if (_referenceCount == 1) {
        if ([self stateSet:kCEESourceBufferStateModified])
            [self setState:kCEESourceBufferStateShouldSyncWhenClose];
    }
}

- (void)saveAtFilePath:(NSString*)filePath {
    const cee_uchar* subject = cee_text_storage_buffer_get(_storage);
    cee_size length = cee_text_storage_size_get(_storage);
    NSMutableData* data = NULL;
    
    if (!subject)
        return;
    
    if (_encodeType != kCEEBufferEncodeTypeUTF8) {
        cee_uchar* convertedBytes = NULL;
        cee_size convertedLength = 0;
        cee_codec_convert_from_utf8(subject,
                                    length,
                                    _encodeType,
                                    &convertedBytes,
                                    &convertedLength);
        if (convertedBytes) {
            data = [[NSMutableData alloc] initWithBytes:convertedBytes length:convertedLength];
            cee_free(convertedBytes);
        }
    }
    else {
        data = [[NSMutableData alloc] initWithBytes:subject length:length];
    }
    
    if (!data)
        return;
    
    length = strlen((const cee_char*)_byteOrderMarks);
    if (length) {
        NSMutableData* orderMarks = [NSMutableData dataWithBytes:_byteOrderMarks length:length];
        [orderMarks appendData:data];
        data = orderMarks;
    }
    
    [data writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO];
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

- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString*)filePath andOption:(CEESourceBufferOpenOption)option {
    if (!filePath || ![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return nil;
    
    CEESourceBuffer* target = nil;
    
    if (option & kCEESourceBufferOpenOptionShare) {
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
    }
    else {
        target = [[CEESourceBuffer alloc] initWithFilePath:filePath];
        if (!target)
            return nil;
    }
    
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
    
    BOOL shouldParsed = NO;
    /** when a source buffer filename extension is changed,
     *  that means another parser is selected, we need to
     *  parse it again.
     */
    if (![buffer.filePath isEqualToString:filePath])
        shouldParsed = YES;
    
    [buffer updateFileModifiedDate];
    
    if ([buffer stateSet:kCEESourceBufferStateFileTemporary]) {
        [buffer clearState:kCEESourceBufferStateFileTemporary];
        buffer.filePath = filePath;
    }
    
    if ([buffer stateSet:kCEESourceBufferStateModified])
        [buffer clearState:kCEESourceBufferStateModified];
    
    if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
        [buffer clearState:kCEESourceBufferStateFileDeleted];
    
    if (shouldParsed)
        cee_source_buffer_parse(buffer, kCEESourceBufferParserOptionCreateSymbolWrapper);
        
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
    NSString* fileName = [buffer.filePath lastPathComponent];
    NSString* extension = [buffer.filePath pathExtension];
    
    if (buffer.encodeType == kCEEBufferEncodeTypeUnknow)
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
