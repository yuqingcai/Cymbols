//
//  CEESessionFrameView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/29.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"

NS_ASSUME_NONNULL_BEGIN

@class CEESessionFrameView;

@protocol CEESessionFrameViewDelegate <NSObject>
- (BOOL)frameViewAttachable:(CEESessionFrameView*)frameView;
- (void)deriveFrameViewWithFilePaths:(NSArray*)filePaths attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameView*)frameView;
- (void)moveFrameView:(CEESessionFrameView*)frameView0 attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameView*)frameView1;
- (CEEView*)titleViewInFrameView:(CEESessionFrameView*)frameView;
- (NSString*)titleOfFrameView:(CEESessionFrameView*)frameView;
@end

@interface CEESessionFrameView : CEEView <NSPasteboardWriting, NSDraggingDestination, NSDraggingSource>
@property (weak) id<CEESessionFrameViewDelegate> delegate;
@end

NS_ASSUME_NONNULL_END
