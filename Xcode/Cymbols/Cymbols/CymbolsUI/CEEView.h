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
    CEEViewStyle _style;
}
@property (strong) NSTrackingArea *trackingArea;
@property NSPointerArray* styleSchemes;
@property (strong, nullable) NSFont* font;
@property (strong, nullable) NSColor* textColor;
@property (strong, nullable) NSColor* backgroundColor;
@property (strong, nullable) NSColor* borderColor;
@property (strong, nullable) NSShadow* textShadow;
@property (strong, nullable) NSGradient* gradient;
@property CGFloat gradientAngle;
@property (strong, nullable) NSString* borders;
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
