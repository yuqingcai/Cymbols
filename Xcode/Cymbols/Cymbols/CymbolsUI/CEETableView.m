//
//  CEETableView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/8.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <objc/message.h>
#import "CEETableView.h"

#define MIN_COLUMN_WIDTH 30.0
#define HEADER_HEIGHT 25.0
#define SCROLLER_WIDTH 15.0

@interface CEETableViewHeader() {
@private
    CGFloat _cursorRectWidth;
}

@property (nullable) SEL dragDividerAction;
@property (strong) NSArray* titles;
@property CEETableView* tableView;
@property (strong) NSArray* columnWidths;
@property (strong) NSArray* columnOffsets;
@end

@implementation CEETableViewHeader

@synthesize columnWidths = _columnWidths;
@synthesize columnOffsets = _columnOffsets;

- (void)initProperties {
    [super initProperties];
    self.borderWidth = 1.0;
    self.borders = @"bottom";
    self.dividerColor = [NSColor gridColor];
    _cursorRectWidth = 4.0;
}

- (NSRect)titleRect:(NSInteger)index {
    if (!_tableView)
        return NSMakeRect(0.0, 0.0, 0.0, 0.0);
    
    CGFloat offset = [_columnOffsets[index] floatValue];
    CGFloat width = [_columnWidths[index] floatValue];
    NSRect rect = NSMakeRect(offset,
                             self.borderWidth,
                             width,
                             self.frame.size.height - self.borderWidth * 2);
    
    rect.origin.x += self.font.pointSize;
    rect.size.width -= self.font.pointSize*2;
    return rect;
}

- (NSArray*)dividerRects {
    if (!_tableView)
        return nil;
    
    NSMutableArray* rects = [[NSMutableArray alloc] init];
    NSRect rect;
    CGFloat width = 0.0;
    CGFloat x = 0.0;
    CGFloat horizontalOffset = [_columnOffsets[0] floatValue];
    for (NSInteger i = 0; i < _columnWidths.count; i ++) {
        width += [_columnWidths[i] floatValue];
        x = horizontalOffset + width;
        rect = NSMakeRect(x - _cursorRectWidth,
                          self.borderWidth,
                          _cursorRectWidth * 2,
                          self.frame.size.height - self.borderWidth * 2);
        [rects addObject:NSStringFromRect(rect)];
    }
    return rects;
}

- (void)resetCursorRects {
    [super resetCursorRects];
    NSArray* rects = [self dividerRects];
    for (NSString* rect in rects)
        [self addCursorRect:NSRectFromString(rect) cursor:[NSCursor resizeLeftRightCursor]];
}

- (void)mouseDown:(NSEvent *)event {
    BOOL keepOn = YES;
    NSPoint location0;
    BOOL dividerDrageable = YES;
    NSArray* dividerRects;
    NSRect draggingDividerRect;
    NSInteger draggingDividerIndex = -1;

    location0 = [self convertPoint:[event locationInWindow] fromView:nil];

    // dragging column divider
    dividerRects = [self dividerRects];
    for (NSInteger i = 0; i < dividerRects.count; i ++) {
        NSRect dividerRect = NSRectFromString(dividerRects[i]);
        if (NSPointInRect(location0, dividerRect)) {
            draggingDividerIndex = i;
            draggingDividerRect = dividerRect;
        }
    }

    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [self.window nextEventMatchingMask:eventMask];
        NSPoint location1 = [self convertPoint:[event locationInWindow] fromView:nil];
        CGFloat delta = location1.x - location0.x;

        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                // dragging column divider
                if (draggingDividerIndex != -1) {
                    if (dividerDrageable) {
                        if ([self adjustWidthOfColumn:draggingDividerIndex withDelta:delta]) {
                            draggingDividerRect.origin.x += delta;
                            if (_dragDividerAction && _tableView)
                                ((void (*)(id, SEL,typeof(self)))objc_msgSend)(_tableView, _dragDividerAction, self);
                        }
                        else {
                            dividerDrageable = NO;
                        }
                    }
                    else {
                        if (fabs(location1.x - draggingDividerRect.origin.x) <= 1.0)
                            dividerDrageable = YES;
                    }
                } // end of dragging column divider
                break;

            case NSEventTypeLeftMouseUp:
                keepOn = NO;
                break;

            default:
                //Ignore any other kind of event.
                break;
        }
        location0 = location1;
    }
}

- (BOOL)adjustWidthOfColumn:(NSInteger)index withDelta:(CGFloat)delta {
    if (!_tableView)
        return NO;
    
    if (index < 0 || index >= _columnWidths.count)
        return NO;
    
    CGFloat width = [_columnWidths[index] floatValue];
    if (width + delta <= MIN_COLUMN_WIDTH)
        return NO;
    
    [self.tableView setColumnWidth:(width + delta) atIndex:index];
    
    if (delta < 0.0)
        [self.tableView adjustHorizontalOffsetWithDelta:delta];
    
    [self.window invalidateCursorRectsForView:self];
    return YES;
}

- (void)setColumnTitles:(NSArray*)titles {
    _titles = titles;
    [self setNeedsDisplay:YES];
}

- (void)setColumnWidths:(NSArray *)columnWidths {
    _columnWidths = columnWidths;
    [self setNeedsDisplay:YES];
}

- (NSArray*)columnWidths {
    return _columnWidths;
}

- (void)setColumnOffsets:(NSArray *)columnOffsets {
    _columnOffsets = columnOffsets;
    [self setNeedsDisplay:YES];
}

- (NSArray*)columnOffsets {
    return _columnOffsets;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    [self drawDividers];
    [self drawTitles];
}

- (void)drawDividers {
    NSBezierPath* path = nil;
    CGFloat dividerOffset = 0.0;
    CGFloat lineWidth = 1.0;
    CGFloat dividerInterval = 4.0;
    if (self.dividerColor)
        [self.dividerColor setStroke];
    
    if (!self.tableView)
        return ;
    
    // draw dividers
    CGFloat width = 0.0;
    CGFloat horizontalOffset = [_columnOffsets[0] floatValue];
    for (int i = 0; i < _columnWidths.count; i ++) {
        width += [_columnWidths[i] floatValue];
        dividerOffset = horizontalOffset + width;
        path = [NSBezierPath bezierPath];
        [path setLineWidth:lineWidth];
        [path moveToPoint:CGPointMake(dividerOffset, dividerInterval)];
        [path lineToPoint:CGPointMake(dividerOffset, self.frame.size.height - dividerInterval)];
        [path stroke];
    }
    // draw dividers end
}

- (void)drawTitles {
    if (!_titles || !_tableView)
        return;
    
    for (int i = 0; i < _columnWidths.count; i ++) {
        NSRect rect = [self titleRect:i];
        NSString* string = _titles[i];
        if (string)
            [self drawString:string inRect:rect];
    }
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
    self.cornerRadius = current.cornerRadius;
    
    if (current.dividerColor)
        self.dividerColor = current.dividerColor;
}

@end

typedef NS_ENUM(NSInteger, ScrollDirection) {
    kScrollDirectionNone,
    kScrollDirectionUpLeft,
    kScrollDirectionUp,
    kScrollDirectionUpRight,
    kScrollDirectionLeft,
    kScrollDirectionRight,
    kScrollDirectionDownLeft,
    kScrollDirectionDown,
    kScrollDirectionDownRight,
};

typedef NS_OPTIONS(NSInteger, ComponentState) {
    kComponentStateGrid = 1 << 0,
    kComponentStateHeader = 1 << 1,
    kComponentStateVerticalScroller = 1 << 2,
    kComponentStateHorizontalScroller = 1 << 3,
    kComponentStateHeaderPadding = 1 << 4,
};

@interface CEETableView() {
    NSMutableIndexSet* _selectedRowIndexes;
};

