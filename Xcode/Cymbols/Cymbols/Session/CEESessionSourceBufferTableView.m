//
//  CEESessionSourceBufferTableView.m
//  Cymbols
//
//  Created by qing on 2020/9/17.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESessionSourceBufferTableView.h"

@interface CEESessionSourceBufferTableView ()
@property NSRect alternativeRowRect;
@end

@implementation CEESessionSourceBufferTableView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    CEEGridRowView* rowView = [self.grid rowViews][0];
    if (!rowView)
        return;
    
    CEEUserInterfaceStyle* style = (CEEUserInterfaceStyle*)[rowView.userInterfaceStyles pointerAtIndex:kCEEViewStyleStateHighLighted];
    NSBezierPath* borderPath = [NSBezierPath bezierPathWithRoundedRect:_alternativeRowRect xRadius:0.0 yRadius:0.0];
    NSColor* color = [[style backgroundColor] colorWithAlphaComponent:0.4];
    [color setFill];
    [borderPath fill];
}

- (void)highlightRowRect:(NSInteger)rowIndex enable:(BOOL)flag {
    _alternativeRowRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    for (CEEGridRowView* rowView in self.grid.subviews) {
        if (rowIndex == rowView.index) {
            if (flag)
                _alternativeRowRect = rowView.frame;
            [self setNeedsDisplay:YES];
            break;
        }
    }
}

@end
