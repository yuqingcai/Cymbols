//
//  AppDelegate.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEEJSONReader.h"
#import "cee_lib.h"
#import "cee_source_parsers.h"
#import "CEETimerFreezer.h"

NSNotificationName CEENotificationHeartBeat = @"CEENotificationHeartBeat";
NSNotificationName CEENotificationApplicationConfigurationChanged = @"CEENotificationApplicationConfigurationChanged";

NSString* CEEWindowSettingFilePathIndexer = @"CEEWindowSettingFilePathIndexer";
NSString* CEEProjectSettingFilePathIndexer = @"CEEProjectSettingFilePathIndexer";
NSString* CEEConfigurationTemplateFilePathIndexer = @"CEEConfigurationTemplateFilePathIndexer";
NSString* CEEConfigurationFilePathIndexer = @"CEEConfigurationFilePathIndexer";
NSString* CEEWelcomeGuideFilePathIndexer = @"CEEWelcomeGuideFilePathIndexer";
NSString* CEETemporaryDirectoryIndexer = @"CEETemporaryDirectoryIndexer";
NSString* CEEUpdateInfoFilePathIndexer = @"CEEUpdateInfoFilePathIndexer";
NSString* CEEApplicationInfoStringIndexer = @"CEEApplicationInfoStringIndexer";
NSString* CEEApplicationVersionIndexer = @"CEEApplicationVersionIndexer";
NSString* CEEBundleVersionIndexer = @"CEEBundleVersionIndexer";
NSString* CEESerializerVersionIndexer = @"CEESerializerVersionIndexer";
NSString* CEEBackwardCompatibleMaxVersionIndexer = @"CEEBackwardCompatibleMaxVersionIndexer";
NSString* CEEStylesDirectoryIndexer = @"CEEStylesDirectoryIndexer";
NSString* CEEApplicationVersionTagFilePathIndexer = @"CEEApplicationVersionTagFilePathIndexer";
NSString* CEEApplicationConfigurationNameLineWrap = @"line_wrap";
NSString* CEEApplicationConfigurationNameCaretBlinkTimeInterval = @"caret_blink_time_interval";
NSString* CEEApplicationConfigurationNameShowLineNumber = @"show_line_number";
NSString* CEEApplicationConfigurationNameUIStyle = @"ui_style";
NSString* CEEApplicationConfigurationNameTextHighlightStyle = @"text_highlight_style";

@interface AppDelegate()
@property (strong) NSTimer* heartBeatTimer;
@property (strong) NSMutableArray* securityBookmarks;
@property (strong) CEETimerFreezer* timeFreezer;
@property (strong) NSString* supportDirectory;
@end

@implementation AppDelegate

@synthesize currentProject = _currentProject;

- (id)init {
    self = [super init];
    if (!self)
        return nil;
        
    cee_parsers_create();
    
    [self setupSupportDirectory];
    [self setupConfiguration];
    [self createHeartBeatTimer];
    [self createNetwork];
    [self createStyleManager];
    [self createSourceBufferManager];
    [self createProjectController];
    
#ifdef TRIAL_VERSION
    [self createTimeFreezer];
#endif
    
    return self;
}

- (void)dealloc {
    
    cee_parsers_free();
    
    [self deleteHeartBeatTimer];
}

- (void)createStyleManager {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [styleManager setDirectory:[self propertyIndex:CEEStylesDirectoryIndexer]];
    [styleManager setUserInterfaceStyleName:_configuration[CEEApplicationConfigurationNameUIStyle]];
    [styleManager setTextHighlightStyleName:_configuration[CEEApplicationConfigurationNameTextHighlightStyle]];
}

- (void)createSourceBufferManager {
    _sourceBufferManager = [[CEESourceBufferManager alloc] init];
    [_sourceBufferManager setTemporaryDirectory:[self propertyIndex:CEETemporaryDirectoryIndexer]];
}

- (void)createProjectController {
    _projectController = [[CEEProjectController alloc] init];
}

- (void)createHeartBeatTimer {
    _heartBeatTimer = [NSTimer timerWithTimeInterval:CEE_APP_HEART_BEAT_INTERVAL target:self selector:@selector(heartBeat:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_heartBeatTimer forMode:NSRunLoopCommonModes];
}

- (void)deleteHeartBeatTimer {
    if (_heartBeatTimer) {
        [_heartBeatTimer invalidate];
        _heartBeatTimer = nil;
    }
}

- (void)heartBeat:(NSTimer *)timer {
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationHeartBeat object:self];
}

