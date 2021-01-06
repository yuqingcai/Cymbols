//
//  CEEView.h
//  Cymbols
//
//  Created by Qing on 2018/9/13.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSView+UIStyle.h"
#import "CEESerialization.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, CEEViewRegion) {
    kCEEViewRegionNone,
    kCEEViewRegionEntire,
    kCEEViewRegionLeft,
    kCEEViewRegionTop,
    kCEEViewRegionRight,
    kCEEViewRegionBottom,
};

@interface CEEView : NSView {
@protected
    CEEUserInterfaceStyleConfiguration* _styleConfiguration;
    CEEViewStyleState _styleState;
}
@property (strong) NSTrackingArea *trackingArea;
@property (strong) NSPointerArray* userInterfaceStyles;
@property (strong) NSFont* font;
@property (strong) NSColor* textColor;
@property (strong) NSColor* backgroundColor;
@property (strong) NSColor* borderColor;
@property (strong) NSColor* dividerColor;
@property (strong) NSShadow* textShadow;
@property (strong) NSGradient* gradient;
@property CGFloat gradientAngle;
@property (strong) NSString* borders;
@property CGFloat borderWidth;
@property CGFloat cornerRadius;

- (void)initProperties;
- (NSImage*)tintedImage:(NSImage*)image withColor:(NSColor *)tint;
- (NSImage*)createDraggingHint;
- (NSDraggingItem*)createDraggingItem;
- (void)drawString:(NSString*)string inRect:(NSRect)rect;
- (NSAttributedString*)string:(NSString*)string withAttributes:(NSDictionary*)attributes inRect:(NSRect)rect truncate:(NSLineBreakMode)mode;
- (void)addMouseTraceArea;
@end

@interface CEEHighlightView : CEEView

@end


NS_ASSUME_NONNULL_END
