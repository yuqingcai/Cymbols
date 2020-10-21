//
//  CEEViewController.h
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/15.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSViewController+UIStyle.h"
#import "CEEView.h"
#import "CEESerialization.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEViewController : NSViewController <CEESerialization>
- (void)initProperties;
@end

NS_ASSUME_NONNULL_END
