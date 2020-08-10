//
//  CEEProject.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEEProject.h"
#import "CEESessionWindowController.h"
#import "CEESessionViewController.h"
#import "CEEIdentifier.h"
#import "cee_symbol.h"
#import "cee_backend.h"

extern char* init_descriptor_template;
extern char* open_file_descriptor_template;

NSNotificationName CEENotificationProjectSettingProperties = @"CEENotificationProjectSettingProperties";
NSNotificationName CEENotificationProjectAddFilePaths = @"CEENotificationProjectAddFilePaths";
NSNotificationName CEENotificationProjectRemoveFilePaths = @"CEENotificationProjectRemoveFilePaths";
NSNotificationName CEENotificationSessionPortOpenSourceBuffer = @"CEENotificationSessionPortOpenSourceBuffer";
NSNotificationName CEENotificationSessionPortSaveSourceBuffer = @"CEENotificationSessionPortSaveSourceBuffer";
NSNotificationName CEENotificationSessionPortCloseSourceBuffer = @"CEENotificationSessionPortCloseSourceBuffer";
NSNotificationName CEENotificationSessionPortPresentSourceBuffer = @"CEENotificationSessionPortPresentSourceBuffer";
NSNotificationName CEENotificationSessionPresent = @"CEENotificationSessionPresent";
NSNotificationName CEENotificationSessionCreatePort = @"CEENotificationSessionCreatePort";
NSNotificationName CEENotificationSessionActivePort = @"CEENotificationSessionActivePort";
NSNotificationName CEENotificationSessionDeletePort = @"CEENotificationSessionDeletePort";

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

@implementation CEEBufferReference

- (instancetype)initWithSourceBuffer:(CEESourceBuffer*)buffer {
    self = [super init];
    if (!self)
        return nil;
    
    _buffer = buffer;
    _paragraphIndex = 0;
    
    return self;
}

@end


@implementation CEEProjectSetting
+ (CEEProjectSetting*)projectSettingWithName:(NSString*)name path:(NSString*)path filePaths:(NSArray*)filePaths {
    CEEProjectSetting* setting = [[CEEProjectSetting alloc] init];
    if (setting) {
        setting.name = name;
        setting.path = path;
        setting.filePaths = filePaths;
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

@implementation CEESessionPort

- (instancetype)init {
    self = [super init];
    if (self) {
        _references = [[NSMutableArray alloc] init];
        _referenceIndex = -1;
        _identifier = CreateUnifiedIdentifierWithPrefix([self className]);
    }
    return self;
}

- (void)appendReference:(CEESourceBuffer*)buffer {
    CEEBufferReference* current = [self currentReference];
    if (current) {
        if (current.buffer == buffer)
            return;
    }
    
    CEEBufferReference* refernce = [[CEEBufferReference alloc] initWithSourceBuffer:buffer];
    if (!_references.count) {
        [_references addObject:refernce];
    }
    else {
        if (_referenceIndex < _references.count - 1) {
            NSRange range = NSMakeRange(_referenceIndex + 1, _references.count - (_referenceIndex + 1));
            [_references removeObjectsInRange:range];
        }
        
        [_references addObject:refernce];
    }
    
    _referenceIndex ++;
}

- (void)nextReference {
    if (_referenceIndex < _references.count - 1) {
        _referenceIndex ++;
        CEEBufferReference* reference = _references[_referenceIndex];
        [self presentSourceBuffer:reference.buffer];
    }
}

- (void)prevReference {
    if (_referenceIndex > 0) {
        _referenceIndex --;
        CEEBufferReference* reference = _references[_referenceIndex];
        [self presentSourceBuffer:reference.buffer];
    }
}

- (CEEBufferReference*)currentReference {
    if (_referenceIndex < 0 || _referenceIndex >= _references.count)
        return nil;
    
    return _references[_referenceIndex];
}

- (void)openSourceBufferWithFilePath:(NSString*)filePath {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath];
    if (buffer) {
        [self appendReference:buffer];
        [_session registerSourceBuffer:buffer];
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
    }
}

- (void)openSourceBuffersWithFilePaths:(NSArray*)filePaths {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    for (NSString* filePath in filePaths) {
        CEESourceBuffer* buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath];
        if (buffer) {
            [self appendReference:buffer];
            [_session registerSourceBuffer:buffer];
        }
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
}


- (void)openUntitledSourceBuffer {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = [sourceBufferManager openUntitledSourceBuffer];
    if (buffer) {
        [self appendReference:buffer];
        [_session registerSourceBuffer:buffer];
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortOpenSourceBuffer object:self];
    }
}

