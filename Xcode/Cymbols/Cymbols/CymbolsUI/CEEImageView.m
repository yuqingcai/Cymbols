//
//  CEEImageView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/7.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEImageView.h"

@implementation CEEImageView

- (void)initProperties {
    _tintColor = nil;
    _style = kCEEViewStyleInit;
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
    [super updateUserInterface];
    
    CEEUserInterfaceStyleScheme* current = (CEEUserInterfaceStyleScheme*)[_styleSchemes pointerAtIndex:self.style];
        
    if (!current)
        return;
    
    NSDictionary* descriptor = current.descriptor;
    NSString* tintColorProperty = descriptor[@"tint_color"];
    
    if (tintColorProperty)
        self.tintColor = [CEEUserInterfaceStyleConfiguration createColorFromString:tintColorProperty];
}

- (void)setSytleSchemes:(NSArray*)schemes {
    _styleSchemes = [[NSPointerArray alloc] init];
    
    for (NSUInteger i = 0; i < kCEEViewStyleMax; i ++)
        [_styleSchemes addPointer:NULL];
    
    for (CEEUserInterfaceStyleScheme* scheme in schemes)
        [_styleSchemes replacePointerAtIndex:scheme.style withPointer:(void*)scheme];
}

@end
