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
@property (strong) CEESourceBuffer* _Nullable buffer;
@property NSInteger presentedLineBufferOffset;
@property NSInteger caretBufferOffset;
@property BOOL editable;
@property BOOL intelligence;
@property BOOL wrap;

- (void)highlightRanges:(CEEList*)ranges;
@end

NS_ASSUME_NONNULL_END
