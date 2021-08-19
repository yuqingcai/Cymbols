//
//  CEEEditViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/11.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEEditViewController : CEEViewController
@property (strong) CEESessionPort* _Nullable port;
@property (strong) CEESourceBuffer* _Nullable buffer;
@property BOOL intelligentPickup;
@property BOOL intelligence;
- (void)highlight:(CEEList*)ranges;
- (void)setEditable:(BOOL)flag;
- (void)setWrap:(BOOL)flag;
- (void)setPresentBufferOffset:(NSInteger)offset;
- (void)setFocusBufferOffset:(NSInteger)offset;
- (NSInteger)presentBufferOffset;
- (NSInteger)focusBufferOffset;
@end

NS_ASSUME_NONNULL_END
