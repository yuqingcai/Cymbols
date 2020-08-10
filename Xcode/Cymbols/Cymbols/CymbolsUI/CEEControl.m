//
//  CEEControl.m
//  Cymbols
//
//  Created by yuqingcai on 2019/7/12.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEControl.h"

@implementation CEEControl

- (void)initProperties {
    _style = kCEEViewStyleInit;
    [self setEnabled:YES];
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

- (void)setEnabled:(BOOL)enabled {
    if (!enabled)
        [self setStyle:kCEEViewStyleDisabled];
    else
        [self clearStyle:kCEEViewStyleDisabled];
    
    [super setEnabled:enabled];
}

- (NSImage*)tintedImage:(NSImage*)image withColor:(NSColor *)tint {
    NSImage *tintedImage = [image copy];
    if (tint) {
        [tintedImage lockFocus];
        [tint set];
        NSRect imageRect = { NSZeroPoint, [tintedImage size] };
        NSRectFillUsingOperation(imageRect, NSCompositingOperationSourceAtop);
        [tintedImage unlockFocus];
    }
    return tintedImage;
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

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    _styleSchemes = [[NSPointerArray alloc] init];
    
    for (NSUInteger i = 0; i < kCEEViewStyleMax; i ++)
        [_styleSchemes addPointer:NULL];
    
    for (CEEUserInterfaceStyleScheme* scheme in schemes)
        [_styleSchemes replacePointerAtIndex:scheme.style withPointer:(void*)scheme];
}

@end
