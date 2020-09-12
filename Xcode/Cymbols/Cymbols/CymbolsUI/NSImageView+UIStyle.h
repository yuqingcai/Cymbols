//
//  NSImageView+UIStateable.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/26.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSView+UIStyle.h"

NS_ASSUME_NONNULL_BEGIN

@interface NSImageView(UIStyle)
- (void)setStyleState:(CEEViewStyleState)state;
- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration;
- (CEEUserInterfaceStyleConfiguration*)styleConfiguration;
- (void)updateUserInterface;
- (void)setSytleSchemes:(NSArray*)schemes;
@end

NS_ASSUME_NONNULL_END
