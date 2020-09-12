//
//  CEESessionFrameManager.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/5.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEESessionFrameSplitView.h"
#import "CEEProject.h"
#import "CEESessionFrameViewController.h"
#import "CEESessionFrameManagerView.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, CEESplitFrameDirection) {
    kCEESplitFrameDirectionVertical,
    kCEESplitFrameDirectionHorizontal,
};

@interface CEESessionFrameManager : CEEViewController <NSSplitViewDelegate, CEESessionFrameManagerViewDelegate>

@property (strong) CEESession* session;
@property BOOL frameAttachable;

- (CEESessionFrameViewController*)createFrame;
- (void)closeFrame:(CEESessionFrameViewController*)frame;
- (void)selectFrame:(CEESessionFrameViewController*)frame;
- (void)deriveFrameWithFilePaths:(NSArray*)filePaths attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameViewController*)frame;
- (void)moveFrameView:(CEESessionFrameView*)view0 attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameView*)view1;
- (void)split:(CEESplitFrameDirection)direction;

@end

NS_ASSUME_NONNULL_END
