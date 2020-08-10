//
//  CEEButton.h
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEControl.h"

NS_ASSUME_NONNULL_BEGIN

IB_DESIGNABLE

@interface CEEButton : CEEControl
@property (strong, nullable) IBInspectable NSColor* backgroundColor;
@property (strong, nullable) IBInspectable NSColor* borderColor;
@property (strong, nullable) IBInspectable NSColor* textColor;
@property (strong, nullable) IBInspectable NSShadow* textShadow;
@property (strong, nullable) IBInspectable NSGradient* gradient;
@property IBInspectable CGFloat gradientAngle;
@property (strong, nullable) IBInspectable NSString* borders;
@property IBInspectable CGFloat borderWidth;
@property IBInspectable CGFloat cornerRadius;
@property (strong, nullable) IBInspectable NSString* title;
@property (strong, nullable) IBInspectable NSImage* icon;
@property (strong, nullable) IBInspectable NSColor* iconColor;
@end

NS_ASSUME_NONNULL_END
