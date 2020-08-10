//
//  NSViewController+UIStyle.h
//  Cymbols
//
//  Created by qing on 2020/1/14.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSView+UIStyle.h"

NS_ASSUME_NONNULL_BEGIN

@interface NSViewController(UIStyle)
- (void)setViewStyle:(CEEViewStyle)style;
- (void)clearViewStyle:(CEEViewStyle)style;
@end

NS_ASSUME_NONNULL_END
