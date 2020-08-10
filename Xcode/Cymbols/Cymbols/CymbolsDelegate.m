//
//  CymbolsDelegate.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEEStyleManager.h"
#import "CEEJSONReader.h"
#import "cee_lib.h"
#import "cee_source_parsers.h"
#import "CEEAutoCompleteController.h"

NSNotificationName CEENotificationHeartBeat = @"CEENotificationHeartBeat";

@interface CymbolsDelegate()

@property (strong) NSString* cymbolsHome;
@property (strong) NSTimer* heartBeatTimer;
@property (strong) NSWindowController* sourceBufferManagerWindowController;
@end

@implementation CymbolsDelegate

- (id)init {
    self = [super init];
    if (!self)
        return nil;
    
    cee_parsers_create();
    
    [self createHomeDirectory];
    [self configure];
    [self createHeartBeatTimer];
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [styleManager setStyleHomeDirectory:[_cymbolsHome stringByAppendingPathComponent:@"Styles"]];
    styleManager.userInterfaceStyleName = _configurations[@"UIStyle"];
    styleManager.textHighlightStyleName = _configurations[@"SyntaxStyle"];
    
    _sourceBufferManager = [[CEESourceBufferManager alloc] init];
    _projectController = [[CEEProjectController alloc] init];
    
    return self;
}

- (void)createHeartBeatTimer {
    _heartBeatTimer = [NSTimer timerWithTimeInterval:0.1 target:self selector:@selector(heartBeat:) userInfo:nil repeats:YES];
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

- (NSString*)welcomeGuidePath {
    return [[_cymbolsHome stringByAppendingPathComponent:@"WelcomeGuide"] stringByAppendingPathComponent:@"WelcomeGuide"];
}

- (void)configure {
    NSString* filepath = [_cymbolsHome stringByAppendingPathComponent:@"Cymbols.cfg"];
    _configurations = [CEEJSONReader objectFromFile:filepath];
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
    for (CEEProject* project in documentController.documents)
        [project serialize];
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
        for (CEESessionPort* port in session.ports)
            [port discardSourceBuffers];
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
    [_sourceBufferManager updateSourceBuffers];
}

- (IBAction)syntaxColorChange:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [styleManager setTextHighlightStyleName:@"Red"];
}

- (CEEProject*)currentProject {
    return [_projectController currentDocument];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    if (_sourceBufferManagerWindowController && [_sourceBufferManagerWindowController.window isVisible])
        return NSTerminateCancel;
    
    if (!_sourceBufferManagerWindowController)
        _sourceBufferManagerWindowController = [[NSStoryboard storyboardWithName:@"SourceBufferManager" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceBufferManagerWindowController"];
    
    NSApplicationTerminateReply terminateReply = NSTerminateCancel;
    NSModalResponse responese = NSModalResponseCancel;
    
    responese = [NSApp runModalForWindow:_sourceBufferManagerWindowController.window];
    if (responese == NSModalResponseCancel)
        terminateReply = NSTerminateCancel;
    else if (responese == NSModalResponseOK)
        terminateReply = NSTerminateNow;
    
    [_sourceBufferManagerWindowController close];
    
    return terminateReply;
}

@end
