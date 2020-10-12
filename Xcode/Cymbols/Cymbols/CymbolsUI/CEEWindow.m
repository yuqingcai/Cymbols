//
//  CEEWindow.m
//  Cymbols
//
//  Created by caiyuqing on 2018/12/13.
//  Copyright © 2018 Lazycatdesign. All rights reserved.
//

#import <objc/runtime.h>
#import "CEEWindow.h"

@implementation CEEWindow

- (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag {
    self = [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag];
    if (!self)
        return nil;
    
    self.titlebarAppearsTransparent = YES;
    
    return self;
}

- (void)close {
    if ([self isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
    [super close];
}

@end
