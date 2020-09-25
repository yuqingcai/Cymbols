//
//  NSView+UIStyle.m
//  Cymbols
//
//  Created by caiyuqing on 2019/9/26.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "NSView+UIStyle.h"
#include "CEEIdentifier.h"

@implementation CEEUserInterfaceStyleScheme
@end

@implementation CEEUserInterfaceStyle

- (instancetype)initWithScheme:(CEEUserInterfaceStyleScheme*)scheme {
    self = [super init];
    if (!self)
        return nil;
    
    NSString* fontProperty = scheme.descriptor[@"font"];
    NSString* backgroundColorProperty = scheme.descriptor[@"background_color"];
    NSString* borderColorProperty = scheme.descriptor[@"border_color"];
    NSString* textColorProperty = scheme.descriptor[@"text_color"];
    NSString* textShadowProperty = scheme.descriptor[@"text_shadow"];
    NSString* gradientProperty = scheme.descriptor[@"gradient"];
    NSString* gradientAngleProperty = scheme.descriptor[@"gradient_angle"];
    NSString* bordersProperty = scheme.descriptor[@"borders"];
    NSString* borderWidthProperty = scheme.descriptor[@"border_width"];
    NSString* cornerRadiusProperty = scheme.descriptor[@"corner_radius"];
    NSString* caretColorProperty = scheme.descriptor[@"caret_color"];
    NSString* caretColorMarkedProperty = scheme.descriptor[@"caret_color_marked"];
    NSString* textBackgroundColorSelectedProperty = scheme.descriptor[@"text_background_color_selected"];
    NSString* textBackgroundColorSelectedOutlineProperty = scheme.descriptor[@"text_background_color_selected_outline"];
    NSString* textBackgroundColorHighlightProperty = scheme.descriptor[@"text_background_color_highlight"];
    NSString* textBackgroundColorHighlightOutlineProperty = scheme.descriptor[@"text_background_color_highlight_outline"];
    NSString* aligmentProperty = scheme.descriptor[@"aligment"];
    NSString* rowSpacingProperty = scheme.descriptor[@"row_spacing"];
    NSString* gridColorProperty = scheme.descriptor[@"grid_color"];
    NSString* gridWidthProperty = scheme.descriptor[@"grid_width"];
    NSString* enableDrawHorizontalGridProperty = scheme.descriptor[@"enable_draw_horizontal_grid"];
    NSString* enableDrawVerticalGridProperty = scheme.descriptor[@"enable_draw_vertical_grid"];
    NSString* knobColorProperty = scheme.descriptor[@"knob_color"];
    NSString* iconColorProperty = scheme.descriptor[@"icon_color"];
    NSString* boxOutlineColorProperty = scheme.descriptor[@"box_outline_color"];
    NSString* boxBackgroundColorProperty = scheme.descriptor[@"box_background_color"];
    NSString* boxBackgroundColorCheckedProperty = scheme.descriptor[@"box_background_color_checked"];
    NSString* boxContentColorProperty = scheme.descriptor[@"box_content_color"];
    NSString* tintColorProperty = scheme.descriptor[@"tint_color"];
    NSString* dividerColorProperty = scheme.descriptor[@"divider_color"];
    NSString* dividerColorVerticalProperty = scheme.descriptor[@"divider_color_vertical"];
    NSString* dividerColorHorizontalProperty = scheme.descriptor[@"divider_color_horizontal"];
    
    if (fontProperty)
        self.font = [CEEUserInterfaceStyleConfiguration createFontFromString:fontProperty];
    
    if (backgroundColorProperty)
        self.backgroundColor = [CEEUserInterfaceStyleConfiguration createColorFromString:backgroundColorProperty];
    
    if (borderColorProperty)
        self.borderColor = [CEEUserInterfaceStyleConfiguration createColorFromString:borderColorProperty];
    
    if (textColorProperty)
        self.textColor = [CEEUserInterfaceStyleConfiguration createColorFromString:textColorProperty];
    
    if (textShadowProperty)
        self.textShadow = [CEEUserInterfaceStyleConfiguration createShadowFromString:textShadowProperty];
    
    if (gradientProperty)
        self.gradient = [CEEUserInterfaceStyleConfiguration createGradientFromString:gradientProperty];
    
    if (gradientAngleProperty)
        self.gradientAngle = [gradientAngleProperty floatValue];
    
    if (bordersProperty)
        self.borders = [NSString stringWithString:bordersProperty];
    
    if (borderWidthProperty)
        self.borderWidth = [borderWidthProperty floatValue];
    
    if (cornerRadiusProperty)
        self.cornerRadius = [cornerRadiusProperty floatValue];

    if (caretColorProperty)
        self.caretColor = [CEEUserInterfaceStyleConfiguration createColorFromString:caretColorProperty];
    
    if (caretColorMarkedProperty)
        self.caretColorMarked = [CEEUserInterfaceStyleConfiguration createColorFromString:caretColorMarkedProperty];
    
    if (textBackgroundColorSelectedProperty)
        self.textBackgroundColorSelected = [CEEUserInterfaceStyleConfiguration createColorFromString:textBackgroundColorSelectedProperty];
    
    if (textBackgroundColorSelectedOutlineProperty)
        self.textBackgroundColorSelectedOutline = [CEEUserInterfaceStyleConfiguration createColorFromString:textBackgroundColorSelectedOutlineProperty];
    
    if (textBackgroundColorHighlightProperty)
        self.textBackgroundColorHighlight = [CEEUserInterfaceStyleConfiguration createColorFromString:textBackgroundColorHighlightProperty];
    
    if (textBackgroundColorHighlightOutlineProperty)
        self.textBackgroundColorHighlightOutline = [CEEUserInterfaceStyleConfiguration createColorFromString:textBackgroundColorHighlightOutlineProperty];
    
    if (aligmentProperty)
        self.aligment = aligmentProperty;

    if (rowSpacingProperty)
        self.rowSpacing = [rowSpacingProperty floatValue];
    
    if (gridColorProperty)
        self.gridColor = [CEEUserInterfaceStyleConfiguration createColorFromString:gridColorProperty];
    
    if (gridWidthProperty)
        self.gridWidth = [gridWidthProperty floatValue];
    
    if (enableDrawHorizontalGridProperty)
        self.enableDrawHorizontalGrid = [enableDrawHorizontalGridProperty boolValue];
        
    if (enableDrawVerticalGridProperty)
        self.enableDrawVerticalGrid = [enableDrawVerticalGridProperty boolValue];

    if (knobColorProperty)
        self.knobColor = [CEEUserInterfaceStyleConfiguration createColorFromString:knobColorProperty];

    if (iconColorProperty)
        self.iconColor = [CEEUserInterfaceStyleConfiguration createColorFromString:iconColorProperty];
    
    if (boxOutlineColorProperty)
        self.boxOutlineColor = [CEEUserInterfaceStyleConfiguration createColorFromString:boxOutlineColorProperty];
    
    if (boxBackgroundColorProperty)
        self.boxBackgroundColor = [CEEUserInterfaceStyleConfiguration createColorFromString:boxBackgroundColorProperty];
    
    if (boxBackgroundColorCheckedProperty)
        self.boxBackgroundColorChecked = [CEEUserInterfaceStyleConfiguration createColorFromString:boxBackgroundColorCheckedProperty];
    
    if (boxContentColorProperty)
        self.boxContentColor = [CEEUserInterfaceStyleConfiguration createColorFromString:boxContentColorProperty];

    if (tintColorProperty)
        self.tintColor = [CEEUserInterfaceStyleConfiguration createColorFromString:tintColorProperty];
    
    if (dividerColorProperty)
        self.dividerColor = [CEEUserInterfaceStyleConfiguration createColorFromString:dividerColorProperty];
        
    if (dividerColorVerticalProperty)
        self.dividerColorVertical = [CEEUserInterfaceStyleConfiguration createColorFromString:dividerColorVerticalProperty];
    
    if (dividerColorHorizontalProperty)
        self.dividerColorHorizontal = [CEEUserInterfaceStyleConfiguration createColorFromString:dividerColorHorizontalProperty];
        
    return self;
}
@end

