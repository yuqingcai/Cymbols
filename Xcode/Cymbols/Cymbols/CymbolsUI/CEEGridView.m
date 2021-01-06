//
//  CEEGridView.m
//  Cymbols
//
//  Created by qing on 2020/7/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <objc/message.h>
#import "CEEGridView.h"


@interface CEEGridRowView ()
@property CGFloat indent;
@end

@implementation CEEGridRowView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
}

- (void)setStyleState:(CEEViewStyleState)state {
    [super setStyleState:state];
    for (NSView* view in self.subviews)
        [view setStyleState:state];
}

@end


@implementation CEEGridView

@synthesize columnOffsets = _columnOffsets;
@synthesize columnWidths = _columnWidths;

- (void)initProperties {
    [super initProperties];
    _gridWidth = 1.0;
    _gridColor = [NSColor clearColor];
    _rowSpacing = 2.0;
    _enableDrawHorizontalGrid = NO;
    _enableDrawVerticalGrid = NO;
    _numberOfColumns = 0;
}

- (BOOL)isFlipped {
    return YES;
}

- (CGFloat)rowHeight {
    if (self.subviews.count)
        return self.subviews[0].frame.size.height;
    return 0.0;
}

- (NSUInteger)numberOfRows {
    return self.subviews.count;
}

- (void)setColumnOffsets:(NSArray *)columnOffsets {
    _columnOffsets = columnOffsets;
    
    CEEGridRowView* rowView = nil;
    CEEView* cellView = nil;
    NSRect rect;
    for (int i = 0; i < self.subviews.count; i ++) {
        rowView = self.subviews[i];
        for (int j = 0; j < rowView.subviews.count; j ++) {
            cellView = rowView.subviews[j];
            rect = cellView.frame;
            rect.origin.x = [_columnOffsets[j] floatValue];
            rect.size.width = [_columnWidths[j] floatValue];
            
            if (j == 0) {
                rect.origin.x += rowView.indent * 16.0;
                rect.size.width -= rowView.indent * 16.0;
                [cellView setFrame:rect];
            }
            else {
                [cellView setFrameOrigin:rect.origin];
            }
        }
    }
    [self setNeedsDisplay:YES];
}

- (NSArray*)columnOffsets {
    return _columnOffsets;
}

- (void)setColumnWidths:(NSArray *)columnWidths {
    _columnWidths = columnWidths;
    CEEGridRowView* rowView = nil;
    CEEView* cellView = nil;
    NSRect rect;
    for (int i = 0; i < self.subviews.count; i ++) {
        rowView = self.subviews[i];
        
        for (int j = 0; j < rowView.subviews.count; j ++) {
            cellView = rowView.subviews[j];
            rect = cellView.frame;
                        
            rect.size.width = [_columnWidths[j] floatValue];
            
            if (j == 0)
                rect.size.width -= rowView.indent * 16.0;
                        
            [cellView setFrameSize:rect.size];
        }
    }
    [self setNeedsDisplay:YES];
}

- (NSArray*)columnWidths {
    return _columnWidths;
}

- (void)setIndent:(CGFloat)indent row:(NSUInteger)row {
    CGFloat placeholder = indent * 16.0;
    CEEGridRowView* rowView = self.subviews[row];
    rowView.indent = indent;
    CEEView* cellView = rowView.subviews[0];
    NSRect rect = cellView.frame;
    rect.origin.x = placeholder;
    rect.size.width = [_columnWidths[0] floatValue] - placeholder;
    [cellView setFrame:rect];
    [self setNeedsDisplay:YES];
}

- (void)appendRowViews:(NSArray*)rowViews {
    NSRect rect = [self.subviews lastObject].frame;
    CGFloat y = rect.origin.y + rect.size.height + _rowSpacing;
    CEEView* rowView = nil;
    for (int i = 0; i < rowViews.count; i ++) {
        rowView = rowViews[i];
        for (int j = 0; j <  rowView.subviews.count; j ++) {
            CEEView* cellView = rowView.subviews[j];
            NSRect frame = NSMakeRect([_columnOffsets[j] floatValue], 0.0, [_columnWidths[j] floatValue], cellView.frame.size.height);
            [cellView setFrame:frame];
        }
        [rowView setFrame:NSMakeRect(0.0, y, self.frame.size.width, rowView.frame.size.height)];
        [self addSubview:rowView];
        y += rowView.frame.size.height + _rowSpacing;
    }
}

