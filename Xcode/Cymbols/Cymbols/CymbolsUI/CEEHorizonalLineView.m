//
//  CEEHorizonalLineView.m
//  Cymbols
//
//  Created by qing on 2020/8/13.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEHorizonalLineView.h"

@implementation CEEHorizonalLineView

- (void)initProperties {
    [super initProperties];
    _color = [NSColor gridColor];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    if (!_color)
        return;
    
    [_color setStroke];
    NSBezierPath* border = [NSBezierPath bezierPath];
    [border setLineWidth: 1.0];
    
    NSSize frameSize = self.frame.size;
    [border moveToPoint: NSMakePoint(0.0, frameSize.height - 0.5)];
    [border lineToPoint: NSMakePoint(frameSize.width, frameSize.height - 0.5)];
    [border moveToPoint: NSMakePoint(0.0, 0.0 + 0.5)];
    [border lineToPoint: NSMakePoint(frameSize.width, 0.0 + 0.5)];
    [border stroke];
}

- (void)updateUserInterface {
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
    
    if (current.color)
        self.color = current.color;
}

@end
