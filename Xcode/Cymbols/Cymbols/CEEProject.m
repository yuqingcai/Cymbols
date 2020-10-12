//
//  CEEProject.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProject.h"
#import "CEESessionWindowController.h"
#import "CEESessionViewController.h"
#import "CEEIdentifier.h"
#import "cee_symbol.h"
#import "cee_backend.h"
#import "cee_source_parser.h"
#include <unistd.h>

extern char* init_descriptor_template;
extern char* open_file_descriptor_template;

NSNotificationName CEENotificationProjectSettingProperties = @"CEENotificationProjectSettingProperties";
NSNotificationName CEENotificationProjectAddFilePaths = @"CEENotificationProjectAddFilePaths";
NSNotificationName CEENotificationProjectRemoveFilePaths = @"CEENotificationProjectRemoveFilePaths";
NSNotificationName CEENotificationSessionPortOpenSourceBuffer = @"CEENotificationSessionPortOpenSourceBuffer";
NSNotificationName CEENotificationSessionPortCloseSourceBuffer = @"CEENotificationSessionPortCloseSourceBuffer";
NSNotificationName CEENotificationSessionPortActiveSourceBuffer = @"CEENotificationSessionPortActiveSourceBuffer";
NSNotificationName CEENotificationSessionPresent = @"CEENotificationSessionPresent";
NSNotificationName CEENotificationSessionCreatePort = @"CEENotificationSessionCreatePort";
NSNotificationName CEENotificationSessionActivePort = @"CEENotificationSessionActivePort";
NSNotificationName CEENotificationSessionDeletePort = @"CEENotificationSessionDeletePort";
NSNotificationName CEENotificationSessionPortCreateContext = @"CEENotificationSessionPortCreateContext";
NSNotificationName CEENotificationSessionPortSetTargetSymbol = @"CEENotificationSessionPortSetTargetSymbol";
NSNotificationName CEENotificationSessionPortRequestTargetSymbolSelection = @"CEENotificationSessionPortRequestTargetSymbolSelection";
NSNotificationName CEENotificationSessionPortSetActivedSymbol = @"CEENotificationSessionPortSetActivedSymbol";
NSNotificationName CEENotificationSessionPortPresentHistory = @"CEENotificationSessionPortPresentHistory";
NSNotificationName CEENotificationSessionPortSaveSourceBuffer = @"CEENotificationSessionPortSaveSourceBuffer";
NSNotificationName CEENotificationSessionPortSetDescriptor = @"CEENotificationSessionPortSetDescriptor";
NSNotificationName CEENotificationSessionPortSearchReference = @"CEENotificationSessionPortSearchReference";


NSArray* ExclusiveFilePaths(NSArray* filePaths)
{
    if (!filePaths)
        return nil;
    
    CEEList* file_paths = NULL;
    for (NSString* filePath in filePaths)
        file_paths = cee_list_prepend(file_paths, (cee_pointer)[filePath UTF8String]);
    
    if (!file_paths)
        return nil;
    
    CEEList* exclusiveds = cee_paths_uniform(file_paths);
    cee_list_free(file_paths);
    
    if (!exclusiveds)
        return nil;
    
    NSMutableArray* _exclusiveds = [[NSMutableArray alloc] init];
    CEEList* p = exclusiveds;
    while (p) {
        [_exclusiveds addObject:[NSString stringWithUTF8String:p->data]];
        p = p->next;
    }
    cee_list_free_full(exclusiveds, cee_free);
    return _exclusiveds;
}

NSArray* ExpandFilePaths(NSArray* filePaths)
{    
    filePaths = ExclusiveFilePaths(filePaths);
    
    NSMutableArray* expand = [[NSMutableArray alloc] init];
    NSNumber *isDirectory;
    
    for (NSString* filePath in filePaths) {
        [[NSURL fileURLWithPath:filePath] getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:NULL];
        if (![isDirectory boolValue]) {
            [expand addObject:filePath];
        }
        else {
            NSDirectoryEnumerator *dirEnumerator = [[NSFileManager defaultManager] enumeratorAtURL:[NSURL fileURLWithPath:filePath] includingPropertiesForKeys:@[NSURLNameKey, NSURLIsDirectoryKey] options:NSDirectoryEnumerationSkipsHiddenFiles errorHandler:nil];
            for (NSURL *URL in dirEnumerator) {
                [URL getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:NULL];
                if (![isDirectory boolValue]) {
                    [expand addObject:[URL path]];
                }
            }
        }
    }
    
    if (!expand.count)
        expand = nil;
    
    return expand;
}

NSDictionary* JSONDictionaryFromString(NSString* string)
{
    NSData* jsonString = [string dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:NO];
    NSDictionary* dict = [NSJSONSerialization JSONObjectWithData:jsonString options:NSJSONReadingMutableContainers error:nil];
    return dict;
}

NSArray* CreateBookmarksWithFilePaths(NSArray* filePaths)
{
    if (!filePaths || !filePaths.count)
        return nil;
    
    NSMutableArray* bookmarks = [[NSMutableArray alloc] init];
    for (NSString* filePath in filePaths) {
        CEESecurityBookmark* bookmark = CreateBookmarkWithFilePath(filePath);
        if (bookmark)
            [bookmarks addObject:bookmark];
    }
    
    return bookmarks;
}

CEESecurityBookmark* CreateBookmarkWithFilePath(NSString* filePath)
{
    CEESecurityBookmark* bookmark = [[CEESecurityBookmark alloc] initWithFilePath:filePath];
    return bookmark;
}

@implementation CEESourceBufferReferenceContext

- (instancetype)initWithFilePath:(NSString*)filePath {
    self = [super init];
    if (!self)
        return nil;
    
    _filePath = filePath;
    _presentedLineBufferOffset = 0;
    _caretBufferOffset = 0;
    return self;
}

@end

@implementation CEEProjectSetting
+ (CEEProjectSetting*)projectSettingWithName:(NSString*)name path:(NSString*)path filePaths:(NSArray*)filePaths filePathsUserSelected:(NSArray*)filePathsUserSelected {
    CEEProjectSetting* setting = [[CEEProjectSetting alloc] init];
    if (setting) {
        setting.name = name;
        setting.path = path;
        setting.filePathsExpanded = filePaths;
        setting.filePathsUserSelected = filePathsUserSelected;
    }
    return setting;
}

