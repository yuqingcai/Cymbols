//
//  CEECheckBox.h
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEStateButton.h"

NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE

@interface CEECheckBox : CEEStateButton
@property (strong) IBInspectable NSColor* boxOutlineColor;
@property (strong) IBInspectable NSColor* boxBackgroundColor;
@property (strong) IBInspectable NSColor* boxBackgroundColorChecked;
@property (strong) IBInspectable NSColor* boxContentColor;
@end

NS_ASSUME_NONNULL_END
