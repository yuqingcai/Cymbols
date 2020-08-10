//
//  CEERenderBuffer.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/19.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "cee_lib.h"
#import "CEESourceBuffer.h"
#import "cee_text_layout.h"

NS_ASSUME_NONNULL_BEGIN
void render_buffer_tags_generate(cee_pointer generator,
                                 CEETextLayoutRef layout,
                                 CEERange range);

@interface CEERenderBuffer : NSObject
@property (weak) CEESourceBuffer* sourceBuffer;
- (instancetype)initWithSourceBuffer:(CEESourceBuffer*)sourceBuffer;
- (void)selectOffset:(cee_long)offset;
@end

NS_ASSUME_NONNULL_END