- (void)setupSupportDirectory {
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSArray* searchPaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    _supportDirectory = [searchPaths firstObject];
    
    if (![fileManager fileExistsAtPath:_supportDirectory isDirectory:nil]) {
        [fileManager createDirectoryAtPath:_supportDirectory withIntermediateDirectories:YES attributes:nil error:nil];
        [self createApplicationVersionTagFile];
    }
    else {
        NSString* versionTagFilePath = [self propertyIndex:CEEApplicationVersionTagFilePathIndexer];
        if (![fileManager fileExistsAtPath:versionTagFilePath]) {
            [self emptySupportDirectory];
            [self createApplicationVersionTagFile];
        }
        else {
            NSString* tagInFile = [NSString stringWithContentsOfFile:versionTagFilePath encoding:NSUTF8StringEncoding error:nil];
            NSString* versionString = [self propertyIndex:CEEApplicationVersionIndexer];
            if ([tagInFile compare:versionString options:NSCaseInsensitiveSearch] != NSOrderedSame) {
                [self emptySupportDirectory];
                [self createApplicationVersionTagFile];
            }
        }
    }
    
    NSArray* descriptors = @[
        @{@"filePath":  @"Configuration.tpl",   @"override": @(YES)},   // configure template
        @{@"filePath":  @"Cymbols.cfg",         @"override": @(NO)},    // configure file
        //@{@"filePath":  @"WelcomeGuide",        @"override": @(YES)},   // welcome guide
    ];
    
    for (NSDictionary* descriptor in descriptors) {
        NSString* fileInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:descriptor[@"filePath"]];
        NSString* copyFilePath = [_supportDirectory stringByAppendingPathComponent:descriptor[@"filePath"]];
        BOOL override = [descriptor[@"override"] boolValue];
        if (override && [[NSFileManager defaultManager] fileExistsAtPath:copyFilePath isDirectory:nil])
            [[NSFileManager defaultManager] removeItemAtPath:copyFilePath error:nil];
    
        if (![[NSFileManager defaultManager] fileExistsAtPath:copyFilePath])
            [[NSFileManager defaultManager] copyItemAtPath:fileInBundle toPath:copyFilePath error:nil];
    }
}

- (void)emptySupportDirectory {
    if (!_supportDirectory)
        return;
    
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSError* error = nil;
    NSArray* files = [fileManager contentsOfDirectoryAtPath:_supportDirectory error:&error];
    if(error)
        return;

    for(NSString* file in files)
        [fileManager removeItemAtPath:[_supportDirectory stringByAppendingPathComponent:file] error:&error];
}

- (void)createApplicationVersionTagFile {
    NSString* versionTagFilePath = [self propertyIndex:CEEApplicationVersionTagFilePathIndexer];
    NSString* versionTag = [self propertyIndex:CEEApplicationVersionIndexer];
    [versionTag writeToFile:versionTagFilePath atomically:YES encoding:NSUTF8StringEncoding error:nil];
}

- (void)createNetwork {
    _network = [[CEENetwork alloc] init];
}

- (void)createTimeFreezer {
    _timeFreezer = [[CEETimerFreezer alloc] init];
}

