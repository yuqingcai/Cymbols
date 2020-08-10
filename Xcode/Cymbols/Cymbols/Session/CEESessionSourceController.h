//
//  CEESessionSourceController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/1.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEESessionSpliter1.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionSourceController : CEEViewController

@property (strong) CEESession* session;
- (void)setFrameAttachable:(BOOL)enable;
- (void)toggleContextView;
- (void)showContextView:(BOOL)shown;
- (void)splitViewHorizontally;
- (void)splitViewVertically;
@end

NS_ASSUME_NONNULL_END
