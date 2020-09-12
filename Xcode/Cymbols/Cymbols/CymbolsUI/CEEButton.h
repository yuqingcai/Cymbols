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
@property (strong) IBInspectable NSColor* backgroundColor;
@property (strong) IBInspectable NSColor* borderColor;
@property (strong) IBInspectable NSColor* textColor;
@property (strong) IBInspectable NSShadow* textShadow;
@property (strong) IBInspectable NSGradient* gradient;
@property (strong) IBInspectable NSString* borders;
@property (strong) IBInspectable NSString* title;
@property (strong) IBInspectable NSImage* icon;
@property (strong) IBInspectable NSColor* iconColor;
@property IBInspectable CGFloat gradientAngle;
@property IBInspectable CGFloat borderWidth;
@property IBInspectable CGFloat cornerRadius;
@property NSControlStateValue state;
@end

NS_ASSUME_NONNULL_END
