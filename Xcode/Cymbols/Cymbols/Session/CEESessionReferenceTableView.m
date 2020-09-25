//
//  CEESessionReferenceTableView.m
//  Cymbols
//
//  Created by qing on 2020/9/17.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESessionReferenceTableView.h"

@interface CEESessionReferenceTableView ()
@property NSRect alternativeRowRect;
@end

@implementation CEESessionReferenceTableView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    CEETableRowView* rowView = [self.grid rowViews][0];
    if (!rowView)
        return;
    
    CEEUserInterfaceStyle* style = (CEEUserInterfaceStyle*)[rowView.userInterfaceStyles pointerAtIndex:kCEEViewStyleStateHeighLighted];
    NSBezierPath* borderPath = [NSBezierPath bezierPathWithRoundedRect:_alternativeRowRect xRadius:0.0 yRadius:0.0];
    NSColor* color = [[style backgroundColor] colorWithAlphaComponent:0.4];
    [color setFill];
    [borderPath fill];
}

- (void)hightlightRowRect:(NSInteger)rowIndex enable:(BOOL)flag {
    _alternativeRowRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    for (CEETableRowView* rowView in self.grid.subviews) {
        if (rowIndex == rowView.index) {
            if (flag)
                _alternativeRowRect = rowView.frame;
            [self setNeedsDisplay:YES];
            break;
        }
    }
}

@end
