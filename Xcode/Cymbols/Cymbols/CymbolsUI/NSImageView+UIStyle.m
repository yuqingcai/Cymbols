//
//  NSImageView+UIStyle.m
//  Cymbols
//
//  Created by caiyuqing on 2019/9/26.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "NSImageView+UIStyle.h"

@implementation NSImageView(UIStyle)

- (void)setStyleState:(CEEViewStyleState)state {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return nil;
}

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
}

@end
