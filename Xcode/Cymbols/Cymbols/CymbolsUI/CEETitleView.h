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
@property CGFloat titleLeading;
@property CGFloat titleTailing;
@property NSString* title;
@property IBInspectable NSImage* icon;
@property CGFloat kern;
@property (weak) id<CEETitleViewDelegate> delegate;
@property (weak) CEEView* draggingSource;
@end

NS_ASSUME_NONNULL_END