- (NSString*)databasePath {
    NSString* path = [[_path stringByAppendingPathComponent:_name] stringByAppendingPathExtension:@"cymd"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:path])
        return path;
    return nil;
}

@end

@implementation CEESecurityBookmark

- (instancetype)initWithFilePath:(NSString*)filePath {
    self = [super init];
    
    if (!self)
        return nil;
    
    self.filePath = filePath;
    NSURL* fileURL = [NSURL fileURLWithPath:filePath];
    NSError* error = nil;
    NSData* bookmarkData = [fileURL bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope includingResourceValuesForKeys:nil relativeToURL:nil error:&error];
    
    if (!error)
        self.content = [bookmarkData base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
    
    return self;
}

@end

@interface CEESessionPort()
@property NSInteger bufferReferenceIndex;
@end

@implementation CEESessionPort

@synthesize descriptor = _descriptor;

- (instancetype)init {
    self = [super init];
    if (self) {
        _bufferReferences = [[NSMutableArray alloc] init];
        _bufferReferenceIndex = -1;
        _identifier = CreateObjectID(self);
        _openedSourceBuffers = [[NSMutableArray alloc] init];
    }
    return self;
}

- (CEESourceBuffer*)securityOpenSourceBufferWithFilePath:(NSString*)filePath {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = nil;
    NSArray* bookmarks = nil;
    
    if (access([filePath UTF8String], R_OK) != 0) {
        bookmarks = [self.session.project getSecurityBookmarksWithFilePaths:@[filePath]];
        if (bookmarks) {
            [self.session.project startAccessSecurityScopedResourcesWithBookmarks:bookmarks];
            buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath];
            [self.session.project stopAccessSecurityScopedResourcesWithBookmarks:bookmarks];
        }
        else {
            buffer = nil;
        }
    }
    else {
        buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath];
    }
    
    return buffer;
}

- (BOOL)securitySaveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    NSArray* bookmarks = nil;
    BOOL ret = NO;
    if (access([filePath UTF8String], W_OK) != 0) {
        bookmarks = [self.session.project getSecurityBookmarksWithFilePaths:@[filePath]];
        if (bookmarks) {
            [self.session.project startAccessSecurityScopedResourcesWithBookmarks:bookmarks];
            ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
            [self.session.project stopAccessSecurityScopedResourcesWithBookmarks:bookmarks];
        }
    }
    else {
        ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
    }
    return ret;
}

- (CEESourceBuffer*)bufferInContext:(CEESourceBufferReferenceContext*)context {
    for (CEESourceBuffer* buffer in _openedSourceBuffers) {
        if ([buffer.filePath isEqualToString:context.filePath])
            return buffer;
    }
    CEESourceBuffer* buffer = [self securityOpenSourceBufferWithFilePath:context.filePath];
    return buffer;
}

- (void)appendBufferReference:(CEESourceBuffer*)buffer {
    CEESourceBufferReferenceContext* current = [self currentBufferReference];
    if (current) {
        if ([current.filePath isEqualToString:buffer.filePath])
            return;
    }
    
    CEESourceBufferReferenceContext* refernce = [[CEESourceBufferReferenceContext alloc] initWithFilePath:buffer.filePath];
    if (!_bufferReferences.count) {
        [_bufferReferences addObject:refernce];
    }
    else {
        if (_bufferReferenceIndex < _bufferReferences.count - 1) {
            NSRange range = NSMakeRange(_bufferReferenceIndex + 1, _bufferReferences.count - (_bufferReferenceIndex + 1));
            [_bufferReferences removeObjectsInRange:range];
        }
        
        [_bufferReferences addObject:refernce];
    }
    
    _bufferReferenceIndex ++;
}

- (void)moveBufferReferenceNext {
    if (_bufferReferenceIndex < _bufferReferences.count - 1) {
        _bufferReferenceIndex ++;
        [self presentHistory:_bufferReferences[_bufferReferenceIndex]];
    }
}

- (void)moveBufferReferencePrev {
    if (_bufferReferenceIndex > 0) {
        _bufferReferenceIndex --;
        [self presentHistory:_bufferReferences[_bufferReferenceIndex]];
    }
}

- (CEESourceBufferReferenceContext*)currentBufferReference {
    if (_bufferReferenceIndex < 0 || _bufferReferenceIndex >= _bufferReferences.count)
        return nil;
    return _bufferReferences[_bufferReferenceIndex];
}

- (void)presentHistory:(CEESourceBufferReferenceContext*)reference {
    CEESourceBuffer* target = nil;
    
    for (NSInteger i = 0; i < _bufferReferences.count; i ++) {
        if (reference == _bufferReferences[i])
            _bufferReferenceIndex = i;
    }
    
    BOOL foundBuffer = NO;
    reference = [self currentBufferReference];
    for (CEESourceBuffer* buffer in _openedSourceBuffers) {
        if ([buffer.filePath isEqualToString:reference.filePath]) {
            foundBuffer = YES;
            break;
        }
    }
    
    if (!foundBuffer) {
        target = [self securityOpenSourceBufferWithFilePath:reference.filePath];
        if (target) {
            if (!target.symbol_wrappers)
                cee_source_buffer_parse(target, kCEESourceBufferParserOptionCreateSymbolWrapper);
            [_openedSourceBuffers addObject:target];
        }
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortPresentHistory object:self];
}

- (NSDictionary*)lastPresentedBufferOffsetInHistory:(NSString*)filePath {
    NSDictionary* bufferOffsets = nil;
    for (NSInteger i = _bufferReferences.count - 1; i >= 0; i --) {
        CEESourceBufferReferenceContext* reference = _bufferReferences[i];
        if ([reference.filePath isEqualToString:filePath]) {
            bufferOffsets = @{
                @"presentedLineBufferOffset" : @(reference.presentedLineBufferOffset),
                @"caretBufferOffset" : @(reference.caretBufferOffset)
            };
            break;
        }
    }
    return bufferOffsets;
}

