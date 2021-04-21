//
//  CEEImageView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/7.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEImageView.h"


@implementation CEEImageView


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

- (void)initProperties {
    _tintColor = nil;
    _styleState = kCEEViewStyleStateActived;
    
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

- (void)drawRect:(NSRect)dirtyRect {
    if (_tintColor)
        self.image = [self tintedImage:self.image withColor:_tintColor];
    [super drawRect:dirtyRect];
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

- (void)updateUserInterface {
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
    
    if (current.tintColor)
        self.tintColor = current.tintColor;
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
