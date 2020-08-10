//
//  CEESessionView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/9/5.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionView.h"

@implementation CEESessionView

@synthesize seperateLineTopSpace = _seperateLineTopSpace;

- (void)initProperties {
    _seperateLineSize = 1.0;
    _seperateLineTopSpace = 0.0;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    [self.borderColor setStroke];
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

@end
