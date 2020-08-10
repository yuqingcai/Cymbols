//
//  CEESourceViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/11.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEEProject.h"
#import "CEETextView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESourceViewController : CEEViewController <CEETextViewDelegate>
@property (weak) CEESessionPort* port;
@property (weak) CEESourceBuffer* buffer;
@property NSInteger paragraphIndex;
- (void)highlightRange:(CEERange)range;
@end

NS_ASSUME_NONNULL_END
