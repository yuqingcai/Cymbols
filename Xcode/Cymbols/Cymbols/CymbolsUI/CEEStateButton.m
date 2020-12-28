//
//  CEEStateButton.m
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEStateButton.h"

@implementation CEEStateButton

- (void)initProperties {
    [super initProperties];
    _isExclusive = NO;
}

- (void)mouseDown:(NSEvent *)event {
    
    BOOL keepOn = YES;
    BOOL isInside = YES;
    NSPoint location;
    
    if (!self.isEnabled)
        return;
    
    if (_isExclusive && self.state == NSControlStateValueOn)
        return;
    
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        location = [self convertPoint:[event locationInWindow] fromView:nil];
        isInside = [self mouse:location inRect:[self bounds]];
    
        switch ([event type]) {
            case NSEventTypeLeftMouseUp:
                if (isInside) {
                    if (self.state == NSControlStateValueOff)
                        self.state = NSControlStateValueOn;
                    else if (self.state == NSControlStateValueOn)
                        self.state = NSControlStateValueOff;
                    
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
