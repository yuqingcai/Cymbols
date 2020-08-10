//
//  NSView+UIStyle.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/26.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEStyleManager.h"

NS_ASSUME_NONNULL_BEGIN


typedef NS_OPTIONS(NSUInteger, CEEViewStyle) {
    kCEEViewStyleInit = 0,
    kCEEViewStyleSelected = 1 << 0,
    kCEEViewStyleActived = 1 << 1 ,
    kCEEViewStyleClicked = 1 << 2,
    kCEEViewStyleDisabled = 1 << 3,
    kCEEViewStyleHeighLighted = 1 << 4,
    kCEEViewStyleMax = 1 << 5,
};

@interface CEEUserInterfaceStyleScheme : NSObject
@property CEEViewStyle style;
@property (strong) NSDictionary* descriptor;
@end

@interface NSView(UIStyle)
- (BOOL)styleSet:(CEEViewStyle)style;
- (void)setStyle:(CEEViewStyle)style;
- (void)clearStyle:(CEEViewStyle)style;
- (void)resetStyle:(CEEViewStyle)style;
- (CEEViewStyle)style;
- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration;
- (CEEUserInterfaceStyleConfiguration*)styleConfiguration;
- (NSString*)getSchemeIdentifier;
- (NSString*)getSchemeClassName;
- (NSString*)createComponentIdentifier:(NSString*)identifier;
- (void)updateUserInterface;
- (void)setSytleSchemes:(NSArray*)schemes;
@end

NS_ASSUME_NONNULL_END
