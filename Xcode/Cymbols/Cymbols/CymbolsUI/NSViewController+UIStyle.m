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

- (void)setViewStyleState:(CEEViewStyleState)state {
    [self.view setStyleState:state];
}

- (void)setViewStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    [self.view setStyleConfiguration:configuration];
}

- (void)setViewStyleConfigurationComplete {
    [self.view setStyleConfigurationComplete];
}
@end