- (NSArray*)removeRowViews:(NSUInteger)numberOfRowViews {
    if (!numberOfRowViews)
        return nil;
    
    NSMutableArray* removes = [[NSMutableArray alloc] init];
    NSInteger rowViewIndex = self.subviews.count - 1;
    for (NSInteger i = 0; i < numberOfRowViews; i ++) {
        CEEView* rowView = self.subviews[rowViewIndex - i];
        [removes addObject:rowView];
    }
    
    for (CEEView* rowView in removes)
        [rowView removeFromSuperview];
    
    return removes;
}

- (NSArray*)removeAllRowViews {
    if (!self.subviews.count)
        return nil;
    
    NSArray* removes = [[self subviews] copy];
    for (NSView* view in removes)
        [view removeFromSuperview];
    return removes;
}

- (__kindof NSView*)cellViewInRow:(NSUInteger)row column:(NSUInteger)column {
    if (row >= self.subviews.count || column >= self.numberOfColumns)
        return nil;
    return self.subviews[row].subviews[column];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    if (_gridColor)
        [_gridColor setStroke];

    NSBezierPath* grid = [NSBezierPath bezierPath];
    [grid setLineWidth: _gridWidth];
    
    if (_enableDrawVerticalGrid)
        [self drawVerticalGrid];
    if (_enableDrawHorizontalGrid)
        [self drawHorizontalGrid];
}

- (void)drawVerticalGrid {
    if (!_columnOffsets || !_columnWidths)
        return;
    
    if (_gridColor)
        [_gridColor setStroke];
    
    NSBezierPath* grid = [NSBezierPath bezierPath];
    [grid setLineWidth: _gridWidth];
    
    for (NSInteger i = 0; i < self.numberOfColumns; i ++) {
        CGFloat x = [_columnOffsets[i] floatValue] + [_columnWidths[i] floatValue];
        CGFloat y = (self.gridWidth / 2.0);
        [grid moveToPoint: NSMakePoint(x, y)];
        [grid lineToPoint: CGPointMake(x, self.frame.size.height - (self.gridWidth / 2.0))];
        [grid stroke];
    }
}

- (void)drawHorizontalGrid {
    if (!self.subviews.count)
        return;
    
    if (_gridColor)
        [_gridColor setStroke];
    
    NSBezierPath* grid = [NSBezierPath bezierPath];
    [grid setLineWidth: _gridWidth];
    
    CGFloat height = 16.0;
    CGFloat x = (self.gridWidth / 2.0);
    CGFloat y = 0.0;

    if ([self rowHeight])
        height = _rowSpacing + [self rowHeight];
    
    y += height;
    while (TRUE) {
        [grid moveToPoint: NSMakePoint(x, y)];
        [grid lineToPoint: CGPointMake(self.frame.size.width - (self.gridWidth / 2.0), y)];
        [grid stroke];
        y += height;
        if (y > self.frame.size.height)
            break;
    }
}

- (NSArray*)rowViews {
    if (self.subviews.count)
        return self.subviews;
    return nil;
}

- (void)updateUserInterface {
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
    
    if (current.font)
        self.font = current.font;
    
    if (current.backgroundColor)
        self.backgroundColor = current.backgroundColor;
    
    if (current.borderColor)
        self.borderColor = current.borderColor;
    
    if (current.textColor)
        self.textColor = current.textColor;
    
    if (current.textShadow)
        self.textShadow = current.textShadow;
    
    if (current.gradient)
        self.gradient = current.gradient;
    
    self.gradientAngle = current.gradientAngle;
    
    if (current.borders)
        self.borders = current.borders;
    
    self.borderWidth = current.borderWidth;
    self.cornerRadius = current.cornerRadius;
    self.rowSpacing = current.rowSpacing;
    
    if (current.gridColor)
        self.gridColor = current.gridColor;
    
    self.enableDrawHorizontalGrid = current.enableDrawHorizontalGrid;
    self.enableDrawVerticalGrid = current.enableDrawVerticalGrid;
}

@end