@property (strong) NSMutableArray* columnWidths;
@property CGFloat horizontalOffset;
@property CGFloat miniColumnWidth;
@property NSRect contentRect;
@property NSRect headerRect;
@property NSRect headerPaddingRect;
@property NSRect gridRect;
@property NSRect verticalScrollerRect;
@property NSRect horizontalScrollerRect;
@property ComponentState componentState;
@property NSTimer* autoScrollTimer;
@property NSTimer* clickDetectTimer;
@property NSUInteger clickTicktack;
@property BOOL autoScrollSelecting;
@property NSInteger keySelectedRowIndex;
@property BOOL updating;
@property NSInteger updateRow;
@property NSInteger updateColumn;
@property NSMutableArray* rowViewBuffer;
@property NSMutableArray* cellViewBuffer;
@property BOOL clickDetect;
@property NSMutableIndexSet* selectedRowIndexesClip;
@property NSMutableArray* cellViewTemplates;
- (CGFloat)columnsWidth;
- (NSArray*)columnOffsets;
@end

@implementation CEETableView

@synthesize selectedRowIndexes = _selectedRowIndexes;
@synthesize enableDrawHeader = _enableDrawHeader;
@synthesize columnAutoresizingStyle = _columnAutoresizingStyle;
@synthesize numberOfColumns = _numberOfColumns;
@synthesize numberOfRows = _numberOfRows;
@synthesize firstRowIndex = _firstRowIndex;

- (void)initProperties {
    [super initProperties];
    _autoScrollSelecting = NO;
    _draggingSessionPasteboard = [NSPasteboard pasteboardWithName:@"IDDraggingSessionPasteboard"];
    _keySelectedRowIndex = -1;
    _firstRowIndex = 0;
    _updating = NO;
    _updateRow = -1;
    _updateColumn = -1;
    _allowsMultipleSelection = NO;
    _enableDrawHeader = NO;
    _columnAutoresizingStyle = kCEETableViewNoColumnAutoresizing;
    _horizontalOffset = 0.0;
    _rowViewBuffer = [[NSMutableArray alloc] init];
    _cellViewBuffer = [[NSMutableArray alloc] init];
    
    self.borderWidth = 0.0;
    
    [self createComponents];
    [self enableComponents:kComponentStateHeader | kComponentStateGrid];
    [self setNumberOfColumns:1];
}

- (BOOL)isFlipped {
    return YES;
}

- (void)setEnableDrawHeader:(BOOL)enableDrawHeader {
    _enableDrawHeader = enableDrawHeader;
    if (_enableDrawHeader) {
        if (![self componentIsPresented:_header])
            [self enableComponents:kComponentStateHeader];
    }
    else {
        if ([self componentIsPresented:_header])
            [self disableComponents:kComponentStateHeader];
    }
}

- (BOOL)enableDrawHeader {
    return _enableDrawHeader;
}

- (void)setColumnAutoresizingStyle:(CEETableViewColumnAutoresizingStyle)columnAutoresizingStyle {
    _columnAutoresizingStyle = columnAutoresizingStyle;
}

- (CEETableViewColumnAutoresizingStyle)columnAutoresizingStyle {
    return _columnAutoresizingStyle;
}

- (void)createComponents {
    [self createHeader];
    [self createHeaderPadding];
    [self createVerticalScroller];
    [self createHorizontalScroller];
    [self createGrid];
}

- (void)setIdentifier:(NSUserInterfaceItemIdentifier)identifier {
    [super setIdentifier:identifier];
    
    if (_header)
        [_header setIdentifier:[self createComponentIdentifier:@"IDHeader"]];
    
    if (_headerPadding)
        [_headerPadding setIdentifier:[self createComponentIdentifier:@"IDHeaderPadding"]];
    
    if (_verticalScroller)
        [_verticalScroller setIdentifier:[self createComponentIdentifier:@"IDScrollerVertical"]];
    
    if (_horizontalScroller)
        [_horizontalScroller setIdentifier:[self createComponentIdentifier:@"IDScrollerHorizontal"]];
    
    if (_grid)
        [_grid setIdentifier:[self createComponentIdentifier:@"IDGridView"]];
}

- (NSInteger)numberOfRows {
    return [self.dataSource numberOfRowsInTableView:self];
}

- (void)createHeader {
    _header = [[CEETableViewHeader alloc] init];
    [_header setAutoresizingMask:NSViewWidthSizable];
    [_header setTableView: self];
    [_header setDragDividerAction:@selector(dragHeaderDivider:)];
    [_header setIdentifier:[self createComponentIdentifier:@"IDHeader"]];
}

- (void)createHeaderPadding {
    _headerPadding = [[CEETableViewHeader alloc] init];
    [_headerPadding setAutoresizingMask:NSViewMinXMargin];
    [_headerPadding setIdentifier:[self createComponentIdentifier:@"IDHeaderPadding"]];
}

- (void)createVerticalScroller {
    _verticalScroller = [[CEEScroller alloc] init];
    [_verticalScroller setDirection:kCEEScrollerDirectionVertical];
    [_verticalScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_verticalScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_verticalScroller setEnabled:YES];
    [_verticalScroller setTarget:self];
    [_verticalScroller setAction:@selector(verticalScroll:)];
    [_verticalScroller setAutoresizingMask:NSViewMinXMargin | NSViewHeightSizable];
    [_verticalScroller setBackgroundColor:self.backgroundColor];
    [_verticalScroller setKnobProportion:1.0];
    [_verticalScroller setIdentifier:[self createComponentIdentifier:@"IDScrollerVertical"]];
}

- (void)createHorizontalScroller {
    _horizontalScroller = [[CEEScroller alloc] init];
    [_horizontalScroller setDirection:kCEEScrollerDirectionHorizontal];
    [_horizontalScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_horizontalScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_horizontalScroller setEnabled:YES];
    [_horizontalScroller setTarget:self];
    [_horizontalScroller setAction:@selector(horizontalScroll:)];
    [_horizontalScroller setAutoresizingMask:NSViewMinYMargin | NSViewWidthSizable];
    [_horizontalScroller setBackgroundColor:self.backgroundColor];
    [_horizontalScroller setKnobProportion:1.0];
    [_horizontalScroller setIdentifier:[self createComponentIdentifier:@"IDScrollerHorizontal"]];
}