- (NSArray*)openSourceBuffersWithFilePaths:(NSArray*)filePaths {
    NSMutableArray* targets = nil;
    NSDictionary* bufferOffsets = nil;
    NSInteger presentedLineBufferOffset = 0;
    NSInteger caretBufferOffset = 0;
    
    for (NSString* filePath in filePaths) {
        CEESourceBuffer* target = nil;
        for (CEESourceBuffer* buffer in _openedSourceBuffers) {
            if ([buffer.filePath isEqualToString:filePath]) {
                target = buffer;
                break;
            }
        }
        
        if (!target) {
            target = [self securityOpenSourceBufferWithFilePath:filePath];
            if (target)
                [_openedSourceBuffers addObject:target];
        }
        
        if (target) {
            if (!targets)
                targets = [[NSMutableArray alloc] init];
            
            [targets addObject:target];
            
            if (!target.symbol_wrappers)
                cee_source_buffer_parse(target, kCEESourceBufferParserOptionCreateSymbolWrapper);
            
            bufferOffsets = [self lastPresentedBufferOffsetInHistory:target.filePath];
            if (bufferOffsets) {
                presentedLineBufferOffset = [bufferOffsets[@"presentedLineBufferOffset"] integerValue];
                caretBufferOffset = [bufferOffsets[@"caretBufferOffset"] integerValue];
            }
            
            [self appendBufferReference:target];
            [self.currentBufferReference setPresentedLineBufferOffset:presentedLineBufferOffset];
            [self.currentBufferReference setCaretBufferOffset:caretBufferOffset];
        }
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
    return targets;
}

- (CEESourceBuffer*)openUntitledSourceBuffer {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = [sourceBufferManager openUntitledSourceBuffer];
    if (buffer) {
        [self.session.project addSecurityBookmarksWithFilePaths:@[buffer.filePath]];
        [_openedSourceBuffers addObject:buffer];
        [self appendBufferReference:buffer];
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
    }
    return buffer;
}

- (void)setActivedSourceBuffer:(CEESourceBuffer*)buffer {
    if (!buffer.symbol_wrappers)
        cee_source_buffer_parse(buffer, kCEESourceBufferParserOptionCreateSymbolWrapper);
    
    NSInteger presentedLineBufferOffset = 0;
    NSInteger caretBufferOffset = 0;
    NSDictionary* bufferOffsets = [self lastPresentedBufferOffsetInHistory:buffer.filePath];
    if (bufferOffsets) {
        presentedLineBufferOffset = [bufferOffsets[@"presentedLineBufferOffset"] integerValue];
        caretBufferOffset = [bufferOffsets[@"caretBufferOffset"] integerValue];
    }
    
    [self appendBufferReference:buffer];
    [self.currentBufferReference setPresentedLineBufferOffset:presentedLineBufferOffset];
    [self.currentBufferReference setCaretBufferOffset:caretBufferOffset];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortActiveSourceBuffer object:self];
}

- (CEESourceBuffer*)activedSourceBuffer {
    CEESourceBufferReferenceContext* reference = [self currentBufferReference];
    for (CEESourceBuffer* buffer in _openedSourceBuffers)
        if ([buffer.filePath isEqualToString:reference.filePath])
            return buffer;
    return nil;
}

- (CEESourceBuffer*)findNextActivedWithCloseSourceBuffers:(NSArray*)buffers {
    CEESourceBuffer* currentActived = [self activedSourceBuffer];
    NSInteger currentActivedIndex = [_openedSourceBuffers indexOfObject:currentActived];
    NSInteger nextActivedIndexBeforeCurrent = -1;
    CEESourceBuffer* nextActivedBeforeCurrent = nil;
    NSInteger nextActivedIndexAfterCurrent = -1;
    CEESourceBuffer* nextActivedAfterCurrent = nil;
    
    nextActivedIndexBeforeCurrent = currentActivedIndex - 1;
    while (nextActivedIndexBeforeCurrent >= 0) {
        CEESourceBuffer* buffer = [_openedSourceBuffers objectAtIndex:nextActivedIndexBeforeCurrent];
        if (![buffers containsObject:buffer]) {
            nextActivedBeforeCurrent = buffer;
            break;
        }
        nextActivedIndexBeforeCurrent --;
    }
    
    nextActivedIndexAfterCurrent = currentActivedIndex + 1;
    while (nextActivedIndexAfterCurrent < _openedSourceBuffers.count) {
        CEESourceBuffer* buffer = [_openedSourceBuffers objectAtIndex:nextActivedIndexAfterCurrent];
        if (![buffers containsObject:buffer]) {
            nextActivedAfterCurrent = buffer;
            break;
        }
        nextActivedIndexAfterCurrent ++;
    }
    
    return nextActivedBeforeCurrent ? nextActivedBeforeCurrent : nextActivedAfterCurrent;    
}

- (void)closeSourceBuffers:(NSArray*)buffers {    
    CEESourceBuffer* currentActived = [self activedSourceBuffer];
    CEESourceBuffer* nextActived = nil;
    if ([buffers containsObject:currentActived]) 
        nextActived = [self findNextActivedWithCloseSourceBuffers:buffers];
    
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    for (CEESourceBuffer* buffer in buffers) {
        [sourceBufferManager closeSourceBuffer:buffer];
        [_openedSourceBuffers removeObject:buffer];
    }
    _descriptor = @"";
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCloseSourceBuffer object:self];
    
    if (nextActived)
        [self setActivedSourceBuffer:nextActived];
    
    if (!_openedSourceBuffers.count)
        [self.session deletePort:self];
}

- (void)closeAllSourceBuffers {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    for (CEESourceBuffer* buffer in _openedSourceBuffers)
        [sourceBufferManager closeSourceBuffer:buffer];
    _openedSourceBuffers = [[NSMutableArray alloc] init];
    _descriptor = @"";
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCloseSourceBuffer object:self];
}

- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    
    NSString* filePathBeforeSave = buffer.filePath;
    
    BOOL withNewFilePath = NO;
    if (![buffer.filePath isEqualToString:filePath])
        withNewFilePath = YES;
    
    if (withNewFilePath) {
        BOOL ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
        if (!ret) {
            NSLog(@"Save Source Buffer Failed!");
            return;
        }
        [self.session.project addSecurityBookmarksWithFilePaths:@[filePath]];
        
        if ([sourceBufferManager isTemporaryFilePath:filePathBeforeSave]) {
            CEEProjectController* controller = (CEEProjectController*)[NSDocumentController sharedDocumentController];
            [controller replaceSourceBufferReferenceFilePath:filePathBeforeSave to:filePath];
        }
        else {
            [self openSourceBuffersWithFilePaths:@[filePath]];
        }
    }
    else {
        [self securitySaveSourceBuffer:buffer atFilePath:filePath];
    }
    
    [self.session.project syncSourceSymbols:buffer];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSaveSourceBuffer object:self userInfo:@{@"sourceBuffer" : buffer}];
}

- (NSString*)serialize {
    CEESourceBufferReferenceContext* reference = nil;
    CEESourceBuffer* buffer = nil;
    
    // session port serialize start
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    
    // opened buffer begin
    serializing = [serializing stringByAppendingFormat:@"\"openedSourceBuffers\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < _openedSourceBuffers.count; i ++) {
        buffer = _openedSourceBuffers[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"\"filePath\":\"%@\"", buffer.filePath];
        serializing = [serializing stringByAppendingFormat:@"}"];
        
        if (i < _openedSourceBuffers.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
    }
    serializing = [serializing stringByAppendingFormat:@"]"];
    // opened buffer end
    serializing = [serializing stringByAppendingFormat:@","];
        
    // reference index begin
    serializing = [serializing stringByAppendingFormat:@"\"sourceBufferReferenceIndex\":\"%ld\"", _bufferReferenceIndex];
    // reference index end
    serializing = [serializing stringByAppendingFormat:@","];
        
    // references begin
    serializing = [serializing stringByAppendingFormat:@"\"sourceBufferReferences\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < _bufferReferences.count; i ++) {
        reference = _bufferReferences[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"\"filePath\":\"%@\"", reference.filePath];
        serializing = [serializing stringByAppendingFormat:@","];
        serializing = [serializing stringByAppendingFormat:@"\"presentedLineBufferOffset\":\"%lu\"", reference.presentedLineBufferOffset];
        serializing = [serializing stringByAppendingFormat:@","];
        serializing = [serializing stringByAppendingFormat:@"\"caretBufferOffset\":\"%lu\"", reference.caretBufferOffset];
        serializing = [serializing stringByAppendingFormat:@"}"];
        
        if (i < _bufferReferences.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
            
    }
    serializing = [serializing stringByAppendingFormat:@"]"];
    // references end
    
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    NSString* identifier = [dict allKeys][0];
    NSDictionary* descriptor = dict[identifier];
    CEESourceBuffer* buffer = nil;
    CEESourceBufferReferenceContext* reference = nil;
    
    // recover Opened Source Buffers
    NSArray* openBufferDescriptors = descriptor[@"openedSourceBuffers"];
    for (NSDictionary* openBufferDescriptor in openBufferDescriptors) {
        NSString* filePath = openBufferDescriptor[@"filePath"];        
        buffer = [self securityOpenSourceBufferWithFilePath:filePath];
        if (buffer)
            [_openedSourceBuffers addObject:buffer];
    }
    
    // recover Source Buffer References
    NSArray* referenceDescriptors = descriptor[@"sourceBufferReferences"];
    for (NSDictionary* referenceDescriptor in referenceDescriptors) {
        NSString* filePath = referenceDescriptor[@"filePath"];
        NSString* presentedLineBufferOffset = referenceDescriptor[@"presentedLineBufferOffset"];
        NSString* caretBufferOffset = referenceDescriptor[@"caretBufferOffset"];
        reference = [[CEESourceBufferReferenceContext alloc] initWithFilePath:filePath];
        [reference setPresentedLineBufferOffset:[presentedLineBufferOffset integerValue]];
        [reference setCaretBufferOffset:[caretBufferOffset integerValue]];
        [_bufferReferences addObject:reference];
    }
    
    // recover Source Buffer Reference Index
    NSString* referenceIndexDescriptor = descriptor[@"sourceBufferReferenceIndex"];
    _bufferReferenceIndex = [referenceIndexDescriptor integerValue];
    
     // recover Context
    reference = _bufferReferences[_bufferReferenceIndex];
    CEESourceBuffer* target = nil;
    for (CEESourceBuffer* buffer in _openedSourceBuffers) {
        if ([buffer.filePath isEqualToString:reference.filePath]) {
            target = buffer;
            break;
        }
    }
    
    if (target) {
        cee_source_buffer_parse(target, kCEESourceBufferParserOptionCreateSymbolWrapper);
        const cee_uchar* subject = cee_text_storage_buffer_get(target.storage);
        CEERange range = cee_range_make(0, cee_text_storage_size_get(target.storage));
        CEEList* symbolReferences = NULL;
        cee_source_reference_parse(target.parser_ref, 
                                   (const cee_uchar*)[target.filePath UTF8String], 
                                   subject, 
                                   target.source_token_map, 
                                   target.prep_directive, 
                                   target.statement, 
                                   range, 
                                   &symbolReferences);
        cee_long offset = reference.caretBufferOffset;
        CEETokenCluster* cluster = cee_token_cluster_search_by_buffer_offset(symbolReferences, 
                                                                             target.prep_directive, 
                                                                             target.statement, 
                                                                             offset);
        if (cluster) {
            _context = [self _createContextByCluster:cluster];
            cee_token_cluster_free(cluster);
        }
        cee_list_free_full(symbolReferences, cee_source_symbol_reference_free);
    }
    
    _identifier = identifier;
}

- (void)discardReferences {    
    _bufferReferences = [[NSMutableArray alloc] init];
    _bufferReferenceIndex = -1;
}

- (void)dealloc {
    if (_context)
        cee_list_free_full(_context, cee_source_symbol_free);
    _context = NULL;
    
    if (_target_symbol)
        cee_source_symbol_free(_target_symbol);
    _target_symbol = NULL;
    
    if (_actived_symbol)
        cee_source_symbol_free(_actived_symbol);
    _actived_symbol = NULL;
}

- (CEEList*)_createContextByCluster:(CEETokenCluster*)cluster {
    if (!cluster)
        return NULL;
    
    CEESourceSymbolReference* symbolReference = NULL;
    CEESourceSymbol* symbol = NULL;
    CEEList* context = NULL;
    CEESourceBufferReferenceContext* bufferReference = [self currentBufferReference];
    CEESourceBuffer* buffer = [self bufferInContext:bufferReference];
    
    CEESourceReferenceSearchOption options = kCEESourceReferenceSearchOptionLocal | 
        kCEESourceReferenceSearchOptionGlobal;
    
    if (cluster->type == kCEETokenClusterTypeReference) {
        symbolReference = (CEESourceSymbolReference*)cluster->content_ref;
        context = cee_symbols_search_by_reference(symbolReference,
                                                  buffer.prep_directive,
                                                  buffer.statement,
                                                  self.session.project.database,
                                                  options);
    }
    else if (cluster->type == kCEETokenClusterTypeSymbol) {
        symbol = (CEESourceSymbol*)cluster->content_ref;
        context = cee_database_symbols_search_by_name(self.session.project.database,
                                                      symbol->name);
    }
    return context;
}

- (void)createContextByCluster:(CEETokenCluster*)cluster {
    if (!cluster)
        return;
    
    CEEList* context = [self _createContextByCluster:cluster];
    if (!context)
        return;

    if (_context)
        cee_list_free_full(_context, cee_source_symbol_free);
    _context = context;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCreateContext object:self];
}

- (void)jumpToTargetSymbolByCluster:(CEETokenCluster*)cluster {
    CEEList* context = [self _createContextByCluster:cluster];
    if (!context)
        return;
        
    if (_context)
        cee_list_free_full(_context, cee_source_symbol_free);
    _context = context;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCreateContext object:self];
    
    if (cee_list_length(_context) == 1)
        [self setTargetSourceSymbol:cee_list_nth_data(_context, 0)];
    else
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortRequestTargetSymbolSelection object:self];
}

