//
//  CEETextLink.h
//  Cymbols
//
//  Created by qing on 2019/12/28.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEETextLabel.h"

NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE

@interface CEETextLink : CEETextLabel
@property (strong) IBInspectable NSString* URLString;
@end

NS_ASSUME_NONNULL_END
