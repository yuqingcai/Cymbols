//
//  CEEPopupTableView.m
//  Cymbols
//
//  Created by qing on 2020/7/8.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEComboBox.h"
#import "CEEPopupPanel.h"
#import "CEETableView.h"
#import "CEETextTableCellView.h"

static CEEPopupPanel* gPopupPanel;
static CEETableView* gTableView;
static BOOL popupPanelStyleConfigurationSet = YES;

@interface CEEComboBox()
+ (CEEPopupPanel*)sharedPopupPanel;
@property BOOL shouldPopupedWhenMouseUp;
@property (strong) NSMutableArray* items;
@end

@implementation CEEComboBox
@synthesize nibNameOfCellView = _nibNameOfCellView;
@synthesize indexOfSelectedItem = _indexOfSelectedItem;

+ (CEEPopupPanel*)sharedPopupPanel {
    if (!gPopupPanel) {
        gPopupPanel = [[CEEPopupPanel alloc] initWithContentRect:NSMakeRect(0.0, 0.0, 100.0, 100.0) styleMask:NSWindowStyleMaskBorderless backing:NSBackingStoreBuffered defer:NO];
        [gPopupPanel.contentView setIdentifier:[CEEView createComponentIdentifier:@"IDPanelView" withPrefix:@"ClassComboBox"]];
        [gPopupPanel setHasShadow:YES];
        
        gTableView = [[CEETableView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
        [gTableView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [gTableView setIdentifier:[gPopupPanel.contentView createComponentIdentifier:@"IDTableView"]];
        [gTableView setEnableDrawHeader:NO];
        
        [gPopupPanel.contentView addSubview:gTableView];
    }
    return gPopupPanel;
}

- (void)initProperties {
    [super initProperties];
    [CEEComboBox sharedPopupPanel];
    _isEnabled = YES;
    _shouldPopupedWhenMouseUp = YES;
    _indexOfSelectedItem = -1;
}

- (void)mouseUp:(NSEvent *)event {
    [super mouseUp:event];

    if (!_shouldPopupedWhenMouseUp) {
        _shouldPopupedWhenMouseUp = YES;
        return;
    }

    if (_shouldPopupedWhenMouseUp) {
        CEEPopupPanel* popupPanel = [CEEComboBox sharedPopupPanel];
        
        [popupPanel setPopupDelegate:self];
        [gTableView setDelegate:self];
        [gTableView setDataSource:self];
        [gTableView setTarget:self];
        [gTableView setAction:@selector(selectRow:)];
        [gTableView setStyleConfiguration:_styleConfiguration];
        [gTableView reloadData];
        
        NSIndexSet *indexSet = [[NSIndexSet alloc] initWithIndex:_indexOfSelectedItem];
        [gTableView selectRowIndexes:indexSet byExtendingSelection:NO];
        
        NSPoint point = [self convertPoint:self.frame.origin toView:nil];
        point = [self.window convertPointToScreen:point];
        CGFloat width = self.frame.size.width;
        
        CGFloat rowHeight = [gTableView rowHeight];
        id<CEETableViewDataSource> dataSource = gTableView.dataSource;
        CGFloat height = rowHeight * [dataSource numberOfRowsInTableView:gTableView];
        if (height < FLT_EPSILON)
            height = CEEPopupPanelSizeMinimumHeight;
        
        NSRect rect = NSMakeRect(point.x, point.y, width , height);
        rect.origin.y -= height - self.borderWidth;
        rect.origin.x -= self.frame.origin.x;
        [popupPanel setFrame:rect display:YES];
        [popupPanel orderFront:self.window];
    }
}

- (void)closePopupPanel:(CEEPopupPanel*)popupPanel {
    NSEvent* event = [NSApp currentEvent];
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    if ([self mouse:location inRect:[self bounds]])
        _shouldPopupedWhenMouseUp = NO;
    else
        _shouldPopupedWhenMouseUp = YES;
}

- (void)addItem:(NSString *)item {
    if (!_items)
        _items = [[NSMutableArray alloc] init];
    [_items addObject:item];
}

- (void)drawRect:(NSRect)dirtyRect {
    NSFont* font = self.font;
    NSColor* backgroundColor = self.backgroundColor;
    NSColor* borderColor = self.borderColor;
    NSColor* iconColor = self.iconColor;
    NSColor* textColor = self.textColor;
    NSShadow* textShadow = self.textShadow;
    NSGradient* gradient = self.gradient;
    CGFloat gradientAngle = self.gradientAngle;
    CGFloat borderWidth = self.borderWidth;
    CGFloat cornerRadius = self.cornerRadius;
        
    if (!_isEnabled) {
        CEEUserInterfaceStyle* disableStyle = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:kCEEViewStyleStateDisabled];
        if (disableStyle) {
            if (disableStyle.font)
                font = disableStyle.font;
            
            if (disableStyle.backgroundColor)
                backgroundColor = disableStyle.backgroundColor;
            
            if (disableStyle.borderColor)
                borderColor = disableStyle.borderColor;
            
            if (disableStyle.textColor)
                textColor = disableStyle.textColor;
            
            if (disableStyle.iconColor)
                iconColor = disableStyle.iconColor;
            
            if (disableStyle.textShadow)
                textShadow = disableStyle.textShadow;
            
            if (disableStyle.gradient)
                gradient = disableStyle.gradient;
            
            gradientAngle = disableStyle.gradientAngle;
            borderWidth = disableStyle.borderWidth;
            cornerRadius = disableStyle.cornerRadius;
        }
    }
        
    NSSize frameSize = self.frame.size;
    NSRect rect = NSMakeRect((borderWidth / 2.0),
                             (borderWidth / 2.0),
                             frameSize.width - borderWidth,
                             frameSize.height - borderWidth);
    NSRect captionRect = NSMakeRect(rect.origin.x + 6.0,
                                    rect.origin.y,
                                    rect.size.width - rect.size.height,
                                    rect.size.height);
    NSRect arrowRect = NSMakeRect(rect.size.width - rect.size.height,
                                  rect.origin.y,
                                  rect.size.height,
                                  rect.size.height);
        
    NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:cornerRadius yRadius:cornerRadius];
    
    if (backgroundColor) {
        [backgroundColor setFill];
        [path fill];
    }
    
    if (borderWidth >= FLT_EPSILON) {
        [path setLineWidth:borderWidth];
        if (borderColor) {
            path = [[NSBezierPath alloc] init];
            [path moveToPoint:rect.origin];
            [path lineToPoint:NSMakePoint(rect.origin.x + rect.size.width,
                                          rect.origin.y)];
            [borderColor setStroke];
            [path stroke];
        }
    }
    
    path = [[NSBezierPath alloc] init];
    NSPoint p0 = NSMakePoint(arrowRect.origin.x + arrowRect.size.width * 0.5,
                            arrowRect.origin.y + arrowRect.size.height * 0.5 - 2.0);
    NSPoint p1 = NSMakePoint(p0.x - 4.0, p0.y + 4.0);
    NSPoint p2 = NSMakePoint(p0.x + 4.0, p0.y + 4.0);
    
    [path moveToPoint:p1];
    [path lineToPoint:p0];
    [path lineToPoint:p2];
    [path setLineWidth:1.0];
    [iconColor setStroke];
    [path stroke];
    
    NSString* caption = nil;
    if (_indexOfSelectedItem != -1)
        caption = _items[_indexOfSelectedItem];
    
    if (!caption)
        return;
    
    NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
    [attributes setValue:font forKey:NSFontAttributeName];
    [attributes setValue:textColor forKey:NSForegroundColorAttributeName];
    
    if (textShadow)
        [attributes setValue:textShadow forKey:NSShadowAttributeName];
    
    NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:caption attributes:attributes];
    NSSize drawingSize = [drawingString size];
    if (captionRect.size.width < 16.0)
        return;
    
    if (drawingSize.width > captionRect.size.width) {
        NSInteger subIndex = 1;
        NSString* subjectString = nil;
        while (1) {
            subjectString = [@"..." stringByAppendingString:[caption substringFromIndex:subIndex]];
            drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
            drawingSize = [drawingString size];
            if (drawingSize.width < captionRect.size.width)
                break;
            subIndex ++;
        }
    }
    
    CGFloat width = drawingSize.width;
    CGFloat height = font.ascender + font.descender;
    CGFloat x = captionRect.origin.x;
    CGFloat y = captionRect.origin.y + ((captionRect.size.height - height) / 2.0 + 0.5);
    NSRect drawRect = NSMakeRect(x, y, width, height);
    [drawingString drawWithRect:drawRect options:0 context:nil];
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    [super setStyleConfiguration:configuration];
    if (!popupPanelStyleConfigurationSet) {
        [[CEEComboBox sharedPopupPanel].contentView setStyleConfiguration:configuration];
        popupPanelStyleConfigurationSet = YES;
    }
}

