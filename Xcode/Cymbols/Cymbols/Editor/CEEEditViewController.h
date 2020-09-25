//
//  CEEEditViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/11.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEEProject.h"
#import "CEETextView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEEditViewController : CEEViewController <CEETextViewDelegate>
@property (strong) CEESessionPort* port;
@property (strong) CEESourceBuffer* buffer;
@property NSInteger offset;
@property BOOL editable;
@property BOOL intelligence;

- (void)highlightRanges:(CEEList*)ranges;
@end

NS_ASSUME_NONNULL_END
