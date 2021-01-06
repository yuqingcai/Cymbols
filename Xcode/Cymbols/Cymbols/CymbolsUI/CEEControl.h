//
//  CEEControl.h
//  Cymbols
//
//  Created by yuqingcai on 2019/7/12.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSControl+UIStyle.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEControl : NSControl <NSUserInterfaceItemIdentification> {
    CEEUserInterfaceStyleConfiguration* _styleConfiguration;
    CEEViewStyleState _styleState;
}
@property (strong) NSPointerArray* userInterfaceStyles;
- (void)initProperties;
- (NSImage*)tintedImage:(NSImage*)image withColor:(NSColor *)tint;
- (NSArray*)schemeColorsFromImage:(NSImage*)image;
@end

NS_ASSUME_NONNULL_END
