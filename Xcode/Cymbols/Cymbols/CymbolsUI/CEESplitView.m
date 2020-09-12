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
    _dividerColorVertical = [NSColor gridColor];
    _dividerColorHorizontal = [NSColor gridColor];
    _borderColor = [NSColor clearColor];
    _borderWidth = 0.0;
    _cornerRadius = 0.0;
    _styleState = kCEEViewStyleStateActived;
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
    
    if (rect.size.height > self.dividerThickness)
        [self.dividerColorVertical set];
    else
        [self.dividerColorHorizontal set];
    
    NSRectFill(rect);
}

- (CGFloat)dividerThickness {
    return 1.0;
}

- (void)setStyleState:(CEEViewStyleState)state {
    _styleState = state;
    [super setStyleState:state];
}

- (CEEViewStyleState)styleState {
    return _styleState;
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    _styleConfiguration = configuration;
    [configuration configureView:self];
    [super setStyleConfiguration:configuration];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return _styleConfiguration;
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
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
    
    self.dividerColorVertical = current.dividerColorVertical;
    self.dividerColorHorizontal = current.dividerColorHorizontal;
    self.borderColor = current.borderColor;
    self.borderWidth = current.borderWidth;
    self.cornerRadius = current.cornerRadius;
}

- (void)setSytleSchemes:(NSArray*)schemes {
    self.userInterfaceStyles = [[NSPointerArray alloc] init];
    
    for (NSUInteger i = 0; i < kCEEViewStyleStateMax; i ++)
        [self.userInterfaceStyles addPointer:NULL];
    
    for (CEEUserInterfaceStyleScheme* scheme in schemes) {
        CEEUserInterfaceStyle* style = [[CEEUserInterfaceStyle alloc] initWithScheme:scheme];
        [self.userInterfaceStyles replacePointerAtIndex:scheme.styleState withPointer:(void*)style];
    }
}

@end