- (void)searchReferencesByCluster:(CEETokenCluster*)cluster {
    if (!cluster)
        return;

    NSString* referenceString;
    CEESourceSymbolReference* reference = NULL;
    CEESourceSymbol* symbol = NULL;
    if (cluster->type == kCEETokenClusterTypeReference) {
        reference = (CEESourceSymbolReference*)cluster->content_ref;
        referenceString = [NSString stringWithUTF8String:reference->name];
    }
    else if (cluster->type == kCEETokenClusterTypeSymbol) {
        symbol = (CEESourceSymbol*)cluster->content_ref;
        referenceString = [NSString stringWithUTF8String:symbol->name];
    }
    referenceString = [referenceString stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    
    self.session.project.searcher.target = referenceString;
    
    if (![referenceString isEqualToString:@""])
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSearchReference object:self userInfo:@{@"referenceString": referenceString}];
}

- (void)setTargetSourceSymbol:(CEESourceSymbol*)symbol {
    _target_symbol = cee_source_symbol_copy(symbol);
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSetTargetSymbol object:self];
}

- (void)setActivedSourceSymbol:(CEESourceSymbol*)symbol {
    _actived_symbol = cee_source_symbol_copy(symbol);
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSetActivedSymbol object:self];
}

- (NSString*)descriptor {
    return _descriptor;
}

- (void)setDescriptor:(NSString*)descriptor {
    _descriptor = descriptor;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSetDescriptor object:self];
}

@end

@implementation CEESession

@synthesize activedPort = _activedPort;

- (instancetype)init {
    self = [super init];
    if (self) {
        _identifier = CreateObjectID(self);
        [self setActivedPort:[self createPort]];
        [self createDescriptor:init_descriptor_template];
    }
    return self;
}

- (void)addPort:(CEESessionPort*)port {
    if (!_ports)
        _ports = [[NSMutableArray alloc] init];
    [_ports addObject:port];
}

- (CEESessionPort*)createPort {
    CEESessionPort* port = [[CEESessionPort alloc] init];
    if (!port)
        return nil;
    
    [port setSession:self];
    [self addPort:port];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionCreatePort object:self];
    return port;
}

- (void)deletePort:(CEESessionPort*)port {
    [port closeAllSourceBuffers];
    [_ports removeObject:port];
    if (port == _activedPort)
        _activedPort = nil;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionDeletePort object:self];
}

- (void)deleteAllPorts {
    for (CEESessionPort* port in _ports)
        [port closeAllSourceBuffers];
    _ports = nil;
    _activedPort = nil;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionDeletePort object:self];
}

- (void)setActivedPort:(CEESessionPort*)port {
    _activedPort = port;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionActivePort object:self];
}

- (CEESessionPort*)activedPort {
    return _activedPort;
}

- (NSArray*)filePathsFilter:(NSString*)condition {
    return [_project getFilePathsWithCondition:condition];
}

