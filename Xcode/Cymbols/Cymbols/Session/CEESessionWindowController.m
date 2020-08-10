//
//  CEESessionWindowController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionWindowController.h"
#import "CEESessionViewController.h"

@implementation CEESessionWindowController

@synthesize session = _session;

- (NSRect)window:(NSWindow *)window willPositionSheet:(NSWindow *)sheet usingRect:(NSRect)rect {
    rect.origin.y -= [(CEESessionViewController*)self.window.contentViewController sheetOffset];
    return rect;
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
    [super windowDidBecomeMain:notification];
    [_session.project setCurrentSession:_session];
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

- (void)deserialize:(NSDictionary*)dict {
    NSRect rect = NSRectFromString(dict[@"frame"]);
    [self.window setFrame:rect display:YES];
    
    if (dict[@"IDSessionView"] && 
        [[self.contentViewController class] conformsToProtocol:@protocol(CEESerialization)])
        [(id<CEESerialization>)self.contentViewController deserialize:dict[@"IDSessionView"]];
}

@end
