//
//  CEEScroller.h
//
//  Created by caiyuqing on 2018/12/3.
//  Copyright © 2018 caiyuqing. All rights reserved.
//

#import "CEEControl.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, CEEScrollerDirection) {
    kCEEScrollerDirectionHorizontal  = 0,
    kCEEScrollerDirectionVertical  = 1,
};


#define CEEVerticalScrollerWidth  15.0
#define CEEHorizontalScrollerHeight  15.0

@interface CEEScroller: CEEControl
+ (CGFloat)scrollerWidthForControlSize:(NSControlSize)controlSize scrollerStyle:(NSScrollerStyle)scrollerStyle;
@property (strong) NSColor* knobColor;
@property (strong) NSColor* backgroundColor;
@property CGFloat knobProportion;
@property NSScrollerStyle scrollerStyle;
@property NSScrollerKnobStyle knobStyle;
@property CEEScrollerDirection direction;
@property CGFloat borderWidth;
@property CGFloat cornerRadius;
@end

NS_ASSUME_NONNULL_END