- (NSString*)serialize {
    AppDelegate* delegate = [NSApp delegate];
    
    // session serialize start
    NSString* serializing = @"{";
    
    // version begin
    serializing = [serializing stringByAppendingFormat:@"\"serializer\":\"%@\"", [delegate serializerVersionString]];
    // version end
    serializing = [serializing stringByAppendingFormat:@","];
    
    // session window serialize begin
    serializing = [serializing stringByAppendingFormat:@"\"ui\":"];
    serializing = [serializing stringByAppendingFormat:@"{"];
    for (CEESessionWindowController* controller in _project.windowControllers) {
        if (controller.session == self)
            serializing = [serializing stringByAppendingFormat:@"%@", [controller serialize]];
    }
    serializing = [serializing stringByAppendingFormat:@"}"];
    // session window serialize end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // session ports serialize begin
    serializing = [serializing stringByAppendingFormat:@"\"ports\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < _ports.count; i ++) {
        CEESessionPort* port = _ports[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"%@", [port serialize]];
        serializing = [serializing stringByAppendingFormat:@"}"];
        if (i < _ports.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
    }
    serializing = [serializing stringByAppendingFormat:@"]"];
    // session ports serialize end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // active port serialize begin
    if (_activedPort)
        serializing = [serializing stringByAppendingFormat:@"\"activePort\":\"%@\"", _activedPort.identifier];
    else
        serializing = [serializing stringByAppendingFormat:@"\"activePort\":\"%@\"", @""];
    // active port serialize end
    
    serializing = [serializing stringByAppendingFormat:@"}"];
    
    //NSLog(@"%@", serializing);
    
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    _descriptor = dict;
    
    [self deleteAllPorts];
    NSArray* portDescriptors = _descriptor[@"ports"];
    for (NSDictionary* portDescriptor in portDescriptors) {
        CEESessionPort* port = [[CEESessionPort alloc] init];
        if (port) {
            [port setSession:self];
            [self addPort:port];
            [port deserialize:portDescriptor];
        }
    }
    
    NSString* activePortIdentifier = _descriptor[@"activePort"];
    for (CEESessionPort* port in _ports) {
        if ([port.identifier compare:activePortIdentifier options:NSLiteralSearch] == NSOrderedSame) {
            [self setActivedPort:port];
            break;
        }
    }
}

- (void)createDescriptor:(const char*)template {
    // session serialize start
    NSString* serializing = @"{";
    
    // session window serialize begin
    serializing = [serializing stringByAppendingFormat:@"\"ui\":"];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"%s", template];
    
    serializing = [serializing stringByAppendingFormat:@"}"];
    // session window serialize end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // session ports serialize begin
    serializing = [serializing stringByAppendingFormat:@"\"ports\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < _ports.count; i ++) {
        CEESessionPort* port = _ports[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"%@", [port serialize]];
        serializing = [serializing stringByAppendingFormat:@"}"];
        if (i < _ports.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
    }
    serializing = [serializing stringByAppendingFormat:@"]"];
    // session ports serialize end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // active port serialize begin
    serializing = [serializing stringByAppendingFormat:@"\"activePort\":\"%@\"", _activedPort.identifier];
    // active port serialize end
        
    serializing = [serializing stringByAppendingFormat:@"}"];
    
    if (_ports.count) {
        CEESessionPort* port = _ports[0];
        NSString* frameViewIdentifier = [@"IDSessionFrameView#" stringByAppendingFormat:@"%@", IdentifierByDeletingPrefix(port.identifier)];
        serializing = [serializing stringByReplacingOccurrencesOfString:@"##IDSessionFrameView##" withString:frameViewIdentifier];
        _descriptor = JSONDictionaryFromString(serializing);
    }
}

@end

@implementation CEEProject

@synthesize properties = _properties;

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    _properties = [CEEProjectSetting projectSettingWithName:nil path:nil filePaths:nil filePathsUserSelected:nil];
    _sessions = [[NSMutableArray alloc] init];
    // create an init session
    CEESession* session = [self createSession];
    [session.project setCurrentSession:session];
    [session.activedPort openUntitledSourceBuffer];
    _searcher = [[CEEProjectSearcher alloc] init];
    return self;
}

- (CEESession*)createSession {
    CEESession* session = [[CEESession alloc] init];
    [session setProject:self];
    [_sessions addObject:session];
    return session;
}

- (BOOL)readFromURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError * _Nullable *)outError { 
    if ([typeName caseInsensitiveCompare:@"com.lazycatdesign.cymbols.cymd"] == NSOrderedSame) {
        [self deserialize:[url path]];
        if (!self.database)
            return NO;
    }
    return YES;
}

- (void)makeWindowControllers {
    for (CEESession* session in _sessions)
        [self makeWindowControllerFromSession:session];
}

- (void)makeWindowControllerFromSession:(CEESession*)session {
    CEESessionWindowController* controller = [[NSStoryboard storyboardWithName:@"Session" bundle:nil] instantiateControllerWithIdentifier:@"IDSessionWindowController"];
    [controller setIdentifier:CreateObjectID(controller)];
    [controller setSession:session];
    [controller deserialize:session.descriptor[@"ui"][@"window"]];
    [self addWindowController:controller];
    [controller showWindow:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPresent object:self];
}

- (void)createSessionWithFilePaths:(NSArray*)filePaths {
    CEESession* session = [self createSession];
    [session.project setCurrentSession:session];
    [session.activedPort openSourceBuffersWithFilePaths:filePaths];
    [session createDescriptor:open_file_descriptor_template];
    [self makeWindowControllerFromSession:session];
}

- (void)deleteSession:(CEESession*)session {
    [session deleteAllPorts];
    for (NSWindowController* controller in self.windowControllers) {
        if ([controller isKindOfClass:[CEESessionWindowController class]]) {
            if (((CEESessionWindowController*)controller).session == session) {
                [controller close];
                [self removeWindowController:controller];
            }
        }
    }
    [_sessions removeObject:session];
}

- (void)deleteAllSessions {
    for (CEESession* session in _sessions) {
        [session deleteAllPorts];
        for (NSWindowController* controller in self.windowControllers) {
            if ([controller isKindOfClass:[CEESessionWindowController class]]) {
                if (((CEESessionWindowController*)controller).session == session) {
                    [controller close];
                    [self removeWindowController:controller];
                }
            }
        }
    }
    _sessions = [[NSMutableArray alloc] init];
    self.currentSession = nil;
}

