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

NSNotificationName CEENotificationHeartBeat = @"CEENotificationHeartBeat";

@interface AppDelegate()
@property (strong) NSTimer* heartBeatTimer;
@property (strong) NSMutableDictionary* activedConfigurations;
@end

@implementation AppDelegate

@synthesize currentProject = _currentProject;

- (id)init {
    self = [super init];
    if (!self)
        return nil;
    
    cee_parsers_create();
    
    [self setupSupportDirectory];
    [self configure];
    [self createHeartBeatTimer];
    [self createNetwork];
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [styleManager setDirectory:[_supportDirectory stringByAppendingPathComponent:@"Styles"]];
    styleManager.userInterfaceStyleName = _activedConfigurations[@"ui_style"];
    styleManager.textHighlightStyleName = _activedConfigurations[@"syntax_style"];
    _sourceBufferManager = [[CEESourceBufferManager alloc] init];
    _projectController = [[CEEProjectController alloc] init];
    
    return self;
}

- (void)createHeartBeatTimer {
    _heartBeatTimer = [NSTimer timerWithTimeInterval:CEE_APP_HEART_BEAT_INTERVAL target:self selector:@selector(heartBeat:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_heartBeatTimer forMode:NSRunLoopCommonModes];
}

- (void)heartBeat:(NSTimer *)timer {
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationHeartBeat object:self];
}

- (void)setupSupportDirectory {
    NSArray* searchPaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    _supportDirectory = [searchPaths firstObject];
    if (![[NSFileManager defaultManager] fileExistsAtPath:_supportDirectory isDirectory:nil])
        [[NSFileManager defaultManager] createDirectoryAtPath:_supportDirectory withIntermediateDirectories:YES attributes:nil error:nil];
    
    // copy configure file
    NSString* fileInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Cymbols.cfg"];
    NSString* copyFilePath = [_supportDirectory stringByAppendingPathComponent:@"Cymbols.cfg"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:copyFilePath])
        [[NSFileManager defaultManager] copyItemAtPath:fileInBundle toPath:copyFilePath error:nil];
    
    // copy welcomeGuide
    fileInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"WelcomeGuide"];
    copyFilePath = [_supportDirectory stringByAppendingPathComponent:@"WelcomeGuide"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:copyFilePath isDirectory:nil])
        [[NSFileManager defaultManager] removeItemAtPath:copyFilePath error:nil];
    [[NSFileManager defaultManager] copyItemAtPath:fileInBundle toPath:copyFilePath error:nil];
}

- (void)createNetwork {
    _network = [[CEENetwork alloc] init];
}

- (NSString*)welcomeGuidePath {
    return [[_supportDirectory stringByAppendingPathComponent:@"WelcomeGuide"] stringByAppendingPathComponent:@"WelcomeGuide"];
}

- (void)configure {
    NSString* filePath = [_supportDirectory stringByAppendingPathComponent:@"Cymbols.cfg"];
    _activedConfigurations = [CEEJSONReader objectFromFile:filePath];
    _configurations = _activedConfigurations;
}


- (NSString*)configurationFilePath {
    return [_supportDirectory stringByAppendingPathComponent:@"Cymbols.cfg"];
}

- (void)dealloc {
    
    cee_parsers_free();
    
    if (_heartBeatTimer) {
        [_heartBeatTimer invalidate];
        _heartBeatTimer = nil;
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    NSDocumentController* documentController = [NSDocumentController sharedDocumentController];
    for (CEEProject* project in documentController.documents) {
        
        if ([project isUntitled])
            [self saveWindowSettingAsDefault:[project.windowControllers lastObject]];
            
        [project serialize];
        [project deleteAllSessions];
        [project close];
    }
    [_sourceBufferManager discardUntitleSourceBuffers];
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
        [project.currentSession.activedPort openSourceBuffersWithFilePaths:@[[url path]]];
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

- (NSString*)serializerVersionString {
    return @"CymbolsSerializer_1_0_0";
}

- (NSString*)infoString {
    return @"CymbolsApplication_1_0_0";
}

- (void)setConfiguration:(NSString*)configuration value:(NSString*)value {
    [_activedConfigurations setValue:value forKey:configuration];
    _configurations = _activedConfigurations;
    NSString* filePath = [_supportDirectory stringByAppendingPathComponent:@"Cymbols.cfg"];
    [CEEJSONReader object:_activedConfigurations toFile:filePath];
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
    
    NSString* defaultWindowSettingPath = [_supportDirectory stringByAppendingPathComponent:@"DefaultWindowSetting.cfg"];
    [CEEJSONReader object:dict toFile:defaultWindowSettingPath];
}

- (NSString*) defaultWindowSettingPath {
    return [_supportDirectory stringByAppendingPathComponent:@"DefaultWindowSetting.cfg"];
}

- (NSString*) defaultProjectSettingPath {
    
    return [_supportDirectory stringByAppendingPathComponent:@"DefaultProjectSetting.cfg"];
}

@end
