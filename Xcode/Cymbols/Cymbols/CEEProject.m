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
#import "CEEJSONReader.h"
#import "cee_symbol.h"
#import "cee_backend.h"
#import "cee_source_parser.h"
#include <unistd.h>

extern char* init_descriptor_template;
extern char* open_file_descriptor_template;

NSNotificationName CEENotificationProjectSettingProperties = @"CEENotificationProjectSettingProperties";
NSNotificationName CEENotificationProjectAddReference = @"CEENotificationProjectAddReference";
NSNotificationName CEENotificationProjectRemoveReference = @"CEENotificationProjectRemoveReference";
NSNotificationName CEENotificationProjectCreateFile = @"CEENotificationProjectCreateFile";
NSNotificationName CEENotificationProjectRemoveFile = @"CEENotificationProjectRemoveFile";
NSNotificationName CEENotificationSessionPortOpenSourceBuffer = @"CEENotificationSessionPortOpenSourceBuffer";
NSNotificationName CEENotificationSessionPortCloseSourceBuffer = @"CEENotificationSessionPortCloseSourceBuffer";
NSNotificationName CEENotificationSessionPortActiveSourceBuffer = @"CEENotificationSessionPortActiveSourceBuffer";
NSNotificationName CEENotificationSessionPresent = @"CEENotificationSessionPresent";
NSNotificationName CEENotificationSessionCreatePort = @"CEENotificationSessionCreatePort";
NSNotificationName CEENotificationSessionActivePort = @"CEENotificationSessionActivePort";
NSNotificationName CEENotificationSessionDeletePort = @"CEENotificationSessionDeletePort";
NSNotificationName CEENotificationSessionPortCreateSourceContext = @"CEENotificationSessionPortCreateSourceContext";
NSNotificationName CEENotificationSessionPortJumpToSymbolRequest = @"CEENotificationSessionPortJumpToSymbolRequest";
NSNotificationName CEENotificationSessionPortJumpToSourcePoint = @"CEENotificationSessionPortJumpToSourcePoint";
NSNotificationName CEENotificationSessionPortPresentHistory = @"CEENotificationSessionPortPresentHistory";
NSNotificationName CEENotificationSessionPortSaveSourceBuffer = @"CEENotificationSessionPortSaveSourceBuffer";
NSNotificationName CEENotificationSessionPortSetDescriptor = @"CEENotificationSessionPortSetDescriptor";
NSNotificationName CEENotificationSessionPortSearchReference = @"CEENotificationSessionPortSearchReference";

NSArray* ExpandFilePaths(NSArray* filePaths)
{
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
    return [[CEESecurityBookmark alloc] initWithFilePath:filePath];
}

BOOL ContextContainSymbol(CEEList* context,
                          CEESourceSymbol* symbol)
{
    if (!context)
        return NO;
    
    CEEList* p = context;
    while (p) {
        CEESourceSymbol* test = p->data;
        
        CEERange range0 = cee_range_consistent_from_discrete(test->ranges);
        CEERange range1 = cee_range_consistent_from_discrete(symbol->ranges);
        
        if (!strcmp(test->file_path, symbol->file_path) &&
            (range0.location == range1.location && range0.length == range1.length))
            return YES;
        p = p->next;
    }
    return FALSE;
}


@implementation CEESourceBufferReferenceContext

- (instancetype)initWithFilePath:(NSString*)filePath {
    self = [super init];
    if (!self)
        return nil;
    
    _filePath = filePath;
    _lineBufferOffset = 0;
    _caretBufferOffset = 0;
    return self;
}

@end

@implementation CEEProjectSetting

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    AppDelegate* delegate = [NSApp delegate];
    NSString* filePath = [delegate defaultProjectSettingPath];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        NSMutableDictionary* descriptor = [CEEJSONReader objectFromFile:filePath];
        CEESecurityBookmark* bookmark = [[CEESecurityBookmark alloc] init];
        bookmark.filePath = descriptor[@"filePath"];
        bookmark.content = descriptor[@"bookmarkContent"];
        self.pathBookmark = bookmark;
        self.path = bookmark.filePath;
    }
    self.name = @"NewProject";
    return self;
}