- (void)setProperties:(CEEProjectSetting*)properties {
    
    if (!properties || !properties.name || !properties.path || !properties.filePathsExpanded)
        return;
    
    _properties = properties;
    
    NSString* backendName = [properties.name stringByAppendingPathExtension:@"cymd"];
    NSString* projectPath = [properties.path stringByAppendingPathComponent:properties.name];
    NSString* backendPath = [projectPath stringByAppendingPathComponent:backendName];
    
    [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[NSURL fileURLWithPath:backendPath]];
    _database = cee_database_create([backendPath UTF8String]);
    
    // turn off journal mode
    if (!_database)
        return ;
    
    // turn off journal mode, apple's sandbox forbid the journal file created
    sqlite3_exec(_database, "PRAGMA journal_mode=OFF;", NULL, 0, 0);

    // save filePathsExpanded
    NSArray* filePaths = properties.filePathsExpanded;
    CEEList* file_paths_expanded = NULL;
    for (NSString* filePath in filePaths)
        file_paths_expanded = cee_list_prepend(file_paths_expanded, cee_strdup([filePath UTF8String]));
    cee_database_filepaths_append(_database, file_paths_expanded);
    cee_list_free_full(file_paths_expanded, cee_free);
    file_paths_expanded = NULL;
    
    // save filePathsUserSelected
    NSArray* filePathsUserSelected = properties.filePathsUserSelected;
    CEEList* file_paths_user_selected = NULL;
    for (NSString* filePath in filePathsUserSelected)
        file_paths_user_selected = cee_list_prepend(file_paths_user_selected, cee_strdup([filePath UTF8String]));
    cee_database_filepaths_user_selected_append(_database, file_paths_user_selected);
    cee_list_free_full(file_paths_user_selected, cee_free);
    file_paths_user_selected = NULL;
        
    // save User Selected File Path Bookmark
    NSArray* bookmarks = CreateBookmarksWithFilePaths(properties.filePathsUserSelected);
    for (CEESecurityBookmark* bookmark in bookmarks) {
        cee_database_security_bookmark_append(_database, 
                                              [bookmark.filePath UTF8String], 
                                              [bookmark.content UTF8String]);
    }
    
    for (CEESession* session in self.sessions) {
        for (CEESessionPort* port in session.ports) {
            [port closeAllSourceBuffers];
            [port discardReferences];
        }
    }
    
    if (properties.filePathsExpanded) {
        for (CEESession* session in self.sessions) {
            [session.activedPort openSourceBuffersWithFilePaths:@[properties.filePathsExpanded[0]]];
        }
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectSettingProperties object:self];
}

- (CEEProjectSetting*)properties {
    return _properties;
}

- (NSArray*)getFilePathsUserSelected {
    if (!_database)
        return nil;
    
    NSMutableArray* filePaths = nil;
    
    CEEList* file_paths = cee_database_filepaths_user_selected_get(_database);
    CEEList* p = NULL;
    p = file_paths;
    while (p) {
        cee_char* str = p->data;
        if (!filePaths)
            filePaths = [[NSMutableArray alloc] init];
        [filePaths addObject:[NSString stringWithUTF8String:str]];
        p = p->next;
    }
    cee_list_free_full(file_paths, cee_free);
    return filePaths;
}

- (NSArray*)getFilePathsWithCondition:(nullable NSString*)condition {
    if (!_database)
        return nil;
    
    NSMutableArray* filePaths = nil;
    condition = [condition stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    if ([condition isEqualToString:@"*"] || 
        [condition isEqualToString:@""])
        condition = nil;
        
    CEEList* file_paths = cee_database_filepaths_get(_database, [condition UTF8String]);
    CEEList* p = NULL;
    p = file_paths;
    while (p) {
        cee_char* str = p->data;
        if (!filePaths)
            filePaths = [[NSMutableArray alloc] init];
        [filePaths addObject:[NSString stringWithUTF8String:str]];
        p = p->next;
    }
    cee_list_free_full(file_paths, cee_free);
    return filePaths;
}

- (void)addFilePaths:(NSArray*)filePaths {
    filePaths = ExpandFilePaths(filePaths);
    if (!filePaths)
        return;
    
    CEEList* file_paths = NULL;
    for (NSString* filePath in filePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    
    cee_database_filepaths_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectAddFilePaths object:self];
}

- (void)removeFilePaths:(NSArray*)filePaths {
    filePaths = ExpandFilePaths(filePaths);
    if (!filePaths)
        return;
    
    CEEList* file_paths = NULL;
    for (NSString* filePath in filePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    
    cee_database_filepaths_remove(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectRemoveFilePaths object:self];
}

- (CEEProjectSetting*)createEmptyProjectSetting {
    CEEProjectSetting* setting = [CEEProjectSetting projectSettingWithName:@"NewProject" path:NSHomeDirectoryForUser(NSUserName()) filePaths:nil filePathsUserSelected:nil];
    
    return setting;
}

- (NSArray*)directoriesFromFilePaths:(NSArray*)filePaths {
    NSMutableArray* directorise = nil;
    for (NSString* filePath in filePaths) {
        BOOL isDirectory = NO;
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory] && isDirectory) {
            if (!directorise)
                directorise = [[NSMutableArray alloc] init];
            [directorise addObject:filePath];
        }
    }
    return directorise;
}

- (void)deserialize:(NSString*)filePath {
    CEEList* descriptors = NULL;
    CEEList* p = NULL;
    cee_char* descriptor_string = NULL;
    NSDictionary* descriptor = NULL;
    CEESession* session = NULL;
    
    _database = cee_database_open([filePath UTF8String]);
    if (!_database)
        return ;
    
    // turn off journal mode, apple's sandbox forbid the journal file created
    sqlite3_exec(_database, "PRAGMA journal_mode=OFF;", NULL, 0, 0);
    
    // clean the init sessions
    [self deleteAllSessions];
    
    NSString* regularFormat = [filePath stringByDeletingLastPathComponent];
    NSString* name = [[regularFormat lastPathComponent] stringByDeletingPathExtension];
    NSString* path = [regularFormat stringByDeletingLastPathComponent];
    
    _properties = [CEEProjectSetting projectSettingWithName:name path:path filePaths:[self getFilePathsWithCondition:nil] filePathsUserSelected:[self getFilePathsUserSelected]];
    
    [self startAccessSecurityScopedDirectories];
    
    // create sessions from session descriptors
    descriptors = cee_database_session_descriptors_get(_database);
    if (descriptors) {
        p = cee_list_last(descriptors);
        while (p) {
            descriptor_string = p->data;
            descriptor = JSONDictionaryFromString([NSString stringWithUTF8String:descriptor_string]);
            session = [self createSession];
            [session deserialize:descriptor];
            [session.project setCurrentSession:session];
            p = p->prev;
        }
        cee_list_free_full(descriptors, cee_free);
    }
    else {
        session = [self createSession];
        [session.project setCurrentSession:session];
    }
}