- (void)presentSourceBuffer:(CEESourceBuffer*)buffer {
    [self appendReference:buffer];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortPresentSourceBuffer object:self];
}

- (void)presentHistory:(CEEBufferReference*)reference {
    for (NSInteger i = 0; i < _references.count; i ++) {
        if (reference == _references[i])
            _referenceIndex = i;
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortPresentSourceBuffer object:self];
}

- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filePath {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    [sourceBufferManager saveSourceBuffer:buffer atPath:filePath];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPortSaveSourceBuffer object:self];
}

- (NSString*)serialize {
    CEEBufferReference* reference = nil;
    // session port serialize start
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    
    // reference index begin
    serializing = [serializing stringByAppendingFormat:@"\"referenceIndex\":\"%ld\"", _referenceIndex];
    // reference index end
    
    serializing = [serializing stringByAppendingFormat:@","];
    
    // references begin
    serializing = [serializing stringByAppendingFormat:@"\"references\":"];
    serializing = [serializing stringByAppendingFormat:@"["];
    for (int i = 0; i < _references.count; i ++) {
        reference = _references[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        serializing = [serializing stringByAppendingFormat:@"\"filePath\":\"%@\"", reference.buffer.filePath];
        serializing = [serializing stringByAppendingFormat:@","];
        serializing = [serializing stringByAppendingFormat:@"\"pragraphIndex\":\"%lu\"", reference.paragraphIndex];
        serializing = [serializing stringByAppendingFormat:@"}"];
        
        if (i < _references.count - 1)
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
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = nil;
    CEEBufferReference* reference = nil;
    
    NSArray* referenceDescriptors = descriptor[@"references"];
    for (NSDictionary* referenceDescriptor in referenceDescriptors) {
        NSString* filePath = referenceDescriptor[@"filePath"];
        NSString* paragraphIndex = referenceDescriptor[@"pragraphIndex"];
        buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath];
        if (buffer) {
            reference = [[CEEBufferReference alloc] initWithSourceBuffer:buffer];
            [reference setParagraphIndex:[paragraphIndex integerValue]];
            [_references addObject:reference];
            [_session registerSourceBuffer:buffer];
        }
    }
    
    if (_references.count) {
        NSString* referenceIndexDescriptor = descriptor[@"referenceIndex"];
        _referenceIndex = [referenceIndexDescriptor integerValue];
        if (_referenceIndex > _references.count - 1)
            _referenceIndex = _references.count - 1;        
    }
    
    _identifier = identifier;
}

- (void)discardSourceBuffers {
    _references = [[NSMutableArray alloc] init];
    _referenceIndex = -1;
}

@end

@implementation CEESession

@synthesize activedPort = _activedPort;

- (instancetype)init {
    self = [super init];
    if (self) {
        _ports = [[NSMutableArray alloc] init];
        _identifier = CreateUnifiedIdentifierWithPrefix([self className]);
        [self setActivedPort:[self createPort]];
        [self createDescriptor:init_descriptor_template];
    }
    return self;
}

- (CEESessionPort*)createPort {
    CEESessionPort* port = [[CEESessionPort alloc] init];
    if (!port)
        return nil;
    
    [port setSession:self];
    [_ports addObject:port];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionCreatePort object:self];
    return port;
}