- (void)saveAsDefaultSetting {
    AppDelegate* delegate = [NSApp delegate];
    NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
    CEESecurityBookmark* bookmark = self.pathBookmark;
    if (bookmark) {
        [dict setValue:bookmark.filePath forKey:@"filePath"];
        [dict setValue:bookmark.content forKey:@"bookmarkContent"];
    }
    NSString* filePath = [delegate defaultProjectSettingPath];
    [CEEJSONReader object:dict toFile:filePath];
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
        _sourceBufferReferences = [[NSMutableArray alloc] init];
        _bufferReferenceIndex = -1;
        _identifier = CreateObjectID(self);
        _openedSourceBuffers = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)appendBufferReference:(CEESourceBuffer*)buffer {
    CEESourceBufferReferenceContext* current = [self currentSourceBufferReference];
    if (current) {
        if ([current.filePath isEqualToString:buffer.filePath])
            return;
    }
    
    CEESourceBufferReferenceContext* refernce = [[CEESourceBufferReferenceContext alloc] initWithFilePath:buffer.filePath];
    if (!_sourceBufferReferences.count) {
        [_sourceBufferReferences addObject:refernce];
    }
    else {
        if (_bufferReferenceIndex < _sourceBufferReferences.count - 1) {
            NSRange range = NSMakeRange(_bufferReferenceIndex + 1, _sourceBufferReferences.count - (_bufferReferenceIndex + 1));
            [_sourceBufferReferences removeObjectsInRange:range];
        }
        
        [_sourceBufferReferences addObject:refernce];
    }
    
    _bufferReferenceIndex ++;
}

- (void)moveSourceBufferReferenceNext {
    if (_bufferReferenceIndex < _sourceBufferReferences.count - 1) {
        _bufferReferenceIndex ++;
        [self presentHistory:_sourceBufferReferences[_bufferReferenceIndex]];
    }
}

- (void)moveSourceBufferReferencePrev {
    if (_bufferReferenceIndex > 0) {
        _bufferReferenceIndex --;
        [self presentHistory:_sourceBufferReferences[_bufferReferenceIndex]];
    }
}

- (CEESourceBufferReferenceContext*)currentSourceBufferReference {
    if (_bufferReferenceIndex < 0 || _bufferReferenceIndex >= _sourceBufferReferences.count)
        return nil;
    return _sourceBufferReferences[_bufferReferenceIndex];
}

- (void)presentHistory:(CEESourceBufferReferenceContext*)reference {
    CEESourceBuffer* target = nil;
    
    for (NSInteger i = 0; i < _sourceBufferReferences.count; i ++) {
        if (reference == _sourceBufferReferences[i])
            _bufferReferenceIndex = i;
    }
    
    reference = [self currentSourceBufferReference];
    for (CEESourceBuffer* buffer in _openedSourceBuffers) {
        if ([buffer.filePath isEqualToString:reference.filePath]) {
            target = buffer;
            break;
        }
    }
    
    if (!target) {
        target = [self openSourceBufferWithFilePath:reference.filePath];
        if (target)
            [_openedSourceBuffers addObject:target];
    }

    if (target) {
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortPresentHistory object:self];
    }
}

- (NSDictionary*)lastPresentedBufferOffsetInHistory:(NSString*)filePath {
    NSDictionary* bufferOffsets = nil;
    for (NSInteger i = _sourceBufferReferences.count - 1; i >= 0; i --) {
        CEESourceBufferReferenceContext* reference = _sourceBufferReferences[i];
        if ([reference.filePath isEqualToString:filePath]) {
            bufferOffsets = @{
                @"lineBufferOffset" : @(reference.lineBufferOffset),
                @"caretBufferOffset" : @(reference.caretBufferOffset)
            };
            break;
        }
    }
    return bufferOffsets;
}

- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString*)filePath {
    NSDictionary* bufferOffsets = nil;
    NSInteger lineBufferOffset = 0;
    NSInteger caretBufferOffset = 0;
    
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
        bufferOffsets = [self lastPresentedBufferOffsetInHistory:target.filePath];
        if (bufferOffsets) {
            lineBufferOffset = [bufferOffsets[@"lineBufferOffset"] integerValue];
            caretBufferOffset = [bufferOffsets[@"caretBufferOffset"] integerValue];
        }
        
        [self appendBufferReference:target];
        [self.currentSourceBufferReference setLineBufferOffset:lineBufferOffset];
        [self.currentSourceBufferReference setCaretBufferOffset:caretBufferOffset];
    }
    
    if (target)
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
    
    return target;
}

- (CEESourceBuffer*)openUntitledSourceBuffer {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = [sourceBufferManager openUntitledSourceBuffer];
    if (buffer) {
        [_openedSourceBuffers addObject:buffer];
        [self appendBufferReference:buffer];
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
    }
    return buffer;
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
    [_openedSourceBuffers removeAllObjects];
    _descriptor = @"";
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCloseSourceBuffer object:self];
}

- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    NSString* filePathBeforeSave = buffer.filePath;
    BOOL ret = NO;
    BOOL withNewFilePath = NO;
    if (![buffer.filePath isEqualToString:filePath])
        withNewFilePath = YES;
    
    if (withNewFilePath) {
        ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
        if (!ret) {
            NSLog(@"ERROR: Save source buffer failed!");
            return;
        }
        
        if (![self.session.project addSecurityBookmarkWithFilePath:filePath]) {
            NSLog(@"ERROR: Create save path bookmark failed!");
            return;
        }
        
        if ([sourceBufferManager isTemporaryFilePath:filePathBeforeSave]) {
            CEEProjectController* controller = (CEEProjectController*)[NSDocumentController sharedDocumentController];
            [controller replaceSourceBufferReferenceFilePath:filePathBeforeSave to:filePath];
        }
        else {
            [self openSourceBufferWithFilePath:filePath];
        }
    }
    else {
        ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
    }
    
    [self.session.project syncSourceSymbols:buffer];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSaveSourceBuffer object:self userInfo:@{@"sourceBuffer" : buffer}];
}

