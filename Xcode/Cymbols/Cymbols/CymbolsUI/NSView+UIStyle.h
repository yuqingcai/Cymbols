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


typedef NS_ENUM(NSUInteger, CEEViewStyleState) {
    kCEEViewStyleStateActived       = 0 ,
    kCEEViewStyleStateDeactived     = 1,
    kCEEViewStyleStateClicked       = 2,
    kCEEViewStyleStateDisabled      = 3,
    kCEEViewStyleStateHighLighted   = 4,
    kCEEViewStyleStateMax           = 6,
};

@interface CEEUserInterfaceStyleScheme : NSObject
@property CEEViewStyleState styleState;
@property (strong) NSDictionary* descriptor;
@end


@interface CEEUserInterfaceStyle : NSObject
@property (strong) NSFont* font;
@property (strong) NSColor* textColor;
@property (strong) NSColor* backgroundColor;
@property (strong) NSColor* borderColor;
@property (strong) NSShadow* textShadow;
@property (strong) NSGradient* gradient;
@property CGFloat gradientAngle;
@property (strong) NSString* borders;
@property CGFloat borderWidth;
@property CGFloat cornerRadius;

@property (strong) NSColor* textBackgroundColorSelected;
@property (strong) NSColor* textBackgroundColorSelectedOutline;
@property (strong) NSColor* textBackgroundColorHighlight;
@property (strong) NSColor* textBackgroundColorHighlightOutline;
@property (strong) NSString* aligment;

@property (strong) NSColor* dividerColor;
@property (strong) NSColor* dividerColorVertical;
@property (strong) NSColor* dividerColorHorizontal;
@property CGFloat rowSpacing;
@property (strong) NSColor* gridColor;
@property CGFloat gridWidth;
@property BOOL enableDrawHorizontalGrid;
@property BOOL enableDrawVerticalGrid;
@property (strong) NSColor* knobColor;
@property (strong) NSColor* iconColor;
@property (strong) NSColor* boxOutlineColor;
@property (strong) NSColor* boxBackgroundColor;
@property (strong) NSColor* boxBackgroundColorChecked;
@property (strong) NSColor* boxContentColor;
@property (strong) NSColor* tintColor;
- (instancetype)initWithScheme:(CEEUserInterfaceStyleScheme*)scheme;
@end


@interface NSView(UIStyle)
- (void)setStyleState:(CEEViewStyleState)state;
- (CEEViewStyleState)styleState;
- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration;
- (CEEUserInterfaceStyleConfiguration*)styleConfiguration;
- (NSString*)getSchemeObjectIdentifier;
- (NSString*)getSchemeClassIdentifier;
- (NSString*)createComponentIdentifier:(NSString*)identifier;
- (void)updateUserInterface;
- (void)setSytleSchemes:(NSArray*)schemes;

@end

NS_ASSUME_NONNULL_END