- (void)createGrid {
    _grid = [[CEEGridView alloc] init];
    [_grid setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [_grid setIdentifier:[self createComponentIdentifier:@"IDGridView"]];
}

- (void)enableComponents:(ComponentState)state {
    _componentState |= state;
    [self updateComponentRects];
    [self updateComponents];
    [self updateComponentsFrame];
}

- (void)disableComponents:(ComponentState)state {
    _componentState &= ~state;
    [self updateComponentRects];
    [self updateComponents];
    [self updateComponentsFrame];
}

- (void)updateComponentRects {
    _contentRect = NSMakeRect(self.borderWidth,
                              self.borderWidth,
                              self.frame.size.width - self.borderWidth * 2,
                              self.frame.size.height - self.borderWidth * 2);
    
    if (_componentState & kComponentStateHeader) {
        if (_componentState & kComponentStateVerticalScroller) {
            _headerRect = NSMakeRect(_contentRect.origin.x,
                                     _contentRect.origin.y,
                                     _contentRect.size.width -  SCROLLER_WIDTH,  HEADER_HEIGHT);
            _headerPaddingRect = NSMakeRect(_headerRect.origin.x +  _headerRect.size.width,
                                            _headerRect.origin.y,
                                            SCROLLER_WIDTH,
                                            HEADER_HEIGHT);
            _componentState |= kComponentStateHeaderPadding;
        }
        else {
            _headerRect = NSMakeRect(_contentRect.origin.x,
                                     _contentRect.origin.y,
                                     _contentRect.size.width,
                                     HEADER_HEIGHT);
            _headerPaddingRect = NSMakeRect(_headerRect.origin.x + _headerRect.size.width,
                                            _headerRect.origin.y,
                                            0,
                                            HEADER_HEIGHT);
            _componentState &= ~kComponentStateHeaderPadding;
        }
    }
    else {
        if (_componentState & kComponentStateVerticalScroller) {
            _headerRect = NSMakeRect(_contentRect.origin.x,
                                     _contentRect.origin.y,
                                     _contentRect.size.width - 
                                     SCROLLER_WIDTH,
                                     0);
            _headerPaddingRect = NSMakeRect(_headerRect.origin.x +
                                            _headerRect.size.width,
                                            _headerRect.origin.y,
                                            SCROLLER_WIDTH,
                                            0);
            _componentState |= kComponentStateHeaderPadding;
        }
        else {
            _headerRect = NSMakeRect(_contentRect.origin.x,
                                     _contentRect.origin.y,
                                     _contentRect.size.width,
                                     0);
            _headerPaddingRect = NSMakeRect(_headerRect.origin.x +
                                            _headerRect.size.width,
                                            _headerRect.origin.y,
                                            0,
                                            0);
            _componentState &= ~kComponentStateHeaderPadding;
        }
    }
        
    if (_componentState & kComponentStateVerticalScroller) {
        if (_componentState & kComponentStateHeader) {
            _verticalScrollerRect = NSMakeRect(_contentRect.origin.x +
                                               _contentRect.size.width - 
                                               SCROLLER_WIDTH,
                                               _headerPaddingRect.origin.y + 
                                               _headerRect.size.height,
                                               SCROLLER_WIDTH,
                                               _contentRect.size.height - 
                                               _headerRect.size.height);
        }
        else {
            _verticalScrollerRect = NSMakeRect(_contentRect.origin.x + 
                                               _contentRect.size.width - 
                                               SCROLLER_WIDTH,
                                               _contentRect.origin.y,
                                               SCROLLER_WIDTH,
                                               _contentRect.size.height);
        }
    }
    else {
        if (_componentState & kComponentStateHeader) {
            _verticalScrollerRect = NSMakeRect(_contentRect.origin.x + 
                                               _contentRect.size.width,
                                               _headerPaddingRect.origin.y + 
                                               _headerRect.size.height,
                                               0,
                                               _contentRect.size.height - 
                                               (_headerRect.origin.y + 
                                                _headerRect.size.height));
        }
        else {
            _verticalScrollerRect = NSMakeRect(_contentRect.origin.x + 
                                               _contentRect.size.width,
                                               _contentRect.origin.y,
                                               0,
                                               _contentRect.size.height);
        }
    }
        
    if (_componentState & kComponentStateHorizontalScroller) {
        if (_componentState & kComponentStateVerticalScroller) {
            _horizontalScrollerRect = NSMakeRect(_contentRect.origin.x,
                                                 _contentRect.origin.y +
                                                 _contentRect.size.height - 
                                                 SCROLLER_WIDTH,
                                                 _contentRect.size.width - 
                                                 SCROLLER_WIDTH,
                                                 SCROLLER_WIDTH);
        }
        else {
            _horizontalScrollerRect = NSMakeRect(_contentRect.origin.x,
                                                 _contentRect.origin.y + 
                                                 _contentRect.size.height - 
                                                 SCROLLER_WIDTH,
                                                 _contentRect.size.width,
                                                 SCROLLER_WIDTH);
        }
    }
    else {
        if (_componentState & kComponentStateVerticalScroller) {
            _horizontalScrollerRect = NSMakeRect(_contentRect.origin.x,
                                                 _contentRect.origin.y + 
                                                 _contentRect.size.height,
                                                 _contentRect.size.width - 
                                                 SCROLLER_WIDTH,
                                                 0);
        }
        else {
            _horizontalScrollerRect = NSMakeRect(_contentRect.origin.x,
                                                 _contentRect.origin.y + 
                                                 _contentRect.size.height,
                                                 _contentRect.size.width,
                                                 0);
        }
    }
    
    if (_componentState & kComponentStateGrid) {
        _gridRect = NSMakeRect(_contentRect.origin.x,
                               _headerRect.origin.y + 
                               _headerRect.size.height,
                               _headerRect.size.width,
                               _horizontalScrollerRect.origin.y - 
                               (_headerRect.origin.y + _headerRect.size.height));
    }
}

- (void)updateComponents {
    // update header
    if (_componentState & kComponentStateHeader) {
        if (![self componentIsPresented:_header]) {
            [self addSubview:_header];
            [_header setStyleState:self.styleState];
        }
    }
    else {
        if ([self componentIsPresented:_header])
            [_header removeFromSuperview];
    }
    if (!_header.styleConfiguration && _styleConfiguration)
        [_header setStyleConfiguration:_styleConfiguration];
    
    // update headerPadding
    if (_componentState & kComponentStateHeaderPadding) {
        if (![self componentIsPresented:_headerPadding])
            [self addSubview:_headerPadding];
            [_headerPadding setStyleState:self.styleState];
    }
    else {
        if ([self componentIsPresented:_headerPadding])
            [_headerPadding removeFromSuperview];
    }
    if (!_headerPadding.styleConfiguration && _styleConfiguration)
        [_headerPadding setStyleConfiguration:_styleConfiguration];
    
    // update grid
    if (_componentState & kComponentStateGrid) {
        if (![self componentIsPresented:_grid])
            [self addSubview:_grid];
            [_grid setStyleState:self.styleState];
    }
    else {
        if ([self componentIsPresented:_grid])
            [_grid removeFromSuperview];
    }
    if (!_grid.styleConfiguration && _styleConfiguration)
        [_grid setStyleConfiguration:_styleConfiguration];
    
    // update vertical scroller
    if (_componentState & kComponentStateVerticalScroller) {
        if (![self componentIsPresented:_verticalScroller])
            [self addSubview:_verticalScroller];
            [_verticalScroller setStyleState:self.styleState];
    }
    else {
        if ([self componentIsPresented:_verticalScroller])
            [_verticalScroller removeFromSuperview];
    }
    if (!_verticalScroller.styleConfiguration && _styleConfiguration)
        [_verticalScroller setStyleConfiguration:_styleConfiguration];
    
    // update horizontal scroller
    if (_componentState & kComponentStateHorizontalScroller) {
        if (![self componentIsPresented:_horizontalScroller])
            [self addSubview:_horizontalScroller];
            [_horizontalScroller setStyleState:self.styleState];
    }
    else {
        if ([self componentIsPresented:_horizontalScroller])
            [_horizontalScroller removeFromSuperview];
    }
    if (!_horizontalScroller.styleConfiguration && _styleConfiguration)
        [_horizontalScroller setStyleConfiguration:_styleConfiguration];
}

- (BOOL)componentIsPresented:(NSView*)component {
    return component.superview != nil;
}

- (void)updateComponentsFrame {
    if ([self componentIsPresented:_header])
        [_header setFrame:_headerRect];
    
    if ([self componentIsPresented:_headerPadding])
        [_headerPadding setFrame:_headerPaddingRect];
    
    if ([self componentIsPresented:_grid])
        [_grid setFrame:_gridRect];
    
    if ([self componentIsPresented:_verticalScroller])
        [_verticalScroller setFrame:_verticalScrollerRect];
    
    if ([self componentIsPresented:_horizontalScroller])
        [_horizontalScroller setFrame:_horizontalScrollerRect];
}

- (void)setFrameSize:(NSSize)newSize {
    CGFloat delta = newSize.width - self.frame.size.width;
    [super setFrameSize:newSize];
        
    if (_columnAutoresizingStyle == kCEETableViewUniformColumnAutoresizingStyle) {
        [self updateColumnWidthsFromFrameSizeWithDelta:delta];
        [_header setColumnOffsets:[self columnOffsets]];
        [_header setColumnWidths:[self columnWidths]];
        [_grid setColumnOffsets:[self columnOffsets]];
        [_grid setColumnWidths:[self columnWidths]];
    }
    
    [self adjustGridRowViewsWhenFrameSizeChanged];
    [self updateGrid];
    [self tintedGridRowViews];
    [self adjustHorizontalScroller];
    [self adjustHorizontalOffsetWithDelta:delta];
    [self adjustVerticalScroller];
}

- (void)dragHeaderDivider:(id)sender {
    [self adjustHorizontalScroller];
    [_header setColumnOffsets:[self columnOffsets]];
    [_header setColumnWidths:[self columnWidths]];
    [_grid setColumnOffsets:[self columnOffsets]];
    [_grid setColumnWidths:[self columnWidths]];
    [self tintedGridRowViews];
}

- (void)adjustHorizontalScroller {
    CGFloat diff = [self columnsWidth] - self.frame.size.width;
    if (diff > FLT_EPSILON) {
        if (!(_componentState & kComponentStateHorizontalScroller))
            [self enableComponents:kComponentStateHorizontalScroller];
        [_horizontalScroller setKnobProportion:[self horizontalScrollerProportion]];
        [_horizontalScroller setFloatValue:[self horizontalScrollerOffset]];
    }
    else {
        if (_componentState & kComponentStateHorizontalScroller)
            [self disableComponents:kComponentStateHorizontalScroller];
    }
}

- (void)adjustVerticalScroller {
    if (_grid.rowViews.count < self.numberOfRows) {
        if (!(_componentState & kComponentStateVerticalScroller))
            [self enableComponents:kComponentStateVerticalScroller];
        [_verticalScroller setKnobProportion:[self verticalScrollerProportion]];
        [_verticalScroller setFloatValue:[self verticalScrollerOffset]];
    }
    else {
        if (_componentState & kComponentStateVerticalScroller)
            [self disableComponents:kComponentStateVerticalScroller];
    }
}

- (CGFloat)columnsWidth {
    CGFloat width = 0;
    for (int i = 0; i < _columnWidths.count; i ++)
        width += [_columnWidths[i] floatValue];
    return width;
}

- (void)setColumnWidth:(CGFloat)width atIndex:(NSInteger)column {
    _columnWidths[column] = @(width);
}

- (void)adjustHorizontalOffsetWithDelta:(CGFloat)delta {
    if (_horizontalOffset < 0.0) {
        if (delta > 0.0) {
            _horizontalOffset += delta;
            if (_horizontalOffset > 0.0)
                _horizontalOffset = 0;
        }
        else if (delta < 0.0) {
            _horizontalOffset += -delta;
            if (_horizontalOffset > 0.0)
                _horizontalOffset = 0.0;
        }
        
        [_header setColumnOffsets:[self columnOffsets]];
        [_grid setColumnOffsets:[self columnOffsets]];
        [_horizontalScroller setFloatValue:[self horizontalScrollerOffset]];
    }
}

- (NSArray*)columnOffsets {
    CGFloat offset = 0.0;
    CGFloat x = 0.0;
    NSMutableArray* offsets = [[NSMutableArray alloc] init];
    for (NSInteger i = 0; i < _columnWidths.count; i ++) {
        if (i > 0)
            x += [_columnWidths[i-1] floatValue];
        offset = x + _horizontalOffset;
        [offsets addObject:[NSNumber numberWithFloat:offset]];
    }
    return offsets;
}

- (void)horizontalScroll:(id)sender {
    CGFloat diff = [self columnsWidth] - self.frame.size.width;
    if (diff > FLT_EPSILON) {
        _horizontalOffset = -(_horizontalScroller.floatValue * diff);
        [_header setColumnOffsets:[self columnOffsets]];
        [_grid setColumnOffsets:[self columnOffsets]];
    }
}

- (void)verticalScroll:(id)sender {
    if (!self.numberOfRows)
        return;
    
    _firstRowIndex = _verticalScroller.floatValue * (self.numberOfRows - _grid.rowViews.count);
    [self updateGrid];
    [self tintedGridRowViews];
}

- (CGFloat)horizontalScrollerOffset {
    CGFloat diff = [self columnsWidth] - self.frame.size.width;
    if (diff > FLT_EPSILON)
        return (-_horizontalOffset) / diff;
    return 0.0;
}

- (CGFloat)horizontalScrollerProportion {
    return (self.frame.size.width / [self columnsWidth]);
}

- (CGFloat)verticalScrollerProportion {
    return (CGFloat)_grid.rowViews.count / self.numberOfRows;
}

- (CGFloat)verticalScrollerOffset {
    return (CGFloat)_firstRowIndex / (self.numberOfRows - _grid.rowViews.count);
}

- (void)reloadData {
    if (!self.dataSource || !self.delegate)
        return;
    
    [self reloadHeader];
    
    if (_firstRowIndex >= [self numberOfRows]) {
        _firstRowIndex = 0;
        _selectedRowIndexes = nil;
    }
    
    if (!self.grid.rowViews.count)
        [self createGridRowViews];
    else
        [self adjustGridRowViewsWhenDataReload];
    
    [self updateGrid];
    [self tintedGridRowViews];
    [self adjustHorizontalScroller];
    [self adjustVerticalScroller];
}

- (void)reloadHeader {
    if (!self.header)
        return;
    
    NSMutableArray* titles = [[NSMutableArray alloc] init];
    for (int i = 0; i < self.numberOfColumns; i ++) {
        NSString* title = [self titleForColumn:i];
        if (title)
            [titles addObject:title];
    }
    [self.header setColumnTitles:titles];
}

- (NSString*)titleForColumn:(NSInteger)column {
   return [self.delegate tableView:self titleForColumn:column];
}

- (void)recycleGridRowViews {
    // Save all row views to buffer.
    NSArray* rowViews = [_grid removeAllRowViews];
    for (CEEGridRowView* rowView in rowViews)
        [self recycleGridRowView:rowView];
}

- (void)recycleGridRowView:(CEEGridRowView*)rowView {
    [_rowViewBuffer addObject:rowView];
    NSArray* cellViews = [rowView removeAllCellViews];
    [_cellViewBuffer addObjectsFromArray:cellViews];
}

- (NSUInteger)gridRowCapability {
    return _grid.frame.size.height / _grid.rowHeight + _grid.rowSpacing;
}

- (void)createGridRowViews {
    NSUInteger numberOfRows = [self numberOfRows];
    
    if (!numberOfRows)
        return;
    
    if (_grid.rowViews)
        [self recycleGridRowViews];
    
    _firstRowIndex = 0;
    NSMutableArray* rowViews = [[NSMutableArray alloc] init];
    CEEGridRowView* rowView = nil;
    NSInteger row = _firstRowIndex;
    CGFloat y = 0.0;
    
    while(TRUE) {
        
        rowView = [self createRowViewWithIndex:row];
        if (!rowView)
            break;
        
        if (y + _grid.rowSpacing + rowView.frame.size.height > _grid.frame.size.height) {
            [self recycleGridRowView:rowView];
            break;
        }
        
        [rowViews addObject:rowView];
        y += _grid.rowSpacing + rowView.frame.size.height;
        row ++;
        
        if (row >= numberOfRows)
            break;
    }
    
    [_grid appendRowViews:rowViews];
}

- (void)adjustGridRowViewsWhenDataReload {
    NSUInteger rowCapability = [self gridRowCapability];
    
    if (!rowCapability)
        return;
    
    NSUInteger numberOfRows = [self numberOfRows];
    if (!numberOfRows) {
        [self recycleGridRowViews];
        return;
    }
    
    NSUInteger n = numberOfRows - _firstRowIndex;
    
    if (n > _grid.numberOfRows) {
        // append row views
        if (rowCapability > _grid.numberOfRows) {
            NSUInteger append = MIN(numberOfRows - _firstRowIndex - _grid.numberOfRows,
                                    rowCapability - _grid.numberOfRows);
            [self insertRowViewsAtTail:append];
        }
    }
    else if (n < _grid.numberOfRows) {
        NSUInteger m = _firstRowIndex;
        NSUInteger d = _grid.numberOfRows - n;
        
        // remove row views
        if (d > m) {
            _firstRowIndex = 0;
            [self removeRowViewsAtTail:(d - m)];
        }
        else { // scroll up
            _firstRowIndex -= m;
        }
    }
}

- (void)adjustGridRowViewsWhenFrameSizeChanged {
    if (!_grid.numberOfRows)
        return;
    
    NSUInteger rowCapability = [self gridRowCapability];
    if (!rowCapability)
        return;
    
    if (_grid.numberOfRows < rowCapability)
        [self insertRowViewsAtTailIfNeed:rowCapability - _grid.numberOfRows];
    else if (_grid.numberOfRows > rowCapability)
        [self removeRowViewsAtTail:_grid.numberOfRows - rowCapability];
}

- (void)insertRowViewsAtTailIfNeed:(NSUInteger)n {
    if (!n)
        return;
    
    NSUInteger tail = 0;
    NSUInteger head = 0;
    NSArray* rowViews = nil;
    NSInteger lastRowIndex = _firstRowIndex + _grid.rowViews.count - 1;
    
    if (lastRowIndex + n >= self.numberOfRows)
        tail = self.numberOfRows - (lastRowIndex + 1);
    else
        tail = n;
    
    n -= tail;
    if (n) {
        if (n >= _firstRowIndex)
            head = _firstRowIndex;
        else
            head = n;
    }
    
    // append row views
    if (tail) {
        rowViews = [self createRowViewsInRange:NSMakeRange(lastRowIndex+1, tail)];
        [_grid appendRowViews:rowViews];
    }
    
    // scroll up
    if (head) {
        rowViews = [self createRowViewsInRange:NSMakeRange(_firstRowIndex-head, head)];
        [_grid appendRowViews:rowViews];
        _firstRowIndex -= head;
    }
}

- (void)insertRowViewsAtTail:(NSUInteger)n {
    NSInteger lastRowIndex = _firstRowIndex + _grid.rowViews.count - 1;
    NSArray* rowViews = [self createRowViewsInRange:NSMakeRange(lastRowIndex+1, n)];
    [_grid appendRowViews:rowViews];
}

- (void)removeRowViewsAtTail:(NSUInteger)n {
    NSArray* rowViews = [_grid removeRowViewsFromTail:n];
    [_rowViewBuffer addObjectsFromArray:rowViews];
    for (CEEGridRowView* rowView in rowViews) {
        // Remove and Save cell views to buffer.
        NSArray* cellViews = [rowView removeAllCellViews];
        [_cellViewBuffer addObjectsFromArray:cellViews];
    }
}

- (void)updateGrid {
    if (!self.numberOfRows)
        return;
    
    _updating = YES;
    _updateRow = 0;
    _updateColumn = 0;
    
    for (NSInteger i = 0; i < _grid.numberOfRows; i ++) {
        
        NSInteger rowIndex = _firstRowIndex + i;
        CEEGridRowView* rowView = _grid.rowViews[i];
        
        [rowView setIndex:rowIndex];
        
        for (NSInteger j = 0; j < _grid.numberOfColumns; j ++)
            [self viewForColumn:j row:rowIndex];
        
        [rowView setRowStyle:[self rowGridRowViewStyle]];
        [rowView setIndent:[self rowViewIndent:rowIndex]];
        [rowView setExpandable:[self isRowViewExpandable:rowIndex]];
        [rowView setExpanded:[self isRowViewExpanded:rowIndex]];
    }
    
    _updating = NO;
}

- (BOOL)isRowViewExpandable:(NSInteger)row {
    return FALSE;
}

- (BOOL)isRowViewExpanded:(NSInteger)row {
    return FALSE;
}

- (CEEGridRowViewStyle)rowGridRowViewStyle {
    return kCEEGridRowViewStylePlain;
}

- (NSInteger)rowViewIndent:(NSInteger)row {
    if ([self.delegate respondsToSelector:@selector(tableView:indentForRow:)])
        return [self.delegate tableView:self indentForRow:row];
    return 0.0;
}

- (__kindof NSView*)viewForColumn:(NSInteger)column row:(NSInteger)row {
    return [self.delegate tableView:self viewForColumn:column row:row];
}

- (__kindof NSView*)makeViewWithIdentifier:(NSUserInterfaceItemIdentifier)identifier {
    NSView* cellView = nil;
    if (_updating) {
        cellView = [_grid cellViewInRow:_updateRow column:_updateColumn];
        _updateColumn ++;
        if (_updateColumn == _grid.numberOfColumns) {
            _updateColumn = 0;
            _updateRow ++;
        }
    }
    else {
        cellView = [self getCellViewFromBufferWithIdentifier:identifier];
        if (!cellView)
            cellView = [self createCellViewWidthIdentifier:identifier];
    }
    return cellView;
}

- (__kindof NSView*)createCellViewWidthIdentifier:(NSUserInterfaceItemIdentifier)identifier {
    if (!_nibNameOfCellView)
        return nil;
    
    NSArray *objects = [[NSArray alloc] init];
    if ([[NSBundle mainBundle] loadNibNamed:_nibNameOfCellView owner:self topLevelObjects:&objects]) {
        for (NSObject* object in objects) {
            if ([object conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
                if ([[((id<NSUserInterfaceItemIdentification>)object) identifier] isEqualToString:identifier])
                    return (NSView*)object;
            }
        }
    }
    return nil;
}

- (CEEGridRowView*)getRowViewFromBuffer {
    if (!_rowViewBuffer.count)
        return nil;
    
    CEEGridRowView* view = [_rowViewBuffer objectAtIndex:0];
    [_rowViewBuffer removeObjectAtIndex:0];
    return view;
}

- (NSView*)getCellViewFromBufferWithIdentifier:(NSString*)identifier {
    if (!_cellViewBuffer.count)
        return nil;
    
    NSView* view = nil;
    for (NSInteger i = 0; i < _cellViewBuffer.count; i ++) {
        view = [_cellViewBuffer objectAtIndex:i];
        if ([view.identifier isEqualToString:identifier]) {
            [_cellViewBuffer removeObjectAtIndex:i];
            break;
        }
    }
    return view;
}

- (NSArray*)createRowViewsInRange:(NSRange)range {
    NSMutableArray* rowViews = [[NSMutableArray alloc] init];
    for (NSInteger i = range.location; i < range.location + range.length; i ++) {
        CEEGridRowView* rowView = [self createRowViewWithIndex:i];
        [rowViews addObject:rowView];
    }
    return rowViews;
}

- (CEEGridRowView*)createRowViewWithIndex:(NSInteger)row {
    CEEGridRowView* rowView = nil;
    CGFloat rowHeight = 1.0;
    
    NSMutableArray* cellViews = [[NSMutableArray alloc] init];
    for (NSInteger j = 0; j < _grid.numberOfColumns; j ++) {
        CEEView* cellView = [self viewForColumn:j row:row];
        if (cellView) {
            [cellViews addObject:cellView];
            if (!cellView.styleConfiguration)
                [cellView setStyleConfiguration:_styleConfiguration];
            if (cellView.frame.size.height > rowHeight)
                rowHeight = cellView.frame.size.height;
        }
    }
    
    rowView = [self getRowViewFromBuffer];
    if (!rowView) {
        rowView = [[CEEGridRowView alloc] init];
        [rowView setAutoresizingMask:NSViewWidthSizable];
        [rowView setStyleConfiguration:_styleConfiguration];
    }
    [rowView setFrame:NSMakeRect(0.0, 0.0, _grid.frame.size.width, rowHeight)];
    [rowView setIndex:row];
    [rowView setCellViews:cellViews];
    [rowView setTarget:self];
    [rowView setExpandAction:@selector(expandAction:)];
    
    return rowView;
}

- (void)expandAction:(id)sender {
}

- (void)mouseDown:(NSEvent*)event {
    [self.window makeFirstResponder:self];
    
    BOOL keepOn = YES;
    BOOL isDragging = NO;
    BOOL clickDetect = NO;
    NSPoint location0 = [self convertPoint:[event locationInWindow] fromView:nil];
    NSInteger rowAtMouseDown = [self rowIndexByLocation:location0];
    NSMutableIndexSet* selectedWhenDragging = nil;
    NSMutableIndexSet* selectedOverlap = nil;
    
    _clickedRow = rowAtMouseDown;
    
    if (rowAtMouseDown == -1)
        goto exit;
    
    [self stopClickDetect];
    
    // double click
    if (event.clickCount == 2) {
        if (_doubleAction)
            ((void (*)(id, SEL,typeof(self)))objc_msgSend)(_target, _doubleAction, self);
        clickDetect = NO;
        goto exit;
    } // end of double click
    
    // row selection when mouse down
    if (event.modifierFlags & NSEventModifierFlagShift) {
        _selectedRowIndexesClip = nil;
        if (!_allowsMultipleSelection) {
            _keySelectedRowIndex = rowAtMouseDown;
            _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
        }
        else {
            [_selectedRowIndexes addIndexes:[self selectionFrom:_keySelectedRowIndex to:rowAtMouseDown]];
        }
    }
    else if (event.modifierFlags & NSEventModifierFlagCommand) {
        if (!_allowsMultipleSelection) {
            _keySelectedRowIndex = rowAtMouseDown;
            _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
        }
        else {
            _selectedRowIndexesClip = [[NSMutableIndexSet alloc] initWithIndexSet:_selectedRowIndexes];
            
            if ([_selectedRowIndexes containsIndex:rowAtMouseDown]) {
                [_selectedRowIndexes removeIndex:rowAtMouseDown];
                if (_selectedRowIndexes.count == 0) {
                    _keySelectedRowIndex = 0;
                    _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
                }
                else {
                    _keySelectedRowIndex = [_selectedRowIndexes indexGreaterThanIndex:rowAtMouseDown];
                    if (_keySelectedRowIndex == NSNotFound)
                        _keySelectedRowIndex = [_selectedRowIndexes indexLessThanIndex:rowAtMouseDown];
                }
            }
            else {
                _keySelectedRowIndex = rowAtMouseDown;
                [_selectedRowIndexes addIndex:rowAtMouseDown];
            }
        }
    }
    else {
        _selectedRowIndexesClip = nil;
        if ([_selectedRowIndexes containsIndex:rowAtMouseDown]) {
            _keySelectedRowIndex = rowAtMouseDown;
            clickDetect = YES;
        }
        else {
            _keySelectedRowIndex = rowAtMouseDown;
            _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
        }
    } // end of row selection when mouse down
    
    [self tintedGridRowViews];
    
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [self.window nextEventMatchingMask:eventMask];
        NSPoint location1 = [self convertPoint:[event locationInWindow] fromView:nil];
        CGFloat deltaX = location1.x - location0.x;
        CGFloat deltaY = location1.y - location0.y;
        
        NSInteger rowAtMouseDragging = [self rowIndexByLocation:location1];
        if (rowAtMouseDragging == -1)
            rowAtMouseDragging = [[_grid.rowViews lastObject] index];
        
        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                
                clickDetect = NO;
                [self stopClickDetect];
                
                // start dragging row
                if ([self dataSourceSupportDragAndDrop]) {
                    
                    if (fabs(deltaX) < 0.1 || fabs(deltaY) < 0.1)
                        break;
                    
                    [_draggingSessionPasteboard clearContents];
                    
                    if ([self dataSourceWriteSelectedIndexesToPasteboard]) {
                        NSDraggingItem* draggingItem = [[NSDraggingItem alloc] initWithPasteboardWriter:self];
                        NSImage* draggingImage = [self createDraggingImage];
                        [draggingItem setDraggingFrame:NSMakeRect(location1.x, location1.y, draggingImage.size.width, draggingImage.size.height) contents:draggingImage];
                        [self beginDraggingSessionWithItems:@[draggingItem] event:event source:(id<NSDraggingSource>)self];
                        keepOn = NO;
                        break; // break from current event handle
                    }
                } // end of dragging row
                
                isDragging = YES;
                // auto scroll selection when dragging
                if ([self shouldAutoScrollSelecting:location1] && !_autoScrollSelecting) {
                    [self startAutoScrollSelecting];
                    _autoScrollSelecting = YES;
                }
                else if (![self shouldAutoScrollSelecting:location1] && _autoScrollSelecting) {
                    [self stopAutoScrollSelecting];
                    _autoScrollSelecting = NO;
                } // end of auto scroll selection
                
                if (!_autoScrollSelecting && NSPointInRect(location1, _grid.frame)) {
                    if (_allowsMultipleSelection) {
                        if (_selectedRowIndexesClip) {
                            selectedWhenDragging = [self selectionFrom:_keySelectedRowIndex to:rowAtMouseDragging];
                            selectedOverlap = [[NSMutableIndexSet alloc] init];
                            [selectedWhenDragging enumerateIndexesUsingBlock:^(NSUInteger index, BOOL *stop) {
                                if ([_selectedRowIndexesClip containsIndex:index])
                                    [selectedOverlap addIndex:index];
                            }];
                            _selectedRowIndexes = [[NSMutableIndexSet alloc] initWithIndexSet:_selectedRowIndexesClip];
                            [_selectedRowIndexes addIndexes:selectedWhenDragging];
                            [_selectedRowIndexes removeIndexes:selectedOverlap];
                        }
                        else {
                            _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:rowAtMouseDragging];
                        }
                    }
                    else {
                        _keySelectedRowIndex = rowAtMouseDragging;
                        _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
                    }
                    [self tintedGridRowViews];
                }
                
                break;
                
            case NSEventTypeLeftMouseUp:
                keepOn = NO;
                
                // stop auto scroll select
                if (_autoScrollSelecting) {
                    [self stopAutoScrollSelecting];
                    _autoScrollSelecting = NO;
                }
                                
                // do click action
                if (_action)
                    ((void (*)(id, SEL,typeof(self)))objc_msgSend)(_target, _action, self);
                
                if (clickDetect)
                    [self startClickDetect];
                [[self nextResponder] mouseUp:event];
                break;

            default:
                //Ignore any other kind of event.
                break;
        }
        location0 = location1;        
    }
    