- (CEESourceBuffer*)securityOpenSourceBufferWithFilePath:(NSString*)filePath {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = nil;
    CEESecurityBookmark* bookmark = nil;
    
    // create bookmark if doesn't exist
    bookmark = [self.session.project getSecurityBookmarkWithFilePath:filePath];
    if (!bookmark) {
        if (![self.session.project addSecurityBookmarkWithFilePath:filePath]) {
#ifdef DEBUG
            NSLog(@"ERROR: Create bookmark failed!");
#endif
            goto exit;
        }
        bookmark = [self.session.project getSecurityBookmarkWithFilePath:filePath];
    }
    
    if (access([filePath UTF8String], R_OK) != 0) {
        if (bookmark) {
            [delegate startAccessingSecurityScopedResourceWithBookmark:bookmark];
        }
        else {
#ifdef DEBUG
            NSLog(@"ERROR: Access security scoped resource but bookmark is nil!");
#endif
            goto exit;
        }
    }
    
    buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath];

exit:
    return buffer;
}

- (void)setActivedSourceBuffer:(CEESourceBuffer*)buffer {
    NSInteger lineBufferOffset = 0;
    NSInteger caretBufferOffset = 0;
    NSDictionary* bufferOffsets = [self lastPresentedBufferOffsetInHistory:buffer.filePath];
    if (bufferOffsets) {
        lineBufferOffset = [bufferOffsets[@"lineBufferOffset"] integerValue];
        caretBufferOffset = [bufferOffsets[@"caretBufferOffset"] integerValue];
    }
    
    [self appendBufferReference:buffer];
    [self.currentSourceBufferReference setLineBufferOffset:lineBufferOffset];
    [self.currentSourceBufferReference setCaretBufferOffset:caretBufferOffset];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortActiveSourceBuffer object:self];
}