- (void)setupConfiguration {
    NSString* filePath = [self propertyIndex:CEEConfigurationFilePathIndexer];
    _configuration = [CEEJSONReader objectFromFile:filePath];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSConstraintBasedLayoutVisualizeMutuallyExclusiveConstraints"];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    NSDocumentController* documentController = [NSDocumentController sharedDocumentController];
    for (CEEProject* project in documentController.documents) {
        
        if ([project isUntitled])
            [self saveWindowSettingAsDefault:[project.windowControllers lastObject]];
            
        [project serialize];
        [project deleteAllSessions];
    }
    [_sourceBufferManager discardTemporaryFiles];
    [self stopAccessingSecurityScopedResourceWithLoggedBookmarks];
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender {
    return YES;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

- (NSObject*)nibObjectWithClass:(Class)class fromNibNamed:(NSString*)name {
    NSArray *objects = [[NSArray alloc] init];    
    if ([[NSBundle mainBundle] loadNibNamed:name owner:self topLevelObjects:&objects]) {
        for (NSObject* object in objects) {
            if ([object isKindOfClass:class])
                return object;
        }
    }
    return nil;
}

- (NSObject*)nibObjectWithIdentifier:(NSString*)identifier fromNibNamed:(NSString*)name {
    NSArray *objects = [[NSArray alloc] init];    
    if ([[NSBundle mainBundle] loadNibNamed:name owner:self topLevelObjects:&objects]) {
        for (NSObject* object in objects) {
            if ([object conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
                NSString* _identifier = [((id<NSUserInterfaceItemIdentification>)object) identifier];
                if ([_identifier isEqualToString:identifier]) 
                    return object;
            }
        }
    }
    return nil;
}

- (void)application:(NSApplication *)application openURLs:(NSArray<NSURL *> *)urls {
    CEEProject* project = nil;
    CEESession* session = nil;
    
    if (!_projectController.currentDocument) {
        project = [[CEEProject alloc] init];
        [_projectController addDocument:project];
        [project makeWindowControllers];
        session = project.currentSession;
        for (CEESessionPort* port in session.ports) {
            [port closeAllSourceBuffers];
            [port discardReferences];
        }
    }
    else {
        project = _projectController.currentDocument;
        session = project.currentSession;
    }
    
    for (NSURL* url in urls) {
        [project.currentSession.activedPort openSourceBufferWithFilePath:[url path]];
        [_projectController noteNewRecentDocumentURL:url];
    }
}

- (void)applicationWillBecomeActive:(NSNotification *)notification {
    [_sourceBufferManager syncSourceBuffersFromFiles];
}

- (CEEProject*)currentProject {
    return _currentProject;
}

- (void)setCurrentProject:(CEEProject *)currentProject {
    _currentProject = currentProject;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    for (CEEProject* project in _projectController.documents) {
        for (NSWindowController* controller in project.windowControllers) {
            if (![controller.window.delegate windowShouldClose:controller.window])
                return NSTerminateCancel;
        }
    }
    return NSTerminateNow;
}

- (void)setConfigurationForKey:(NSString*)key value:(NSString*)value {
    [_configuration setValue:value forKey:key];
    NSString* filePath = [self propertyIndex:CEEConfigurationFilePathIndexer];
    [CEEJSONReader object:_configuration toFile:filePath];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationApplicationConfigurationChanged object:self];
}

- (void)saveWindowSettingAsDefault:(CEEWindowController*)controller {
    if (!controller)
        return;
    
    NSSize frameSize = controller.window.frame.size;
    NSPoint frameOrigin = controller.window.frame.origin;
    
    NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
    [dict setValue:@(frameOrigin.x) forKey:@"x"];
    [dict setValue:@(frameOrigin.y) forKey:@"y"];
    [dict setValue:@(frameSize.width) forKey:@"width"];
    [dict setValue:@(frameSize.height) forKey:@"height"];
    
    NSString* filePath = [self propertyIndex:CEEWindowSettingFilePathIndexer];
    [CEEJSONReader object:dict toFile:filePath];
}

- (void)startAccessingSecurityScopedResourceWithBookmark:(CEESecurityBookmark*)bookmark {
    BOOL isStale = NO;
    NSError* error = nil;
    BOOL isSuccess = NO;
    NSData* bookmarkData = [[NSData alloc] initWithBase64EncodedString:bookmark.content options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSURL *allowedURL = [NSURL URLByResolvingBookmarkData:bookmarkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:&isStale error:&error];
    if (allowedURL)
        isSuccess = [allowedURL startAccessingSecurityScopedResource];
    
    [self logSecurityBookmarks:bookmark];
}

- (void)stopAccessingSecurityScopedResourceWithBookmark:(CEESecurityBookmark*)bookmark {
    BOOL isStale = NO;
    NSError* error = nil;
    NSData* bookmarkData = [[NSData alloc] initWithBase64EncodedString:bookmark.content options:NSDataBase64DecodingIgnoreUnknownCharacters];
    NSURL *allowedURL = [NSURL URLByResolvingBookmarkData:bookmarkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:&isStale error:&error];
    if (allowedURL)
        [allowedURL stopAccessingSecurityScopedResource];
}

- (void)stopAccessingSecurityScopedResourceWithLoggedBookmarks {
    for (CEESecurityBookmark* bookmark in _securityBookmarks)
        [self stopAccessingSecurityScopedResourceWithBookmark:bookmark];
}

- (void)logSecurityBookmarks:(CEESecurityBookmark*)bookmark {
    if (!_securityBookmarks)
        _securityBookmarks = [[NSMutableArray alloc] init];
    [_securityBookmarks addObject:bookmark];
}

- (NSString*)propertyIndex:(NSString*)indexer {
    if ([indexer isEqualToString:CEEApplicationInfoStringIndexer]) {
        NSString* version = [self propertyIndex:CEEApplicationVersionIndexer];
        return [NSString stringWithFormat:@"Cymbols-%@", version, nil];
    }
    else if ([indexer isEqualToString:CEEApplicationVersionIndexer]) {
        return [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    }
    else if ([indexer isEqualToString:CEEBundleVersionIndexer]) {
        return [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
    }
    else if ([indexer isEqualToString:CEEBackwardCompatibleMaxVersionIndexer]) {
        return @"Cymbols-1.5.0";
    }
    else if ([indexer isEqualToString:CEESerializerVersionIndexer]) {
        return @"Serializer-1.0.0";
    }
    else if ([indexer isEqualToString:CEEWindowSettingFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"DefaultWindowSetting.cfg"];
    }
    else if ([indexer isEqualToString:CEEProjectSettingFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"DefaultProjectSetting.cfg"];
    }
    else if ([indexer isEqualToString:CEEConfigurationTemplateFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"Configuration.tpl"];
    }
    else if ([indexer isEqualToString:CEEConfigurationFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"Cymbols.cfg"];
    }
    else if ([indexer isEqualToString:CEEWelcomeGuideFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"WelcomeGuide/WelcomeGuide.md"];
    }
    else if ([indexer isEqualToString:CEETemporaryDirectoryIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"Backups/Untitled"];
    }
    else if ([indexer isEqualToString:CEEUpdateInfoFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"UpdateInfo.cfg"];
    }
    else if ([indexer isEqualToString:CEEStylesDirectoryIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"Styles"];
    }
    else if ([indexer isEqualToString:CEEApplicationVersionTagFilePathIndexer]) {
        return [_supportDirectory stringByAppendingPathComponent:@"Version.tag"];
    }
        
    return nil;
}

@end
