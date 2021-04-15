//
//  CEEGridView.m
//  Cymbols
//
//  Created by qing on 2020/7/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <objc/message.h>
#import "CEEGridView.h"
#import "CEETitlebarButton.h"

#define EXPAND_BUTTON_WIDTH 16.0
#define EXPAND_BUTTON_HEIGHT 16.0

@interface CEEGridRowView () {
    NSMutableArray* _cellViews;
}
@property (strong) CEETitlebarButton* expandButton;
@end

@implementation CEEGridRowView

@synthesize cellViews = _cellViews;
@synthesize indent = _indent;
@synthesize columnOffsets = _columnOffsets;
@synthesize columnWidths = _columnWidths;
@synthesize rowStyle = _rowStyle;
@synthesize expandable = _expandable;
@synthesize expanded = _expanded;

- (void)initProperties {
    [super initProperties];
}

- (NSUInteger)numberOfCellViews {
    return _cellViews.count;
}

- (NSArray*)removeAllCellViews {
    NSArray* removes = [_cellViews copy];
    for (NSView* view in removes)
        [view removeFromSuperview];
    _cellViews = nil;
    return removes;
}

- (void)setCellViews:(NSArray*)cellViews {
    if (!_cellViews)
        _cellViews = [[NSMutableArray alloc] init];
    [_cellViews addObjectsFromArray:cellViews];
    for (CEEView* cellView in _cellViews)
        [self addSubview:cellView];
}

- (NSArray*)cellViews {
    return _cellViews;
}

- (void)setColumnOffsets:(NSArray *)columnOffsets {
    _columnOffsets = columnOffsets;
    
    for (int i = 0; i < _cellViews.count; i ++) {
        CEEView* cellView = _cellViews[i];
        NSRect cellViewRect = cellView.frame;
        cellViewRect.origin.x = [_columnOffsets[i] floatValue];
        
        if (i == 0) {
            cellViewRect.origin.x += [self placeholder];
            if (_rowStyle == kCEEGridRowViewStyleHierarchical) {
                NSRect buttonFrame = NSMakeRect(0.0, 0.0, 0.0, 0.0);
                if (_expandButton)
                    buttonFrame = _expandButton.frame;
                buttonFrame.origin.x = cellViewRect.origin.x;
                cellViewRect.origin.x = buttonFrame.origin.x + buttonFrame.size.width;
                [_expandButton setFrameOrigin:buttonFrame.origin];
            }
        }
        
        [cellView setFrameOrigin:cellViewRect.origin];
    }
}

- (NSArray*)columnOffsets {
    return _columnOffsets;
}

- (void)setColumnWidths:(NSArray *)columnWidths {
    _columnWidths = columnWidths;
    
    for (int i = 0; i < _cellViews.count; i ++) {
        CEEView* cellView = _cellViews[i];
        NSRect cellViewRect = cellView.frame;
        cellViewRect.size.width = [_columnWidths[i] floatValue];
        
        if (i == 0) {
            cellViewRect.size.width -= [self placeholder];
            if (_rowStyle == kCEEGridRowViewStyleHierarchical) {
                NSRect buttonFrame = NSMakeRect(0.0, 0.0, 0.0, 0.0);
                if (_expandButton)
                    buttonFrame = _expandButton.frame;
                cellViewRect.size.width -= buttonFrame.size.width;
                
                if ((buttonFrame.origin.x + buttonFrame.size.width > [_columnWidths[0] floatValue]) &&
                    _expandable && !_expandButton.hidden)
                    [_expandButton setHidden:YES];
                else if ((buttonFrame.origin.x + buttonFrame.size.width < [_columnWidths[0] floatValue]) &&
                         _expandable && _expandButton.hidden)
                    [_expandButton setHidden:NO];
            }
        }
        
        if (cellViewRect.size.width <= FLT_EPSILON)
            cellViewRect.size.width = 0.0;
        
        [cellView setFrameSize:cellViewRect.size];
    }
}

- (NSArray*)columnWidths {
    return _columnWidths;
}

- (CGFloat)placeholder {
    return _indent * 16.0;
}

- (void)setRowStyle:(CEEGridRowViewStyle)rowStyle {
    _rowStyle = rowStyle;
    if (_rowStyle == kCEEGridRowViewStyleHierarchical) {
        NSRect buttonFrame;
        if (!_expandButton) {
            buttonFrame = NSMakeRect(0.0,
                                     (self.frame.size.height - EXPAND_BUTTON_HEIGHT) / 2.0,
                                     EXPAND_BUTTON_WIDTH,
                                     EXPAND_BUTTON_HEIGHT);
            _expandButton = [[CEETitlebarButton alloc] initWithFrame:buttonFrame];
            [self addSubview:_expandButton];
            if (!_expandButton.target)
                [_expandButton setTarget:self.target];
            if (!_expandButton.action)
                [_expandButton setAction:self.expandAction];
        }
    }
}

- (CEEGridRowViewStyle)rowStyle {
    return _rowStyle;
}

