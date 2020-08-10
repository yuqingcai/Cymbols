//
//  CEETextField.h
//  Cymbols
//
//  Created by qing on 2019/12/28.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEETextView.h"

NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE

@interface CEETextField : CEETextView

@property (strong) IBInspectable NSString* stringValue;

@end

NS_ASSUME_NONNULL_END
