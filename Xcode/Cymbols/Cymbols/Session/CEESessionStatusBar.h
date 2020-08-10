//
//  CEESessionStatusBar.h
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionStatusBar : CEEView
@property (strong) NSColor* iconColor;
@property CGFloat leadingOffset;
@property CGFloat tailingOffset;
@property (strong) NSString* content;
@end

NS_ASSUME_NONNULL_END
