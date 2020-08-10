//
//  CEETitleView.h
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"

NS_ASSUME_NONNULL_BEGIN

@class CEETitleView;


@protocol CEETitleViewDelegate <NSObject>
- (void)expandTitleViewDetail:(CEETitleView*)titleView;
- (void)collapseTitleViewDetail:(CEETitleView*)titleView;
@end

@interface CEETitleView : CEEView
@property (strong) NSColor* iconColor;
@property (weak) id<CEETitleViewDelegate> delegate;
@property CGFloat leadingOffset;
@property CGFloat tailingOffset;
@property NSString* title;
@end

NS_ASSUME_NONNULL_END