- (void)setIndent:(CGFloat)indent {
    _indent = indent;
    
    if (!self.cellViews || !self.cellViews.count)
        return;
    
    if (_rowStyle == kCEEGridRowViewStyleHierarchical) {
        NSRect buttonFrame = NSMakeRect(0.0, 0.0, 0.0, 0.0);
        if (_expandButton)
            buttonFrame = _expandButton.frame;
        buttonFrame.origin.x = [self placeholder];
        [_expandButton setFrameOrigin:buttonFrame.origin];
        CEEView* cellView0 = self.cellViews[0];
        NSRect cellView0Frame = cellView0.frame;
        cellView0Frame.origin.x = buttonFrame.origin.x + buttonFrame.size.width;
        cellView0Frame.size.width = [_columnWidths[0] floatValue] - cellView0Frame.origin.x - buttonFrame.size.width;
        [cellView0 setFrame:cellView0Frame];
    }
    else {
        CEEView* cellView0 = self.cellViews[0];
        NSRect cellView0Frame = cellView0.frame;
        cellView0Frame.origin.x = [self placeholder];
        cellView0Frame.size.width = [_columnWidths[0] floatValue] - [self placeholder];
        [cellView0 setFrame:cellView0Frame];
    }
}

- (CGFloat)indent {
    return _indent;
}

- (void)setExpandable:(BOOL)expandable {
    _expandable = expandable;
    
    if (!_expandButton)
        return;
    
    if (_expandable && _expandButton.hidden)
        [_expandButton setHidden:NO];
    else if (!_expandable && !_expandButton.hidden)
        [_expandButton setHidden:YES];
}

- (BOOL)expandable {
    return _expandable;
}

- (void)setExpanded:(BOOL)expanded {
    _expanded = expanded;
    
    if (!_expandButton)
        return;
    
    if (_expanded)
        [_expandButton setIcon:[NSImage imageNamed:@"icon_expanded_16x16"]];
    else
        [_expandButton setIcon:[NSImage imageNamed:@"icon_expandable_16x16"]];
    
}

- (BOOL)expanded {
    return _expanded;
}

- (__kindof NSView*)replaceCellViewAtClumn:(NSInteger)column withView:(NSView*)view {
    NSView* replaced = _cellViews[column];
    NSRect frame = replaced.frame;
    if (replaced.superview)
        [replaced removeFromSuperview];
    [_cellViews replaceObjectAtIndex:column withObject:view];
    [self addSubview:view];
    [view setFrame:frame];
    return replaced;
}

@end

@interface CEEGridView ()
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
    for (int i = 0; i < self.subviews.count; i ++) {
        CEEGridRowView* rowView = self.subviews[i];
        [rowView setColumnOffsets:columnOffsets];
    }
    [self setNeedsDisplay:YES];
}

- (NSArray*)columnOffsets {
    return _columnOffsets;
}

- (void)setColumnWidths:(NSArray *)columnWidths {
    _columnWidths = columnWidths;
    for (int i = 0; i < self.subviews.count; i ++) {
        CEEGridRowView* rowView = self.subviews[i];
        [rowView setColumnWidths:columnWidths];
    }
    [self setNeedsDisplay:YES];
}

- (NSArray*)columnWidths {
    return _columnWidths;
}

- (void)appendRowViews:(NSArray*)rowViews {
    NSRect rect = [self.subviews lastObject].frame;
    CGFloat y = rect.origin.y + rect.size.height + _rowSpacing;
    for (int i = 0; i < rowViews.count; i ++) {
        CEEGridRowView* rowView = rowViews[i];
        [rowView setFrame:NSMakeRect(0.0, y, self.frame.size.width, rowView.frame.size.height)];
        [self addSubview:rowView];
        [rowView setColumnWidths:_columnWidths];
        [rowView setColumnOffsets:_columnOffsets];
        y += rowView.frame.size.height + _rowSpacing;
    }
}

- (NSArray*)removeRowViewsFromTail:(NSInteger)n {
    NSMutableArray* rowViews = nil;
    
    if (n <= 0)
        return nil;
    
    rowViews = [[NSMutableArray alloc] init];
    NSInteger rowViewIndex = self.subviews.count - 1;
    for (NSInteger i = 0; i < n; i ++)
        [rowViews addObject:self.subviews[rowViewIndex - i]];
    
    for (CEEView* rowView in rowViews)
        [rowView removeFromSuperview];
    
    return rowViews;
}

- (NSArray*)removeAllRowViews {
    NSMutableArray* rowViews = [[self subviews] copy];
    for (NSView* view in rowViews)
        [view removeFromSuperview];
    return rowViews;
}

- (__kindof NSView*)cellViewInRow:(NSUInteger)row column:(NSUInteger)column {
    if (row >= self.subviews.count || column >= self.numberOfColumns)
        return nil;
    
    CEEGridRowView* rowView = self.subviews[row];
    if (!rowView || !rowView.cellViews || !rowView.cellViews.count)
        return nil;
    
    return rowView.cellViews[column];
}

- (__kindof NSView*)replaceCellViewInRow:(NSUInteger)row column:(NSUInteger)column withView:(NSView*)view {
    CEEGridRowView* rowView = self.subviews[row];
    NSView* replaced = [rowView replaceCellViewAtClumn:column withView:view];
    return replaced;
}

- (BOOL)containCellView:(NSView*)view {
    for (CEEGridRowView* rowView in self.subviews) {
        if ([rowView.cellViews containsObject:view])
            return YES;
    }
    return NO;
}

- (void)replaceEmptyViewToCellView:(NSView*)cellView {
    for (CEEGridRowView* rowView in self.subviews) {
        for (NSInteger i = 0; i < rowView.cellViews.count; i++) {
            if (rowView.cellViews[i] == cellView)
                [rowView replaceCellViewAtClumn:i withView:[[NSView alloc] init]];
        }
    }
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
    if (!self.subviews || !self.subviews.count)
        return nil;
    return self.subviews;
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
