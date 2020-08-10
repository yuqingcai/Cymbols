//
//  NSControl+UIStyle.m
//  Cymbols
//
//  Created by caiyuqing on 2019/9/26.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "NSControl+UIStyle.h"

@implementation NSControl(UIStateable)

- (BOOL)styleSet:(CEEViewStyle)style {
    return NO;
}

- (CEEViewStyle)style {
    return kCEEViewStyleInit;
}

- (void)setStyle:(CEEViewStyle)style {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view setStyle:style];
}

- (void)clearStyle:(CEEViewStyle)style {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view clearStyle:style];
}

- (void)resetStyle:(CEEViewStyle)style {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view resetStyle:style];
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view setStyleConfiguration:configuration];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return nil;
}

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
}

@end