- (void)setStyleConfigurationComplete {
    [super setStyleConfigurationComplete];
    popupPanelStyleConfigurationSet = NO;
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
    self.borderWidth = current.borderWidth;
    
    if (current.iconColor)
        self.iconColor = current.iconColor;
    
    self.cornerRadius = current.cornerRadius;
}

- (void)setNibNameOfCellView:(NSString *)nibNameOfCellView {
    [gTableView setNibNameOfCellView:nibNameOfCellView];
}

- (NSString*)nibNameOfCellView {
    return [gTableView nibNameOfCellView];
}

- (__kindof NSView*)makeViewWithIdentifier:(NSUserInterfaceItemIdentifier)identifier {
    return [gTableView makeViewWithIdentifier:identifier];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (_items)
        return _items.count;
    return 0;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEETextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDTextTableCellView"];
    cellView.text.stringValue = _items[row];
    return cellView;
}

- (void)selectRow:(id)sender {
    _indexOfSelectedItem = gTableView.selectedRow;
    if (_delegate)
        [_delegate selectedIndexChangedOfComboBox:self];
    [self setNeedsDisplay:YES];
}

- (void)setIndexOfSelectedItem:(NSInteger)index {
    _indexOfSelectedItem = index;
    if (_delegate)
        [_delegate selectedIndexChangedOfComboBox:self];
    [self setNeedsDisplay:YES];
}

- (NSInteger)indexOfSelectedItem {
    return _indexOfSelectedItem;
}

- (NSString*)itemAtIndex:(NSInteger)index {
    return _items[index];
}

- (void)removeAllItems {
    _items = nil;
}

@end
