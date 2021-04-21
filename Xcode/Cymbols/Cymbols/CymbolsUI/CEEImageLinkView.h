//
//  CEEImageLinkView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/7.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEImageView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEImageLinkView : CEEImageView
@property (strong) IBInspectable NSString* URLString;
- (void)initProperties;
@end

NS_ASSUME_NONNULL_END
