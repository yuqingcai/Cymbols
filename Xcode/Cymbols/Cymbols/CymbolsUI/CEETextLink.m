//
//  CEETextLink.m
//  Cymbols
//
//  Created by qing on 2019/12/28.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEETextLink.h"

@interface CEETextLink ()
@property (strong) NSCursor* currentCursor;
@end

@implementation CEETextLink

@synthesize URLString = _URLString;

- (void)initProperties {
    [super initProperties];
    [self addMouseTraceArea];
}

- (void)setFrame:(NSRect)frame {
    [super setFrame:frame];
    
    if (self.trackingArea)
        [self removeTrackingArea:self.trackingArea];
    [self addMouseTraceArea];
}

- (void)mouseEntered:(NSEvent *)event {
    _currentCursor = [NSCursor currentCursor];
    [[NSCursor pointingHandCursor] set];
}

- (void)mouseExited:(NSEvent *)event {
    [_currentCursor set];
}

- (void)mouseDown:(NSEvent *)event {
    BOOL keepOn = YES;
    BOOL isInside = YES;
    NSPoint location;
    
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        location = [self convertPoint:[event locationInWindow] fromView:nil];
        isInside = [self mouse:location inRect:[self bounds]];
        
        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                break;
                
            case NSEventTypeLeftMouseUp:
                
                if (isInside && _URLString)
                    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:_URLString]];
                
                keepOn = NO;
                break;
            default:
                /* Ignore any other kind of event. */
                break;
        }
    }
}


@end