- (CEESourceBuffer*)activedSourceBuffer {
    CEESourceBufferReferenceContext* reference = [self currentSourceBufferReference];
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


- (NSString*)serialize {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
        
    // session port serialize start
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    
    // opened buffer serialize begin
    // ignore all Untitled Source Buffers
    NSMutableArray* openedSourceBuffers = [NSMutableArray arrayWithArray:_openedSourceBuffers];
    for (CEESourceBuffer* buffer in _openedSourceBuffers) {
        if ([buffer stateSet:kCEESourceBufferStateFileTemporary])
            [openedSourceBuffers removeObject:buffer];
    }
    
    serializing = [serializing stringByAppendingFormat:@"\"openedSourceBuffers\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < openedSourceBuffers.count; i ++) {
        CEESourceBuffer* buffer = openedSourceBuffers[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"\"filePath\":\"%@\"", buffer.filePath];
        serializing = [serializing stringByAppendingFormat:@"}"];
        
        if (i < openedSourceBuffers.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
    }
    serializing = [serializing stringByAppendingFormat:@"]"];
    // opened buffer serialize end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // references serialize begin
    // ignore all Temporary File References
    int backStep = 0;
    NSMutableArray* bufferReferences = [NSMutableArray arrayWithArray:_sourceBufferReferences];
    for (int i = 0; i < _sourceBufferReferences.count; i ++) {
        CEESourceBufferReferenceContext* reference = _sourceBufferReferences[i];
        if ([sourceBufferManager isTemporaryFilePath:reference.filePath]) {
            [bufferReferences removeObject:reference];
            if (i <= _bufferReferenceIndex)
                backStep ++;
        }
    }
    _bufferReferenceIndex -= backStep;
    if (_bufferReferenceIndex < 0)
        _bufferReferenceIndex = bufferReferences.count - 1;
    
    serializing = [serializing stringByAppendingFormat:@"\"sourceBufferReferences\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < bufferReferences.count; i ++) {
        CEESourceBufferReferenceContext* reference = bufferReferences[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"\"filePath\":\"%@\"", reference.filePath];
        serializing = [serializing stringByAppendingFormat:@","];
        serializing = [serializing stringByAppendingFormat:@"\"lineBufferOffset\":\"%lu\"", reference.lineBufferOffset];
        serializing = [serializing stringByAppendingFormat:@","];
        serializing = [serializing stringByAppendingFormat:@"\"caretBufferOffset\":\"%lu\"", reference.caretBufferOffset];
        serializing = [serializing stringByAppendingFormat:@"}"];
        
        if (i < bufferReferences.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
        
    }
    serializing = [serializing stringByAppendingFormat:@"]"];
    // references serialize end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // reference index begin
    serializing = [serializing stringByAppendingFormat:@"\"sourceBufferReferenceIndex\":\"%ld\"", _bufferReferenceIndex];
    // reference index end
    
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    NSString* identifier = [dict allKeys][0];
    NSDictionary* descriptor = dict[identifier];
    CEESourceBufferReferenceContext* reference = nil;
    
    // Recover Opened Source Buffers
    NSArray* openBufferDescriptors = descriptor[@"openedSourceBuffers"];
    for (NSDictionary* openBufferDescriptor in openBufferDescriptors) {
        NSString* filePath = openBufferDescriptor[@"filePath"];
        CEESourceBuffer* buffer = [self securityOpenSourceBufferWithFilePath:filePath];
        if (buffer)
            [_openedSourceBuffers addObject:buffer];
    }
    
    // Make sure there is always an opened source buffer existed when deserializing.
    // Cymbols open an untitled source buffer when _openedSourceBuffers is empty, then
    // manual init the _sourceBufferReferences and _bufferReferenceIndex. 
    if (!_openedSourceBuffers.count) {
        AppDelegate* delegate = [NSApp delegate];
        CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
        CEESourceBuffer* buffer = [sourceBufferManager openUntitledSourceBuffer];
        if (buffer) {
            [_openedSourceBuffers addObject:buffer];
            reference = [[CEESourceBufferReferenceContext alloc] initWithFilePath:buffer.filePath];
            if (reference) {
                [_sourceBufferReferences addObject:reference];
                _bufferReferenceIndex ++;
            }
        }
    }
    else {
        // recover Source Buffer References
        NSArray* referenceDescriptors = descriptor[@"sourceBufferReferences"];
        for (NSDictionary* referenceDescriptor in referenceDescriptors) {
            NSString* filePath = referenceDescriptor[@"filePath"];
            NSString* lineBufferOffset = referenceDescriptor[@"lineBufferOffset"];
            NSString* caretBufferOffset = referenceDescriptor[@"caretBufferOffset"];
            
            if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
                continue;
            
            reference = [[CEESourceBufferReferenceContext alloc] initWithFilePath:filePath];
            if (reference) {
                [reference setLineBufferOffset:[lineBufferOffset integerValue]];
                [reference setCaretBufferOffset:[caretBufferOffset integerValue]];
                [_sourceBufferReferences addObject:reference];
            }
        }
        
        // recover Source Buffer Reference Index
        NSString* referenceIndexDescriptor = descriptor[@"sourceBufferReferenceIndex"];
        _bufferReferenceIndex = [referenceIndexDescriptor integerValue];
        
        // Make sure the _bufferReferenceIndex is legal.
        if (_bufferReferenceIndex < 0 || _bufferReferenceIndex > _sourceBufferReferences.count - 1)
            _bufferReferenceIndex = _sourceBufferReferences.count - 1;
        
    }
    
    // recover Context
    reference = _sourceBufferReferences[_bufferReferenceIndex];
    CEESourceBuffer* target = nil;
    for (CEESourceBuffer* buffer in _openedSourceBuffers) {
        if ([buffer.filePath isEqualToString:reference.filePath]) {
            target = buffer;
            break;
        }
    }
    
    if (target) {
        const cee_uchar* subject = cee_text_storage_buffer_get(target.storage);
        CEERange range = cee_range_make(0, cee_text_storage_size_get(target.storage));
        CEEList* symbolReferences = NULL;
        cee_source_reference_parse(target.parser_ref,
                                   [target.filePath UTF8String],
                                   (const cee_char*)subject,
                                   target.source_token_map,
                                   target.prep_directive_fregment,
                                   target.statement_fregment,
                                   range,
                                   &symbolReferences);
        CEETokenCluster* cluster =
            cee_token_cluster_search_by_buffer_offset(symbolReferences,
                                                      target.prep_directive_fregment,
                                                      target.statement_fregment,
                                                      reference.caretBufferOffset);
        if (cluster) {
            _source_context = [self createSourceContext:cluster];
            cee_token_cluster_free(cluster);
        }
        cee_list_free_full(symbolReferences, cee_source_symbol_reference_free);
    }
    
    _identifier = identifier;
}

- (void)discardReferences {    
    _sourceBufferReferences = [[NSMutableArray alloc] init];
    _bufferReferenceIndex = -1;
}

- (void)dealloc {
    if (_source_context)
        cee_source_context_free(_source_context);
    _source_context = NULL;
}

- (CEESourceContext*)createSourceContext:(CEETokenCluster*)cluster {
    if (!cluster)
        return NULL;
    
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = delegate.sourceBufferManager;
    CEESourceBufferReferenceContext* bufferReference = [self currentSourceBufferReference];
    CEESourceBuffer* buffer = [sourceBufferManager openSourceBufferWithFilePath:bufferReference.filePath];
    CEESourceContext* context = NULL;
        
    if (cluster->type == kCEETokenClusterTypeReference)
        context = cee_source_reference_context_create([bufferReference.filePath UTF8String],
                                                      (CEESourceSymbolReference*)cluster->content_ref,
                                                      buffer.prep_directive_fregment,
                                                      buffer.statement_fregment,
                                                      self.session.project.database);
    else if (cluster->type == kCEETokenClusterTypeSymbol)
        context = cee_source_symbol_context_create([bufferReference.filePath UTF8String],
                                                   (CEESourceSymbol*)cluster->content_ref,
                                                   buffer.prep_directive_fregment,
                                                   buffer.statement_fregment,
                                                   self.session.project.database);
    
    [sourceBufferManager closeSourceBuffer:buffer];
    
    return context;
}

- (void)createContextByCluster:(CEETokenCluster*)cluster {
    if (_source_context)
        cee_source_context_free(_source_context);
    _source_context = [self createSourceContext:cluster];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCreateSourceContext object:self];
}

- (void)jumpToSymbolByCluster:(CEETokenCluster*)cluster {
    if (_source_context)
        cee_source_context_free(_source_context);
    _source_context = [self createSourceContext:cluster];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortCreateSourceContext object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortJumpToSymbolRequest object:self];
}

- (void)jumpToSymbol:(CEESourceSymbol*)symbol {
    _jumpPoint = [[CEESourcePoint alloc] initWithSourceSymbol:symbol];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortJumpToSourcePoint object:self];
}

