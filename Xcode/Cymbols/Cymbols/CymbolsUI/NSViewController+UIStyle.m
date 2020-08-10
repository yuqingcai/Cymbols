//
//  NSViewController+UIStyle.m
//  Cymbols
//
//  Created by qing on 2020/1/14.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "NSView+UIStyle.h"
#import "NSViewController+UIStyle.h"

@implementation NSViewController(UIStyle)

- (void)setViewStyle:(CEEViewStyle)style {
    [self.view setStyle:style];
}

- (void)clearViewStyle:(CEEViewStyle)style {
    [self.view clearStyle:style];
}

@end