exit:
    [super mouseDown:event];
}

- (BOOL)dataSourceSupportDragAndDrop {
    return [self.dataSource respondsToSelector:@selector(tableView:writeRowsWithIndexes:toPasteboard:)];
}

- (BOOL)dataSourceWriteSelectedIndexesToPasteboard {
    return [self.dataSource tableView:self writeRowsWithIndexes:_selectedRowIndexes toPasteboard:_draggingSessionPasteboard];
}

- (void)rightMouseDown:(NSEvent *)event {
    NSPoint location0 = [self convertPoint:[event locationInWindow] fromView:nil];
    NSInteger rowAtMouseDown = [self rowIndexByLocation:location0];
    _clickedRow = rowAtMouseDown;
    [super rightMouseDown:event];
}

- (void)startClickDetect {
    _clickTicktack = 0;
    _clickDetectTimer = [NSTimer timerWithTimeInterval:0.1 target:self selector:@selector(clickDetecting:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:_clickDetectTimer forMode:NSRunLoopCommonModes];
}

- (void)stopClickDetect {
    if (_clickDetectTimer) {
        [_clickDetectTimer invalidate];
        _clickDetectTimer = nil;
    }
}

- (void)clickDetecting:(NSTimer*)timer {
    _clickTicktack ++;
    if (_clickTicktack == 5) {
        _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
        [self tintedGridRowViews];
        [self stopClickDetect];
    }
}

- (NSImage*)createDraggingImage {
    NSArray* filePaths = [_draggingSessionPasteboard propertyListForType:NSFilenamesPboardType];
    
    NSUInteger count = filePaths.count;
    if (count >= 20)
        count = 20;
    CGFloat rowHeight = [_grid rowHeight];
    CGFloat width = 200.0;
    CGFloat height = count * rowHeight;
    
    if (height < FLT_EPSILON)
        height = rowHeight * 2;
    
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];
    [image lockFocus];
    [[NSColor clearColor] set];
    NSRectFill(NSMakeRect(0, 0, image.size.width, image.size.height));
    
    NSDictionary* attributes = @{
        NSFontAttributeName : self.font,
        NSForegroundColorAttributeName : self.textColor,
        NSKernAttributeName : @(0.22)
    };
    
    for (NSInteger i = 0; i < count; i ++) {
        NSRect rect = NSMakeRect(0.0, 0.0, image.size.width, rowHeight);
        NSString* string = [filePaths[i] lastPathComponent];
        NSAttributedString* attributedString = [self string:string withAttributes:attributes inRect:rect truncate:NSLineBreakByTruncatingTail];
        NSRect drawingRect = NSMakeRect(0.0, rowHeight * i, attributedString.size.width, attributedString.size.height);
        [attributedString drawInRect:drawingRect];
    }
    [image unlockFocus];
    
    return image;
}