@implementation NSView(UIStyle)

- (void)setStyleState:(CEEViewStyleState)state {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view setStyleState:state];
}

- (CEEViewStyleState)styleState {
    return kCEEViewStyleStateActived;
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view setStyleConfiguration:configuration];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return nil;
}

- (NSString*)getSchemeObjectIdentifier {
    NSString* identifier = nil;
    
    // try to get the view identifier
    if ([self conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
        identifier = [(id<NSUserInterfaceItemIdentification>)self identifier];
        if (identifier) {
            // it's a reserved Identifier, ignore
            if (IdentifierIsReserved(identifier))
                identifier = nil;
            else
                identifier = [identifier stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
        }
    }
    return identifier;
}

- (NSString*)getSchemeClassIdentifier {
    NSString* identifier = [self className];
    if ([[identifier substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        identifier = [identifier stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"Class"];
    return identifier;
}

- (NSString*)createComponentIdentifier:(NSString*)identifier {
    NSString* componentIdentifier = nil;
    NSString* mainIdentifier = nil;
    if (!self.identifier || IdentifierIsReserved(self.identifier))
        mainIdentifier = CreateClassID(self);
    else
        mainIdentifier = self.identifier;
    
    componentIdentifier = [mainIdentifier stringByAppendingFormat:@".%@", identifier];
    componentIdentifier = CreateObjectIDWithPrefix(componentIdentifier);
    
    return componentIdentifier;
}

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
}

@end
