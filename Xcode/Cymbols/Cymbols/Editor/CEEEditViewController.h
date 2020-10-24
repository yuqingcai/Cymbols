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
@property NSInteger lineBufferOffset;
@property NSInteger caretBufferOffset;
- (void)highlightRanges:(CEEList*)ranges;
- (void)setEditable:(BOOL)flag;
- (void)setIntelligence:(BOOL)flag;
- (void)setWrap:(BOOL)flag;

@end

NS_ASSUME_NONNULL_END