- (void)jumpToReference:(CEESourceSymbolReference*)reference {
    _jumpPoint = [[CEESourcePoint alloc] initWithSourceSymbolReference:reference];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortJumpToSourcePoint object:self];
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
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSearchReference object:self];
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
        _ports = nil;
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
    return [_project getReferenceFilePathsWithCondition:condition];
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

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    _sessions = nil;
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
    if (!_sessions)
        _sessions = [[NSMutableArray alloc] init];
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
    
    if ([self isUntitled])
        [self loadDefaultWindowSetting:controller];
    [self addWindowController:controller];
    [controller showWindow:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPresent object:self];
}

- (void)loadDefaultWindowSetting:(CEEWindowController*)windowController {
    AppDelegate* delegate = [NSApp delegate];
    NSString* filePath = [delegate defaultWindowSettingPath];
    NSMutableDictionary* descriptor = [CEEJSONReader objectFromFile:filePath];
    if (descriptor &&
        descriptor[@"x"] && descriptor[@"y"] &&
        descriptor[@"width"] && descriptor[@"height"]) {
        CGFloat x = [descriptor[@"x"] floatValue];
        CGFloat y = [descriptor[@"y"] floatValue];
        CGFloat width = [descriptor[@"width"] floatValue];
        CGFloat height = [descriptor[@"height"] floatValue];
        NSRect frame = [windowController.window frame];
        frame.origin.x = x;
        frame.origin.y = y;
        frame.size.width = width;
        frame.size.height = height;
        [windowController.window setFrame:frame display:NO];
    }
}

