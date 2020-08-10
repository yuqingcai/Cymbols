//
//  CEEStateButton.h
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEButton.h"

NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE

@interface CEEStateButton : CEEButton
- (void)setPushed:(BOOL)pushed;
- (BOOL)pushed;
@end

NS_ASSUME_NONNULL_END
