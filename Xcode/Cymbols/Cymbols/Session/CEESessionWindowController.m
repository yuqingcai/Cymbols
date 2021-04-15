//
//  CEESessionWindowController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEESessionWindowController.h"
#import "CEESessionViewController.h"
#import "CEESourceBufferManagerViewController.h"

@interface CEESessionWindowController()
@property (strong) NSWindowController* sourceBufferManagerWindowController;
@end

@implementation CEESessionWindowController

@synthesize session = _session;

- (NSRect)window:(NSWindow *)window willPositionSheet:(NSWindow *)sheet usingRect:(NSRect)rect {
    rect.origin.y -= [(CEESessionViewController*)self.window.contentViewController sheetOffset];
    return rect;
}

- (void)windowWillClose:(NSNotification *)notification {
    AppDelegate* delegate = [NSApp delegate];
    CEEProject* project = _session.project;
    if (project.sessions.count == 1) {
        if ([project isUntitled])
            [delegate saveWindowSettingAsDefault:[project.windowControllers lastObject]];
        [project serialize];
    }
    
    [_session deleteAllPorts];
    [_session.project.sessions removeObject:_session];
}

- (void)windowDidResize:(NSNotification *)notification {
    if (![self.window isVisible])
        return;
    
    AppDelegate* delegate = [NSApp delegate];
    if ([_session.project isUntitled])
        [delegate saveWindowSettingAsDefault:self];
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    __block BOOL shouldClose = YES;
    NSMutableArray* syncBuffers = [[NSMutableArray alloc] init];
    
    for (CEESessionPort* port in _session.ports) {
        for (CEESourceBuffer* buffer in [port openedSourceBuffers]) {
            if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile]) {
                if (![syncBuffers containsObject:buffer])
                    [syncBuffers addObject:buffer];
            }
        }
    }
    
    if (syncBuffers.count) {
        if (!_sourceBufferManagerWindowController)
            _sourceBufferManagerWindowController = [[NSStoryboard storyboardWithName:@"SourceBufferManager" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceBufferManagerWindowController"];
        
        CEESourceBufferManagerViewController* controller = (CEESourceBufferManagerViewController*)_sourceBufferManagerWindowController.contentViewController;
        [controller setModifiedSourceBuffers:syncBuffers];
        [self.window beginSheet:_sourceBufferManagerWindowController.window completionHandler:(^(NSInteger result) {
            if (result == NSModalResponseCancel)
                shouldClose = NO;
            else
                shouldClose = YES;
            [NSApp stopModalWithCode:result];
            [controller setModifiedSourceBuffers:nil];
        })];
        [NSApp runModalForWindow:self.window];
    }
    return shouldClose;
}

- (void)windowWillBeginSheet:(NSNotification *)notification {
    [(CEESessionViewController*)self.window.contentViewController setFrameAttachable:NO];
}

- (void)windowDidEndSheet:(NSNotification *)notification {
    [(CEESessionViewController*)self.window.contentViewController setFrameAttachable:YES];
}

- (void)setSession:(CEESession *)session {
    _session = session;
    [(CEESessionViewController*)self.window.contentViewController setSession:_session];
}

- (CEESession*)session {
    return _session;
}

- (void)windowDidBecomeMain:(NSNotification *)notification {
    AppDelegate* delegate = [NSApp delegate];
    [super windowDidBecomeMain:notification];
    [_session.project setCurrentSession:_session];
    [delegate setCurrentProject:_session.project];
}

- (void)windowDidResignMain:(NSNotification *)notification {
    [super windowDidResignMain:notification];
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"window\":"];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"frame\":\"%@\",", NSStringFromRect(self.window.frame)];
    CEESessionViewController* controller = (CEESessionViewController*)self.contentViewController;
    serializing = [serializing stringByAppendingFormat:@"%@", [controller serialize]];
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (BOOL)deserialize:(NSDictionary*)dict {
    NSRect rect = NSRectFromString(dict[@"frame"]);
    [self.window setFrame:rect display:YES];
    
    if (dict[@"IDSessionView"] && 
        [[self.contentViewController class] conformsToProtocol:@protocol(CEESerialization)])
        [(id<CEESerialization>)self.contentViewController deserialize:dict[@"IDSessionView"]];
    
    return YES;
}

@end
