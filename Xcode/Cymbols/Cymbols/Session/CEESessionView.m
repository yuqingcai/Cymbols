//
//  CEESessionView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/9/5.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionView.h"

@interface CEESessionView ()
@property NSRect mouseActionRegion;
@property CGFloat mouseActionRegionHeight;
@end

@implementation CEESessionView

@synthesize seperateLineTopSpace = _seperateLineTopSpace;

- (void)initProperties {
    _seperateLineSize = 1.0;
    _seperateLineTopSpace = 0.0;
    _mouseActionRegionHeight = 24.0;
}

- (void)setFrame:(NSRect)frame {
    [super setFrame:frame];
    _mouseActionRegion = NSMakeRect(0.0,
                                    self.frame.size.height - _mouseActionRegionHeight,
                                    self.frame.size.width,
                                    _mouseActionRegionHeight);
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    [self.dividerColor setStroke];
    NSBezierPath* border = [NSBezierPath bezierPath];
    [border setLineWidth: _seperateLineSize];
    NSSize frameSize = self.frame.size;
    [border moveToPoint: CGPointMake(-0.5, frameSize.height - _seperateLineTopSpace - (_seperateLineSize / 2.0))];
    [border lineToPoint: CGPointMake(frameSize.width, frameSize.height - _seperateLineTopSpace - (_seperateLineSize / 2.0))];
    [border stroke];
}

- (void)setSeperateLineTopSpace:(CGFloat)seperateLineTopSpace {
    _seperateLineTopSpace = seperateLineTopSpace;
    [self setNeedsDisplay:YES];
}

- (CGFloat)seperateLineTopSpace {
    return _seperateLineTopSpace;
}

- (void)mouseDown:(NSEvent *)event {
    if (event.clickCount == 2) {
        NSPoint point = event.locationInWindow;
        point = [self convertPoint:point fromView:nil];
        if (NSPointInRect(point, _mouseActionRegion))
            [self.window zoom:self];
    }
    [super mouseDown:event];
}

@end
