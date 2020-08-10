//
//  CEESessionFrameManagerView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/29.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"

NS_ASSUME_NONNULL_BEGIN

@class CEESessionFrameManagerView;

@protocol CEESessionFrameManagerViewDelegate <NSObject>
- (BOOL)frameViewAttachable:(CEESessionFrameManagerView*)frameView;
- (void)createInitFrameViewWithFilePaths:(NSArray*)filePaths;
@end

@interface CEESessionFrameManagerView : CEEView <NSPasteboardWriting, NSDraggingDestination>
@property (weak) id<CEESessionFrameManagerViewDelegate> delegate;
@end

NS_ASSUME_NONNULL_END
