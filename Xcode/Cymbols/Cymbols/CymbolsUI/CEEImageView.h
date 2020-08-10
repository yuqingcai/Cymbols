//
//  CEEImageView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/7.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSImageView+UIStyle.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEImageView : NSImageView {
    CEEUserInterfaceStyleConfiguration* _styleConfiguration;
    CEEViewStyle _style;
}
@property NSPointerArray* styleSchemes;
@property (strong, nullable) NSColor* tintColor;
- (void)initProperties;
- (NSImage*)tintedImage:(NSImage*)image withColor:(NSColor *)tint;
@end

NS_ASSUME_NONNULL_END