- (NSMutableIndexSet*)selectionFrom:(NSInteger)from to:(NSInteger)to {
    if (from < 0 || to < 0)
        return nil;
    
    NSMutableIndexSet* selection = nil;
    NSRange range;
    if (from < to)
        range = NSMakeRange(from, to - from + 1);
    else
        range = NSMakeRange(to, from - to + 1);
    
    selection = [NSMutableIndexSet indexSetWithIndexesInRange:range];
    return selection;
}

- (void)tintedGridRowViews {
    for (CEEGridRowView* rowView in _grid.rowViews) {
        NSInteger index = rowView.index;
        if ([_selectedRowIndexes containsIndex:index]) {
            if (rowView.styleState != kCEEViewStyleStateHighLighted)
                [rowView setStyleState:kCEEViewStyleStateHighLighted];
        }
        else {
            if (rowView.styleState == kCEEViewStyleStateHighLighted)
                [rowView setStyleState:kCEEViewStyleStateActived];
        }
    }
}

- (NSInteger)rowIndexByLocation:(NSPoint)point {
    point = [_grid convertPoint:point fromView:self];
    for (CEEGridRowView* rowView in _grid.rowViews) {
        NSRect rect = rowView.frame;
        rect.origin.y -= _grid.rowSpacing;
        rect.size.height += _grid.rowSpacing;
        if (NSPointInRect(point, rect))
            return [rowView index];
    }
    return -1;
}

