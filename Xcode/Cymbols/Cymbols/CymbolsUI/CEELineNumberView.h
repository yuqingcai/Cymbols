//
//  CEELineNumberView.h
//  Cymbols
//
//  Created by caiyuqing on 2018/12/3.
//  Copyright © 2018 caiyuqing. All rights reserved.
//

#import "CEEView.h"
#import "cee_text_edit.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEELineNumberView : CEEView
@property NSArray* _Nullable lineNumberTags;
- (void)setTextAttributesDescriptor:(NSString*)descriptor;
@end

NS_ASSUME_NONNULL_END