- (void)setProperties:(CEEProjectSetting*)properties {
    AppDelegate* delegate = [NSApp delegate];
    
    if (!properties || !properties.name || !properties.path)
        return;
        
    NSString* backendName = [properties.name stringByAppendingPathExtension:@"cymd"];
    NSString* projectPath = [properties.path stringByAppendingPathComponent:properties.name];
    NSString* backendPath = [projectPath stringByAppendingPathComponent:backendName];
    
    [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[NSURL fileURLWithPath:backendPath]];
    _database = cee_database_create([backendPath UTF8String]);
    _databasePath = backendPath;
    _name = properties.name;
    
    // turn off journal mode
    if (!_database)
        return ;
    
    // set application info in database
    cee_database_application_info_set(_database, [[delegate infoString] UTF8String]);
    
    // turn off journal mode, apple's sandbox forbid the journal file created
    sqlite3_exec(_database, "PRAGMA journal_mode=MEMORY;", NULL, 0, 0);

    CEEList* file_paths = NULL;
    
    // { save referenceRoots
    for (NSString* filePath in properties.referenceRoots)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    cee_database_file_reference_roots_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    // save referenceRoots }
    
    // { save referenceFilePaths
    NSArray* referenceFilePaths = ExpandFilePaths(properties.referenceRoots);
    for (NSString* filePath in referenceFilePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    cee_database_file_references_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    // save referencedFilePaths }
    
    
    // { add referenceRoots bookmarks then enable accessing
    for (NSString* filePath in properties.referenceRoots) {
        CEESecurityBookmark* bookmark = [self addSecurityBookmarkWithFilePath:filePath];
        [delegate startAccessingSecurityScopedResourceWithBookmark:bookmark];
    }
    // add referenceRoots bookmarks then enable accessing }
    
    for (CEESession* session in self.sessions) {
        for (CEESessionPort* port in session.ports) {
            [port closeAllSourceBuffers];
            [port discardReferences];
        }
    }
    
    if (referenceFilePaths) {
        for (CEESession* session in self.sessions)
            [session.activedPort openSourceBufferWithFilePath:referenceFilePaths[0]];
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectSettingProperties object:self];
}

- (void)serialize {
    cee_database_session_descriptors_remove(_database);
    for (CEESessionWindowController* controller in self.windowControllers) {
        CEESession* session = controller.session;
        NSString* string = [session serialize];
        cee_database_session_descriptor_append(_database, [string UTF8String]);
    }
}

- (void)deserialize:(NSString*)filePath {
    CEEList* descriptors = NULL;
    CEEList* p = NULL;
    cee_char* descriptor_string = NULL;
    NSDictionary* descriptor = NULL;
    CEESession* session = NULL;
    
    _database = cee_database_open([filePath UTF8String]);
    if (!_database)
        return;
    
    // turn off journal mode, apple's sandbox forbid the journal file created
    sqlite3_exec(_database, "PRAGMA journal_mode=MEMORY;", NULL, 0, 0);
    
    [self validateDatabase];
    
    // clean the init sessions
    [self deleteAllSessions];
    
    NSString* regularFormat = [filePath stringByDeletingLastPathComponent];
    _name = [[regularFormat lastPathComponent] stringByDeletingPathExtension];
        
    [self startAccessFilePathsReferenced];
    
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

- (void)validateDatabase {
    AppDelegate* delegate = [NSApp delegate];
    // get application info in database
    cee_char* info_string = cee_database_application_info_get(_database);
    if (!info_string)
        [self createCurrentVersionDatabaseContent];
    
    NSString* infoStringLogged = [NSString stringWithUTF8String:info_string];
    NSString* infoStringCurrent = [delegate infoString];
    
    infoStringLogged = [infoStringLogged stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    infoStringCurrent = [infoStringCurrent stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    
    if ([infoStringLogged compare:infoStringCurrent options:NSCaseInsensitiveSearch] != NSOrderedSame)
        [self createCurrentVersionDatabaseContent];
    
    cee_free(info_string);
}

- (void)createSessionWithFilePaths:(NSArray*)filePaths {
    CEESession* session = [self createSession];
    [session.project setCurrentSession:session];
    
    for (NSString* filePath in filePaths)
        [session.activedPort openSourceBufferWithFilePath:filePath];
    
    [session createDescriptor:open_file_descriptor_template];
    [self makeWindowControllerFromSession:session];
}

- (void)deleteAllSessions {
    for (CEESession* session in _sessions)
        [session deleteAllPorts];
    
    _sessions = nil;
    self.currentSession = nil;
}

- (NSArray*)getReferenceRoots {
    if (!_database)
        return nil;
    
    NSMutableArray* filePaths = nil;
    CEEList* file_paths = cee_database_file_reference_roots_get(_database);
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

- (NSArray*)getReferenceFilePathsWithCondition:(nullable NSString*)condition {
    if (!_database)
        return nil;
    
    NSMutableArray* filePaths = nil;
    condition = [condition stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    if ([condition isEqualToString:@"*"] || 
        [condition isEqualToString:@""])
        condition = nil;
        
    CEEList* file_paths = cee_database_file_reference_paths_get(_database, [condition UTF8String]);
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

- (void)addReferenceRoot:(NSString*)filePath {
    AppDelegate* delegate = [NSApp delegate];
    
    CEEList* file_paths = NULL;
    
    // { save roots
    file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    cee_database_file_reference_roots_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    // save roots }
    
    NSArray* filePaths = ExpandFilePaths(@[filePath]);
    for (NSString* filePath in filePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    cee_database_file_references_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    // save referencedFilePaths }
        
    // { add roots bookmarks then enable accessing
    CEESecurityBookmark* bookmark = [self addSecurityBookmarkWithFilePath:filePath];
    [delegate startAccessingSecurityScopedResourceWithBookmark:bookmark];
    // save roots bookmarks then enable accessing }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectAddReference object:self];
}

- (void)addReferenceRoots:(NSArray*)roots {
    for (NSString* root in roots)
        [self addReferenceRoot:root];
}

- (void)removeReferenceRoot:(NSString*)filePath {    
    CEEList* file_paths = NULL;
    
    // { remove referenceFilePaths
    NSArray* filePaths = ExpandFilePaths(@[filePath]);
    for (NSString* filePath in filePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    cee_database_file_references_remove(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    // remove referencedFilePaths }
    
    // { remove roots
    file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    cee_database_file_reference_roots_remove(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    // remove roots }
    
    [self removeSecurityBookmarkWithFilePath:filePath];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectRemoveReference object:self];
}

- (void)removeReferenceRoots:(NSArray*)roots {
    for (NSString* root in roots)
        [self removeReferenceRoot:root];
}

- (void)addReference:(NSString*)filePath {
    cee_database_file_reference_append(_database, [filePath UTF8String]);
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectAddReference object:self];
}

- (void)addReferences:(NSArray*)filePaths {
    CEEList* file_paths = NULL;
    NSArray* expandedFilePaths = ExpandFilePaths(filePaths);
    for (NSString* filePath in expandedFilePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    cee_database_file_references_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectAddReference object:self];
}

- (void)removeReference:(NSString*)filePath {
    cee_database_file_reference_remove(_database, [filePath UTF8String]);
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectRemoveReference object:self];
}

- (void)removeReferences:(NSArray*)filePaths {
    CEEList* file_paths = NULL;
    NSArray* expandedFilePaths = ExpandFilePaths(filePaths);
    for (NSString* filePath in expandedFilePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    file_paths = cee_list_reverse(file_paths);
    cee_database_file_references_remove(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    file_paths = NULL;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectRemoveReference object:self];
}

- (BOOL)filePathIsReferenced:(NSString*)filePath {
    return cee_database_file_reference_is_existed(_database, [filePath UTF8String]);
}

- (BOOL)filePathIsReferenceRoot:(NSString*)filePath {
    return cee_database_file_reference_root_is_existed(_database, [filePath UTF8String]);
}

- (void)createCurrentVersionDatabaseContent {
    // TODO: create current version database content
}

- (void)startAccessFilePathsReferenced {
    AppDelegate* delegate = [NSApp delegate];
    NSArray* filePaths = [self getReferenceRoots];
    for (NSString* filePath in filePaths) {
        CEESecurityBookmark* bookmark = [self getSecurityBookmarkWithFilePath:filePath];
        [delegate startAccessingSecurityScopedResourceWithBookmark:bookmark];
    }
}

- (void)syncSourceSymbols:(CEESourceBuffer*)buffer {
    if (!buffer)
        return;
    
    CEEList* list = NULL;
    cee_int symbols_count = cee_database_file_reference_symbols_count_get(_database,
                                                                          [buffer.filePath UTF8String]);
    if (symbols_count)
        cee_database_symbols_delete_by_file_path(_database, [buffer.filePath UTF8String]);
    
    cee_source_fregment_symbol_tree_dump_to_list(buffer.prep_directive_symbol_tree, &list);
    cee_source_fregment_symbol_tree_dump_to_list(buffer.statement_symbol_tree, &list);
    if (list) {
        // update last parsed time to buffer
        cee_char* time_str = cee_time_to_iso8601(cee_time_current());
        if (time_str) {
            cee_database_file_reference_last_parsed_time_set(_database,
                                                             [buffer.filePath UTF8String],
                                                             time_str);
            cee_free(time_str);
        }       
        cee_database_symbols_write(_database, list);
        cee_database_file_reference_symbols_count_set(_database,
                                                      [buffer.filePath UTF8String],
                                                      cee_list_length(list));
        cee_list_free(list);
    }
}

- (void)close {
    cee_database_close(_database);
    _database = NULL;
    [super close];
}

- (BOOL)isUntitled {
    return self.database == NULL;
}

- (BOOL)createFile:(NSString*)filePath {
    BOOL ret = [[NSFileManager defaultManager] createFileAtPath:filePath contents:nil attributes:nil];
    if (ret)
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectCreateFile object:self];
    return ret;
}

- (BOOL)createDirectory:(NSString*)path {
    BOOL ret = [[NSFileManager defaultManager] createDirectoryAtPath:path withIntermediateDirectories:NO attributes:nil error:nil];
    if (ret)
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectCreateFile object:self];
    return ret;
}


- (BOOL)removeFile:(NSString*)filePath {
    NSURL* URL = [NSURL fileURLWithPath:filePath];
    NSError* error;
    NSArray* expandedFilePaths = ExpandFilePaths(@[filePath]);
    BOOL ret = [[NSFileManager defaultManager] trashItemAtURL:URL resultingItemURL:nil error:&error];
    if (!ret) {
#ifdef DEBUG
        NSLog(@"ERROR: remove file path %@ failed!", filePath);
#endif
    }
    if (ret) {
        if (cee_database_file_reference_root_is_existed(_database, [filePath UTF8String]))
            cee_database_file_reference_root_remove(_database, [filePath UTF8String]);
        
        if (expandedFilePaths)
            [self removeReferences:expandedFilePaths];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectRemoveFile object:self];
        
        AppDelegate* delegate = [NSApp delegate];
        CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
        [sourceBufferManager syncSourceBuffersFromFiles];
        
    }
    return ret;
}

- (BOOL)removeFiles:(NSArray*)filePaths {
    BOOL ret = FALSE;
    for (NSString* filePath in filePaths) {
        NSArray* expandedFilePaths = ExpandFilePaths(@[filePath]);
        NSURL* URL = [NSURL fileURLWithPath:filePath];
        NSError* error;
        ret = [[NSFileManager defaultManager] trashItemAtURL:URL resultingItemURL:nil error:&error];
        if (ret) {
            if (cee_database_file_reference_root_is_existed(_database, [filePath UTF8String]))
                cee_database_file_reference_root_remove(_database, [filePath UTF8String]);
            
            if (expandedFilePaths)
                [self removeReferences:expandedFilePaths];
        }
        else {
#ifdef DEBUG
            NSLog(@"ERROR: remove file path %@ failed!", filePath);
#endif
            break;
        }
    }
    
    if (ret) {
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectRemoveFile object:self];
                
        AppDelegate* delegate = [NSApp delegate];
        CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
        [sourceBufferManager syncSourceBuffersFromFiles];
    }
    return ret;
}


- (CEESecurityBookmark*)addSecurityBookmarkWithFilePath:(NSString*)filePath {
    CEESecurityBookmark* bookmark = CreateBookmarkWithFilePath(filePath);
    if (!bookmark) {
#ifdef DEBUG
        NSLog(@"ERROR: add security bookmark for %@ failed!", filePath);
#endif
        return nil;
    }
    
    cee_database_security_bookmark_append(_database,
                                          [bookmark.filePath UTF8String],
                                          [bookmark.content UTF8String]);
    
#ifdef DEBUG
        NSLog(@"INFO: add security bookmark for %@ successed!", filePath);
#endif
    return bookmark;
}

- (void)removeSecurityBookmarkWithFilePath:(NSString*)filePath {
    cee_boolean ret = cee_database_security_bookmark_remove(_database,
                                                            [filePath UTF8String]);
#ifdef DEBUG
    if (!ret)
        NSLog(@"ERROR: remove security bookmark for %@ failed!", filePath);
    else
        NSLog(@"INFO: remove security bookmark for %@ successed!", filePath);
#endif
}

- (CEESecurityBookmark*)getSecurityBookmarkWithFilePath:(NSString*)filePath {
    cee_char* content = cee_database_security_bookmark_content_get(_database,
                                                                   [filePath UTF8String]);
    if (!content)
        return nil;
    
    CEESecurityBookmark* bookmark = [[CEESecurityBookmark alloc] init];
    bookmark.filePath = filePath;
    bookmark.content = [NSString stringWithUTF8String:content];
    cee_free(content);
    return bookmark;
}

- (NSString*)shortFilePath:(NSString*)filePath {
    
    NSString* shortFilePath = filePath;
    CEEList* roots = cee_database_file_reference_roots_get(_database);
    CEEList* p = roots;
    
    while (p) {
        BOOL isDirectory = NO;
        NSString* root = [NSString stringWithUTF8String:p->data];
        
        if (![[NSFileManager defaultManager] fileExistsAtPath:root isDirectory:&isDirectory])
            goto next;
                
        if (!isDirectory && [filePath compare:root options:NSCaseInsensitiveSearch] == NSOrderedSame) {
            shortFilePath = [filePath lastPathComponent];
            break;
        }
        
        if (isDirectory && filePath.length > root.length) {
            NSString* prefix = [filePath substringWithRange:NSMakeRange(0, root.length)];
            if ([prefix compare:root options:NSCaseInsensitiveSearch] == NSOrderedSame) {
                NSInteger location = prefix.length;
                NSUInteger length = filePath.length - prefix.length;
                shortFilePath = [NSString stringWithFormat:@"%@%@", [root lastPathComponent], [filePath substringWithRange:NSMakeRange(location, length)]];
                break;
            }
        }
next:
        p = p->next;
    }
    
    return shortFilePath;
}

@end

@implementation CEEProjectController

- (CEEProject*)createProjectFromSetting:(CEEProjectSetting*)setting {
    CEEProject* project = [[CEEProject alloc] init];
    [self addDocument:project];
    [project setProperties:setting];
    [project makeWindowControllers];
    if (project.databasePath)
        [self noteNewRecentDocumentURL:[NSURL fileURLWithPath:project.databasePath]];
    return project;
}

- (CEEProject*)openProjectFromURL:(NSURL*)url {
    CEEProject* project = [[CEEProject alloc] init];
    [self addDocument:project];
    [project deserialize:[url path]];
    [project makeWindowControllers];
    [self noteNewRecentDocumentURL:url];
    
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
        if ([project.databasePath isEqualToString:[url path]])
            return project;
    }
    return nil;
}

- (void)replaceSourceBufferReferenceFilePath:(NSString*)filePath0 to:(NSString*)filePath1 {
    for (CEEProject* project in self.documents) {
        for (CEESession* session in project.sessions) {
            for (CEESessionPort* port in session.ports) {
                for (CEESourceBufferReferenceContext* reference in port.sourceBufferReferences) {
                    if ([reference.filePath isEqualToString:filePath0])
                        reference.filePath = filePath1;
                }
            }
        }
    }
}

@end