- (void)setNumberOfColumns:(NSInteger)columns {
    _numberOfColumns = columns;
    [self initColumnWidths];
    if (_grid) {
        [_grid setNumberOfColumns:columns];
        [_grid setColumnWidths:[self columnWidths]];
        [_grid setColumnOffsets:[self columnOffsets]];
    }
    if (_header) {
        [_header setColumnWidths:[self columnWidths]];
        [_header setColumnOffsets:[self columnOffsets]];
    }
}

- (NSInteger)numberOfColumns {
    return _numberOfColumns;
}

- (void)initColumnWidths {
    CGFloat width = self.frame.size.width / _numberOfColumns;
    _columnWidths = [[NSMutableArray alloc] init];
    for (NSInteger i = 0; i < _numberOfColumns; i ++)
        [_columnWidths addObject:[NSNumber numberWithFloat:width]];
}

- (void)updateColumnWidthsFromFrameSizeWithDelta:(CGFloat)delta {
    for (NSInteger i = 0; i < _columnWidths.count; i ++) {
        CGFloat width = [_columnWidths[i] floatValue];
        width += (delta / _columnWidths.count);
        _columnWidths[i] = [NSNumber numberWithFloat:width];
    }
}

- (ScrollDirection)scrollDirection:(NSPoint)point {
    NSInteger x = point.x;
    NSInteger y = point.y;
    NSInteger x0 = _grid.frame.origin.x;
    NSInteger y0 = _grid.frame.origin.y;
    NSInteger x1 = _grid.frame.origin.x + _grid.frame.size.width;
    NSInteger y1 = _grid.frame.origin.y + _grid.frame.size.height;
        
    if (x < x0 && y < y0)
        return kScrollDirectionUpLeft;
    else if (x >= x0 && x <= x1 && y < y0)
        return kScrollDirectionUp;
    else if (x > x1 && y < y0)
        return kScrollDirectionUpRight;
    else if (x < x0 && y >= y0 && y <= y1)
        return kScrollDirectionLeft;
    else if (x > x1 && y >= y0 && y <= y1)
        return kScrollDirectionRight;
    else if (x < x0 && y > y1)
        return kScrollDirectionDownLeft;
    else if (x >= x0 && x <= x1 && y > y1)
        return kScrollDirectionDown;
    else if (x > x1 && y > y1)
        return kScrollDirectionDownRight;
    
    return kScrollDirectionNone;
}

