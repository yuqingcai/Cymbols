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
    _styleState = kCEEViewStyleStateActived;
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
