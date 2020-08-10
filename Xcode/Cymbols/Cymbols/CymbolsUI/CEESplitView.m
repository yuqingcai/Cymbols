//
//  CEESplitView.m
//  Cymbols
//
//  Created by Qing on 2018/9/13.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEESplitView.h"

@implementation CEESplitView

- (void)initProperties {
    _gridColor = [NSColor gridColor];
    _style = kCEEViewStyleInit;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    self = [super initWithCoder:decoder];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (void)drawDividerInRect:(NSRect)rect {
    [super drawDividerInRect:rect];
    [_gridColor set];
    NSRectFill(rect);
}

- (NSColor*)dividerColor {
    return _gridColor;
}

- (CGFloat)dividerThickness {
    return 1.0;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    NSSize frameSize = self.frame.size;
    if (self.borderColor && self.borderWidth > FLT_EPSILON) {
        NSRect borderRect = NSMakeRect(self.borderWidth / 2.0,
                                       self.borderWidth / 2.0,
                                       frameSize.width - self.borderWidth,
                                       frameSize.height - self.borderWidth);
        
        NSBezierPath* borderPath = [NSBezierPath bezierPathWithRoundedRect:borderRect xRadius:self.cornerRadius yRadius:self.cornerRadius];
        [self.borderColor setStroke];
        [borderPath setLineWidth: self.borderWidth];
        [borderPath stroke];
    }
    
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    _styleConfiguration = configuration;
    [configuration configureView:self];
    [super setStyleConfiguration:configuration];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return _styleConfiguration;
}

- (BOOL)styleSet:(CEEViewStyle)style {
    return (_style & style) != 0;
}

- (void)setStyle:(CEEViewStyle)style {
    _style |= style;
    [super setStyle:style];
}

- (void)clearStyle:(CEEViewStyle)style {
    _style &= ~style;
    [super clearStyle:style];
}

- (void)resetStyle:(CEEViewStyle)style {
    _style = style;
    [super resetStyle:style];
}

- (CEEViewStyle)style {
    return _style;
}

- (void)setNeedsLayout:(BOOL)needsLayout {
    [super setNeedsLayout:needsLayout];
    [self setNeedsDisplay:YES];
}

- (void)adjustSubviews {
    [super adjustSubviews];
    [self setNeedsLayout:YES];
}

- (void)updateUserInterface {
    
    [super updateUserInterface];
    
    CEEUserInterfaceStyleScheme* current = (CEEUserInterfaceStyleScheme*)[self.styleSchemes pointerAtIndex:self.style];    
    if (!current)
        return;
    
    NSDictionary* descriptor = current.descriptor;
    
    NSString* gridColorProperty = descriptor[@"grid_color"];
    NSString* borderColorProperty = descriptor[@"border_color"];
    NSString* borderWidthProperty = descriptor[@"border_width"];
    NSString* cornerRadiusProperty = descriptor[@"corner_radius"];
    
    if (gridColorProperty)
        self.gridColor = [CEEUserInterfaceStyleConfiguration createColorFromString:gridColorProperty];
    else
        self.gridColor = [NSColor gridColor];
    
    if (borderColorProperty)
        self.borderColor = [CEEUserInterfaceStyleConfiguration createColorFromString:borderColorProperty];
    
    if (borderWidthProperty)
        self.borderWidth = [borderWidthProperty floatValue];
    
    if (cornerRadiusProperty)
        self.cornerRadius = [cornerRadiusProperty floatValue];
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
    _styleSchemes = [[NSPointerArray alloc] init];
    
    for (NSUInteger i = 0; i < kCEEViewStyleMax; i ++)
        [_styleSchemes addPointer:NULL];
    
    for (CEEUserInterfaceStyleScheme* scheme in schemes)
        [_styleSchemes replacePointerAtIndex:scheme.style withPointer:(void*)scheme];
}

@end