- (BOOL)shouldAutoScrollSelecting:(NSPoint)point {
    return [self scrollDirection:point] == kScrollDirectionNone ? NO : YES;
}

- (void)startAutoScrollSelecting {
    _autoScrollTimer = [NSTimer timerWithTimeInterval:0.05 target:self selector:@selector(autoScrollSelecting:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:_autoScrollTimer forMode:NSRunLoopCommonModes];
}

- (void)stopAutoScrollSelecting {
    if (_autoScrollTimer) {
        [_autoScrollTimer invalidate];
        _autoScrollTimer = nil;
    }
}

- (void)autoScrollSelecting:(NSTimer*)timer {
    NSPoint point = [self convertPoint:[self.window mouseLocationOutsideOfEventStream] fromView:nil];
    ScrollDirection direction = [self scrollDirection:point];
    
    switch (direction) {
        case kScrollDirectionDown:
            [self scrollDownWithSelection:YES];
            break;
            
        case kScrollDirectionDownRight:
            [self scrollDownWithSelection:YES];
            [self scrollRight];
            break;
            
        case kScrollDirectionDownLeft:
            [self scrollDownWithSelection:YES];
            [self scrollLeft];
            break;
            
        case kScrollDirectionUp:
            [self scrollUpWithSelection:YES];
            break;
            
        case kScrollDirectionUpRight:
            [self scrollUpWithSelection:YES];
            [self scrollRight];
            break;
            
        case kScrollDirectionUpLeft:
            [self scrollUpWithSelection:YES];
            [self scrollLeft];
            break;
            
        case kScrollDirectionLeft:
            [self scrollLeft];
            break;
            
        case kScrollDirectionRight:
            [self scrollRight];
            break;
            
        default:
            break;
    }
}

- (void)scrollWheel:(NSEvent*)event {
    if (_grid.rowViews.count < self.numberOfRows) {
        if (event.scrollingDeltaY < 0) {
            [self scrollDownWithSelection:NO];
        }
        else {
            if (_firstRowIndex > 0)
                [self scrollUpWithSelection:NO];
        }
    }
    
    if ([self columnsWidth] - self.frame.size.width > FLT_EPSILON) {
        if (event.scrollingDeltaX < 0.0)
            [self scrollLeft];
        else if (event.scrollingDeltaX > 0.0)
            [self scrollRight];
    }
}

- (void)scrollUpWithSelection:(BOOL)select {
    if (_firstRowIndex == 0)
        return;
    
    _firstRowIndex --;
    [self updateGrid];
    
    if (select) {
        NSInteger firstIndex = [[_grid.rowViews firstObject] index];
        if (_allowsMultipleSelection) {
            NSMutableIndexSet* selectedWhenDragging = [self selectionFrom:_keySelectedRowIndex to:firstIndex];
            NSMutableIndexSet* selectedOverlap = [[NSMutableIndexSet alloc] init];
            if (_selectedRowIndexesClip) {
                [selectedWhenDragging enumerateIndexesUsingBlock:^(NSUInteger index, BOOL *stop) {
                    if ([_selectedRowIndexesClip containsIndex:index])
                        [selectedOverlap addIndex:index];
                }];
                _selectedRowIndexes = [[NSMutableIndexSet alloc] initWithIndexSet:_selectedRowIndexesClip];
                [_selectedRowIndexes addIndexes:selectedWhenDragging];
                [_selectedRowIndexes removeIndexes:selectedOverlap];
            }
            else {
                [_selectedRowIndexes addIndexes:selectedWhenDragging];
            }
        }
        else {
            _keySelectedRowIndex = firstIndex;
            _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
        }
    }
    
    [self tintedGridRowViews];
    [_verticalScroller setFloatValue:[self verticalScrollerOffset]];
}

- (void)scrollDownWithSelection:(BOOL)select {
    if (_firstRowIndex >= (self.numberOfRows - _grid.rowViews.count))
        return;
    
    _firstRowIndex ++;
    [self updateGrid];
    
    if (select) {
        NSInteger lastIndex = [[_grid.rowViews lastObject] index];
        if (_allowsMultipleSelection) {
            NSMutableIndexSet* selectedWhenDragging = [self selectionFrom:_keySelectedRowIndex to:lastIndex];
            NSMutableIndexSet* selectedOverlap = [[NSMutableIndexSet alloc] init];
            if (_selectedRowIndexesClip) {
                [selectedWhenDragging enumerateIndexesUsingBlock:^(NSUInteger index, BOOL *stop) {
                    if ([_selectedRowIndexesClip containsIndex:index])
                        [selectedOverlap addIndex:index];
                }];
                _selectedRowIndexes = [[NSMutableIndexSet alloc] initWithIndexSet:_selectedRowIndexesClip];
                [_selectedRowIndexes addIndexes:selectedWhenDragging];
                [_selectedRowIndexes removeIndexes:selectedOverlap];
            }
            else {
                [_selectedRowIndexes addIndexes:selectedWhenDragging];
            }
        }
        else {
            _keySelectedRowIndex = lastIndex;
            _selectedRowIndexes = [self selectionFrom:_keySelectedRowIndex to:_keySelectedRowIndex];
        }
    }
    
    [self tintedGridRowViews];
    [_verticalScroller setFloatValue:[self verticalScrollerOffset]];
}

- (void)scrollLeft {
    CGFloat diff = [self columnsWidth] - self.frame.size.width;
    if (diff > 0.0) {
        if (fabs(1.0 - _horizontalScroller.knobProportion) > FLT_EPSILON &&
            _horizontalScroller.floatValue > FLT_EPSILON) {
            _horizontalScroller.floatValue -= 0.2;
            if (_horizontalScroller.floatValue < 0.0)
                _horizontalScroller.floatValue = 0.0;
            [_header setColumnOffsets:[self columnOffsets]];
            [_grid setColumnOffsets:[self columnOffsets]];
        }
    }
}

- (void)scrollRight {
    CGFloat diff = [self columnsWidth] - self.frame.size.width;
    if (diff > 0.0) {
        if (fabs(1.0 - _horizontalScroller.knobProportion) > FLT_EPSILON &&
            fabs(1.0 - _horizontalScroller.floatValue) > FLT_EPSILON) {
            _horizontalScroller.floatValue += 0.2;
            if (_horizontalScroller.floatValue > 1.0)
                _horizontalScroller.floatValue = 1.0;
            [_header setColumnOffsets:[self columnOffsets]];
            [_grid setColumnOffsets:[self columnOffsets]];
        }
    }
}

- (NSInteger)selectedRow {
    if (!_selectedRowIndexes.count)
        return -1;
    return [_selectedRowIndexes firstIndex];
}

- (void)scrollRowToVisible:(NSInteger)row {
    NSUInteger numberOfRowView = _grid.rowViews.count;
    NSUInteger lastRowViewIndex = _firstRowIndex + numberOfRowView - 1;
    
    if (row >= _firstRowIndex && row <= lastRowViewIndex)
        return;
    
    if (row < _firstRowIndex)
        _firstRowIndex = row;
    else if (row > lastRowViewIndex)
        _firstRowIndex += row - lastRowViewIndex;
    
    [self updateGrid];
    [self adjustVerticalScroller];
    [self tintedGridRowViews];
}

- (void)selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend {
    if (!indexes)
        return;
    NSMutableIndexSet* indexSet = [[NSMutableIndexSet alloc] init];
    NSInteger i = [indexes firstIndex];
    while (i != NSNotFound) {
        [indexSet addIndex:i];
        i = [indexes indexGreaterThanIndex:i];
    }
    _selectedRowIndexes = [[NSMutableIndexSet alloc] initWithIndexSet:indexes];
    [self tintedGridRowViews];
}

- (CGFloat)rowHeight {
    return _grid.rowHeight;
}

- (void)setFirstRowIndex:(NSInteger)firstRowIndex {
    _firstRowIndex = firstRowIndex;
    [self reloadData];
}

- (NSInteger)firstRowIndex {
    return _firstRowIndex;
}

- (void)setStyleState:(CEEViewStyleState)state {
    _styleState = state;
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    [_verticalScroller setStyleState:state];
    [_horizontalScroller setStyleState:state];
    [_header setStyleState:state];
    [_headerPadding setStyleState:state];
    [self tintedGridRowViews];
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
    self.cornerRadius = current.cornerRadius;
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration *)configuration {
    [super setStyleConfiguration:configuration];
    [_header setStyleConfiguration:configuration];
    [_verticalScroller setStyleConfiguration:configuration];
    [_horizontalScroller setStyleConfiguration:configuration];
    [_grid setStyleConfiguration:configuration];
    
    for (CEEGridRowView* view in _rowViewBuffer)
        [view setStyleConfiguration:configuration];
    
    for (CEEView* view in _cellViewBuffer)
        [view setStyleConfiguration:configuration];
}

#pragma mark - protocol NSDraggingDestination

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return NSDragOperationNone;
    
    if (!self.dataSource)
        return NSDragOperationNone;
    
    if ([self.dataSource respondsToSelector:@selector(tableView:validateDrop:proposedRow:proposedDropOperation:)])
        return [self.dataSource tableView:self validateDrop:sender proposedRow:(_firstRowIndex + _grid.rowViews.count - 1)  proposedDropOperation:NSTableViewDropOn];
    
    return NSDragOperationNone;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return NSDragOperationNone;
    
    if (!self.dataSource)
        return NSDragOperationNone;
    
    return NSDragOperationMove;
}

