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
#import "CEEWindowController.h"

NSNotificationName CEENotificationHeartBeat = @"CEENotificationHeartBeat";

@interface AppDelegate()
@property (strong) NSString* cymbolsHome;
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
    
    [self createHomeDirectory];
    [self configure];
    [self createHeartBeatTimer];
    [self createNetwork];
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [styleManager setStyleHomeDirectory:[_cymbolsHome stringByAppendingPathComponent:@"Styles"]];
    styleManager.userInterfaceStyleName = _activedConfigurations[@"UIStyle"];
    styleManager.textHighlightStyleName = _activedConfigurations[@"SyntaxStyle"];
    
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

- (void)createHomeDirectory {
    _cymbolsHome = [NSHomeDirectory() stringByAppendingPathComponent:@".Cymbols"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:_cymbolsHome isDirectory:nil])
        [[NSFileManager defaultManager] createDirectoryAtPath:_cymbolsHome withIntermediateDirectories:YES attributes:nil error:nil];
    
    // copy configure file
    NSString* fileInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Cymbols.cfg"];
    NSString* copyFilePath = [_cymbolsHome stringByAppendingPathComponent:@"Cymbols.cfg"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:copyFilePath])
        [[NSFileManager defaultManager] copyItemAtPath:fileInBundle toPath:copyFilePath error:nil];
    
    // copy welcomeGuide
    fileInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"WelcomeGuide"];
    copyFilePath = [_cymbolsHome stringByAppendingPathComponent:@"WelcomeGuide"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:copyFilePath isDirectory:nil])
        [[NSFileManager defaultManager] removeItemAtPath:copyFilePath error:nil];
    [[NSFileManager defaultManager] copyItemAtPath:fileInBundle toPath:copyFilePath error:nil];
}

- (void)createNetwork {
    _network = [[CEENetwork alloc] init];
}

- (NSString*)welcomeGuidePath {
    return [[_cymbolsHome stringByAppendingPathComponent:@"WelcomeGuide"] stringByAppendingPathComponent:@"WelcomeGuide"];
}

- (void)configure {
    NSString* filepath = [_cymbolsHome stringByAppendingPathComponent:@"Cymbols.cfg"];
    _activedConfigurations = [CEEJSONReader objectFromFile:filepath];
    _configurations = _activedConfigurations;
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
        NSArray* untiledSourceBufferFilePaths = [_sourceBufferManager untitleSourceBuffersFilePaths];
        [project removeSecurityBookmarksWithFilePaths:untiledSourceBufferFilePaths];
        [project serialize];
        [project deleteAllSessions];
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

- (IBAction)syntaxColorChange:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [styleManager setTextHighlightStyleName:@"Red"];
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

- (void)setConfiguration:(NSString*)configuration value:(NSString*)value {
    [_activedConfigurations setValue:value forKey:configuration];
    _configurations = _activedConfigurations;
    NSString* filepath = [_cymbolsHome stringByAppendingPathComponent:@"Cymbols.cfg"];
    [CEEJSONReader object:_activedConfigurations toFile:filepath];
}

@end
