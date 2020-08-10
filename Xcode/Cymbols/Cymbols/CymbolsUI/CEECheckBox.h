//
//  CEECheckBox.h
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEButton.h"

NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE

@interface CEECheckBox : CEEButton
@property (strong, nullable) IBInspectable NSColor* boxOutlineColor;
@property (strong, nullable) IBInspectable NSColor* boxBackgroundColor;
@property (strong, nullable) IBInspectable NSColor* boxBackgroundColorChecked;
@property (strong, nullable) IBInspectable NSColor* boxContentColor;
- (void)setChecked:(BOOL)checked;
- (BOOL)checked;
@end

NS_ASSUME_NONNULL_END