- (void)draggingExited:(nullable id<NSDraggingInfo>)sender {
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return NO;
    
    if (!self.dataSource)
        return NO;
    
    if ([self.dataSource respondsToSelector:@selector(tableView:acceptDrop:row:dropOperation:)])
        return [self.dataSource tableView:self acceptDrop:sender row:(_firstRowIndex + _grid.rowViews.count - 1) dropOperation:NSTableViewDropOn];
    
    return NO;
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return;
    return;
}

#pragma mark - protocol NSDraggingSource

- (void)setDraggingSourceOperationMask:(NSDragOperation)mask forLocal:(BOOL)isLocal {
    
}

- (void)draggingSession:(NSDraggingSession *)session willBeginAtPoint:(NSPoint)screenPoint {
    NSArray* filePaths = [_draggingSessionPasteboard propertyListForType:NSFilenamesPboardType];
    [[session draggingPasteboard] setPropertyList:filePaths forType:NSFilenamesPboardType];
}

- (void)draggingSession:(NSDraggingSession *)session endedAtPoint:(NSPoint)screenPoint operation:(NSDragOperation)operation {
    [[session draggingPasteboard]  clearContents];
}

- (NSDragOperation)draggingSession:(NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
    switch(context) {
        case NSDraggingContextOutsideApplication:
            return NSDragOperationCopy;
            break;
            
        case NSDraggingContextWithinApplication:
        default:
            return NSDragOperationGeneric;
            break;
    }
    return NSDragOperationGeneric;
}

#pragma mark - protocol NSPasteboardWriting
- (NSArray<NSPasteboardType> *)writableTypesForPasteboard:(NSPasteboard *)pasteboard {
    return @[NSPasteboardTypeFileURL];
}

- (NSPasteboardWritingOptions)writingOptionsForType:(NSPasteboardType)type pasteboard:(NSPasteboard *)pasteboard {
    return NSPasteboardWritingPromised;
}

- (nullable id)pasteboardPropertyListForType:(NSPasteboardType)type {
    return nil;
}

@end