- (void)deletePort:(CEESessionPort*)port {
    [_ports removeObject:port];
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

- (void)registerSourceBuffer:(CEESourceBuffer*)buffer {
    if (!_registeredSourceBuffers)
        _registeredSourceBuffers = [[NSMutableArray alloc] init];
    
    for (CEESourceBuffer* registeredBuffer in _registeredSourceBuffers) {
        if (registeredBuffer == buffer)
            return;
    }
    
    [_registeredSourceBuffers addObject:buffer];
}

- (NSString*)serialize {
    // session serialize start
    NSString* serializing = @"{";
    
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
    serializing = [serializing stringByAppendingFormat:@"\"activePort\":\"%@\"", _activedPort.identifier];
    // active port serialize end
        
    serializing = [serializing stringByAppendingFormat:@"}"];
    
    NSLog(@"%@", serializing);
    
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    _descriptor = dict;
    
    [_ports removeAllObjects];
    NSArray* portDescriptors = _descriptor[@"ports"];
    for (NSDictionary* portDescriptor in portDescriptors) {
        CEESessionPort* port = [[CEESessionPort alloc] init];
        if (port) {
            [port setSession:self];
            [_ports addObject:port];
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
    if (self) {
        _sessions = [[NSMutableArray alloc] init];
        // create an init session
        CEESession* session = [self createSession];
        [session.project setCurrentSession:session];
        [session.activedPort openUntitledSourceBuffer];
    }
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
    [controller setIdentifier:CreateUnifiedIdentifierWithPrefix([controller className])];
    [controller setSession:session];
    [controller deserialize:session.descriptor[@"ui"][@"window"]];
    [self addWindowController:controller];
    [controller showWindow:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationSessionPresent object:self];
}

- (void)createSessionWithFilePaths:(NSArray*)filePaths {
    CEESession* session = [self createSession];
    [session.project setCurrentSession:session];
    for (NSString* filePath in filePaths)
        [session.activedPort openSourceBufferWithFilePath:filePath];
    [session createDescriptor:open_file_descriptor_template];
    [self makeWindowControllerFromSession:session];
}

- (void)deleteSessions {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    for (NSWindowController* controller in self.windowControllers) {
        [controller close];
        [self removeWindowController:controller];
    }
    
    for (CEESession* session in _sessions) {
        for (CEESourceBuffer* buffer in session.registeredSourceBuffers)
            [sourceBufferManager closeSourceBuffer:buffer];
    }
    
    _sessions = [[NSMutableArray alloc] init];
    self.currentSession = nil;
}

- (void)setProperties:(CEEProjectSetting*)properties {
    if (!properties || !properties.name || !properties.filePaths)
        return;
    
    _properties = properties;
    
    NSString* backendName = [properties.name stringByAppendingPathExtension:@"cymd"];
    NSString* projectPath = [properties.path stringByAppendingPathComponent:properties.name];
    NSString* backendPath = [projectPath stringByAppendingPathComponent:backendName];
    
    [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[NSURL fileURLWithPath:backendPath]];
    _database = cee_database_create([backendPath UTF8String]);
    
    NSArray* filePaths = properties.filePaths;
    CEEList* file_paths = NULL;
    for (NSString* filePath in filePaths)
        file_paths = cee_list_prepend(file_paths, cee_strdup([filePath UTF8String]));
    cee_database_filepaths_append(_database, file_paths);
    cee_list_free_full(file_paths, cee_free);
    
    if (filePaths) {
        for (CEESession* session in self.sessions) {
            for (CEESessionPort* port in session.ports)
                [port discardSourceBuffers];
            
            [session.activedPort openSourceBufferWithFilePath:filePaths[0]];
        }
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationProjectSettingProperties object:self];
}

- (CEEProjectSetting*)properties {
    return _properties;
}

- (NSArray*)getFilePathsWithCondition:(nullable NSString*)condition {
    if (!_database)
        return nil;
    
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    condition = [condition stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    CEEList* file_paths = cee_database_filepaths_get(_database, [condition UTF8String]);
    CEEList* p = NULL;
    p = file_paths;
    while (p) {
        cee_char* str = p->data;
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
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    NSMutableArray* buffers = [[NSMutableArray alloc] init];
    CEEProjectSetting* setting = nil;
    
    if (!self.database) {
        
        for (CEESession* session in _sessions)
            [buffers addObjectsFromArray:[session registeredSourceBuffers]];
        
        for (CEESourceBuffer* buffer in buffers)
            if (!(buffer.states & kCEESourceBufferStateFileUntitled))
                [filePaths addObject:buffer.filePath];
    }
    
    setting = [CEEProjectSetting projectSettingWithName:@"NewProject" path:NSHomeDirectoryForUser(NSUserName()) filePaths:filePaths];
    
    return setting;
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
    
    // clean the init sessions
    [self deleteSessions];
    
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
    
    _properties = [CEEProjectSetting projectSettingWithName:[[filePath lastPathComponent] stringByDeletingPathExtension] path:[filePath stringByDeletingLastPathComponent] filePaths:[self getFilePathsWithCondition:nil]];
}

- (void)serialize {
    cee_database_session_descriptors_remove(_database);
    for (CEESessionWindowController* controller in self.windowControllers) {
        CEESession* session = controller.session;
        NSString* string = [session serialize];
        cee_database_session_descriptor_append(_database, [string UTF8String]);
    }
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

@end
