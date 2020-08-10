//
//  CEEOpenGLTextView.h
//  Cymbols
//
//  Created by Qing on 2018/9/7.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEETextView.h"
#include "cee_font.h"
#include "cee_color.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEOpenGLTextView : CEETextView <NSTextInputClient> 
@property CEETextAttribute* textAttribute;
@end

NS_ASSUME_NONNULL_END