- (void)startAccessSecurityScopedDirectories {
    NSArray* directories = [self directoriesFromFilePaths:[self getFilePathsUserSelected]];
    NSArray* bookmarks = [self getSecurityBookmarksWithFilePaths:directories];
    [self startAccessSecurityScopedResourcesWithBookmarks:bookmarks];    
}

- (void)stopAccessSecurityScopedDirectories {
    NSArray* directories = [self directoriesFromFilePaths:[self getFilePathsUserSelected]];
    NSArray* bookmarks = [self getSecurityBookmarksWithFilePaths:directories];
    [self stopAccessSecurityScopedResourcesWithBookmarks:bookmarks];
}

- (void)serialize {
    cee_database_session_descriptors_remove(_database);
    for (CEESessionWindowController* controller in self.windowControllers) {
        CEESession* session = controller.session;
        NSString* string = [session serialize];
        cee_database_session_descriptor_append(_database, [string UTF8String]);
    }
}

- (void)syncSourceSymbols:(CEESourceBuffer*)buffer {
    if (!buffer)
        return;
    
    CEEList* list = NULL;
    cee_database_symbols_from_source_clean(_database, [buffer.filePath UTF8String]);
    cee_source_fregment_symbol_tree_dump_to_list(buffer.prep_directive_symbol_tree, &list);
    cee_source_fregment_symbol_tree_dump_to_list(buffer.statement_symbol_tree, &list);
    if (list)
        cee_database_symbols_write(_database, list);
}

- (void)addSecurityBookmarksWithFilePaths:(NSArray*)filePaths {
    NSArray* bookmarks = CreateBookmarksWithFilePaths(filePaths);
    for (CEESecurityBookmark* bookmark in bookmarks) {
        cee_database_security_bookmark_append(_database,
                                              [bookmark.filePath UTF8String],
                                              [bookmark.content UTF8String]);
    }
}

- (void)removeSecurityBookmarksWithFilePaths:(NSArray*)filePaths {
    for (NSString* filePath in filePaths)
        cee_database_security_bookmark_remove(_database, [filePath UTF8String]);
}

- (NSArray*)getSecurityBookmarksWithFilePaths:(NSArray*)filePaths {
    NSMutableArray* bookmarks = nil;
    for (NSString* filePath in filePaths) {
        cee_char* content = cee_database_security_bookmark_content_get(_database, [filePath UTF8String]);
        if (content) {
            CEESecurityBookmark* bookmark = [[CEESecurityBookmark alloc] init];
            bookmark.filePath = filePath;
            bookmark.content = [NSString stringWithUTF8String:content];
            cee_free(content);
            if (!bookmarks)
                bookmarks = [[NSMutableArray alloc] init];
            [bookmarks addObject:bookmark];
        }
    }
    return bookmarks;
}

- (void)startAccessSecurityScopedResourcesWithBookmarks:(NSArray*)bookmarks {
    for (CEESecurityBookmark* bookmark in bookmarks) {
        BOOL isStale = NO;
        NSError* error = nil;
        BOOL isSuccess = NO;
        NSData* bookmarkData = [[NSData alloc] initWithBase64EncodedString:bookmark.content options:NSDataBase64DecodingIgnoreUnknownCharacters];
        NSURL *allowedUrl = [NSURL URLByResolvingBookmarkData:bookmarkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:&isStale error:&error];
        if (allowedUrl)
            isSuccess = [allowedUrl startAccessingSecurityScopedResource];
    }
}

- (void)stopAccessSecurityScopedResourcesWithBookmarks:(NSArray*)bookmarks {
    for (CEESecurityBookmark* bookmark in bookmarks) {
        BOOL isStale = NO;
        NSError* error = nil;
        NSData* bookmarkData = [[NSData alloc] initWithBase64EncodedString:bookmark.content options:NSDataBase64DecodingIgnoreUnknownCharacters];
        NSURL *allowedUrl = [NSURL URLByResolvingBookmarkData:bookmarkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:&isStale error:&error];
        if (allowedUrl)
            [allowedUrl stopAccessingSecurityScopedResource];
    }
}

- (void)close {
    [self stopAccessSecurityScopedDirectories];
    cee_database_close(_database);
    [super close];
}

@end

@implementation CEEProjectController

- (CEEProject*)createProjectFromSetting:(CEEProjectSetting*)setting {
    CEEProject* project = [[CEEProject alloc] init];
    NSString* databasPath = [project.properties databasePath];
    
    [self addDocument:project];    
    [project setProperties:setting];
    [project makeWindowControllers];
    
    if (databasPath)
        [self noteNewRecentDocumentURL:[NSURL fileURLWithPath:databasPath]];
    return project;
}

- (CEEProject*)openProjectFromURL:(NSURL*)url {
    CEEProject* project = [[CEEProject alloc] init];
    [self addDocument:project];
    [project deserialize:[url path]];
    [project makeWindowControllers];
    [self noteNewRecentDocumentURL:[NSURL fileURLWithPath:[project.properties databasePath]]];
    return project;
}


- (void)openDocumentWithContentsOfURL:(NSURL*)url display:(BOOL)displayDocument completionHandler:(void (^)(NSDocument *document, BOOL documentWasAlreadyOpen, NSError *error))completionHandler {
    [super openDocumentWithContentsOfURL:url display:displayDocument completionHandler:(^(NSDocument *document, BOOL documentWasAlreadyOpen, NSError *error) {
        completionHandler(document, documentWasAlreadyOpen, error);
    })];
}

- (void)clearRecentDocuments:(id)sender {
    [super clearRecentDocuments:sender];
}

- (__kindof NSDocument *)documentForURL:(NSURL *)url {
    for (CEEProject* project in self.documents) {
        if ([[project.properties databasePath] isEqualToString:[url path]])
            return project;
    }
    return nil;
}

- (void)replaceSourceBufferReferenceFilePath:(NSString*)filePath0 to:(NSString*)filePath1 {
    for (CEEProject* project in self.documents) {
        for (CEESession* session in project.sessions) {
            for (CEESessionPort* port in session.ports) {
                for (CEESourceBufferReferenceContext* reference in port.bufferReferences) {
                    if ([reference.filePath isEqualToString:filePath0])
                        reference.filePath = filePath1;
                }
            }
        }
    }
}

@end
