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
@property (strong) CEESessionPort* _Nullable port;
@property (strong) CEESourceBuffer* _Nullable buffer;
- (void)highlightRanges:(CEEList*)ranges;
- (void)setEditable:(BOOL)flag;
- (void)setIntelligence:(BOOL)flag;
- (void)setWrap:(BOOL)flag;
- (void)setLineBufferOffset:(NSInteger)offset;
- (void)setCaretBufferOffset:(NSInteger)offset;
@end

NS_ASSUME_NONNULL_END
