//
//  CEEStateButton.m
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEStateButton.h"

@implementation CEEStateButton

- (void)mouseDown:(NSEvent *)event {
    
    BOOL keepOn = YES;
    BOOL isInside = YES;
    NSPoint location;
    
    if (!self.enabled)
        return;
    
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        location = [self convertPoint:[event locationInWindow] fromView:nil];
        isInside = [self mouse:location inRect:[self bounds]];
    
        switch ([event type]) {
            case NSEventTypeLeftMouseUp:
                if (isInside) {
                    if (self.state == NSOffState)
                        self.state = NSOnState;
                    else if (self.state == NSOnState)
                        self.state = NSOffState;
                    
                    if (self.action)
                        [self sendAction:self.action to:self.target];
                }
    
                keepOn = NO;
                break;
            default:
                /* Ignore any other kind of event. */
                break;
        }
    }
}

@end
