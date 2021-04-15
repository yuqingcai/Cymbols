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
    NSString* alternativeTextColorProperty = scheme.descriptor[@"alternative_text_color"];
    
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
    
    if (alternativeTextColorProperty)
        self.alternativeTextColor = [CEEUserInterfaceStyleConfiguration createColorFromString:alternativeTextColorProperty];
    
    return self;
}
@end

@implementation CEEUserInterfaceStyleConfiguration

+ (NSColor*)createColorFromString:(NSString*)string {
    NSColor* color = nil;
    NSString* patternString = @"(rgba|hsba)\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    if (matches.count) {
        for (NSTextCheckingResult *match in matches) {
            if ([[string substringWithRange:[match rangeAtIndex:1]] caseInsensitiveCompare:@"rgba"] == NSOrderedSame) {
                CGFloat r = [[string substringWithRange:[match rangeAtIndex:2]] floatValue] / 256.0;
                CGFloat g = [[string substringWithRange:[match rangeAtIndex:3]] floatValue] / 256.0;
                CGFloat b = [[string substringWithRange:[match rangeAtIndex:4]] floatValue] / 256.0;
                CGFloat a = [[string substringWithRange:[match rangeAtIndex:5]] floatValue] / 100.0;
                color = [NSColor colorWithSRGBRed:r green:g blue:b alpha:a];
                
            }
            else if ([[string substringWithRange:[match rangeAtIndex:1]] caseInsensitiveCompare:@"hsba"] == NSOrderedSame) {
                CGFloat h = [[string substringWithRange:[match rangeAtIndex:2]] floatValue] / 360.0;
                CGFloat s = [[string substringWithRange:[match rangeAtIndex:3]] floatValue] / 100.0;
                CGFloat b = [[string substringWithRange:[match rangeAtIndex:4]] floatValue] / 100.0;
                CGFloat a = [[string substringWithRange:[match rangeAtIndex:5]] floatValue] / 100.0;
                color = [NSColor colorWithHue:h saturation:s brightness:b alpha:a];
                color = [color colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
            
            }
        }
    }
    else {
        patternString = @"#[0-9a-fA-F]+";
        regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
        matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
        if (matches.count) {
            unsigned int hexValue = 0;
            NSString* hexString = [string substringFromIndex:1];
            NSScanner* scanner = [NSScanner scannerWithString:hexString];
            [scanner scanHexInt:&hexValue];
            CGFloat r = (CGFloat)(((unsigned char)(hexValue >> 16)) / 256.0);
            CGFloat g = (CGFloat)(((unsigned char)(hexValue >> 8)) / 256.0);
            CGFloat b = (CGFloat)(((unsigned char)(hexValue)) / 256.0);
            color = [NSColor colorWithSRGBRed:r green:g blue:b alpha:1.0];
        }
    }
    
    return color;
}

+ (NSFont*)createFontFromString:(NSString*)string {
    NSFont* font = nil;
    NSString* patternString = @"font\\s*\\(\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*,\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    for (NSTextCheckingResult *match in matches) {
        NSString* family = [string substringWithRange:[match rangeAtIndex:1]];
        CGFloat size = [[string substringWithRange:[match rangeAtIndex:2]] floatValue];
        NSFontTraitMask traits = 0;
        NSString* style = [string substringWithRange:[match rangeAtIndex:3]];
        if ([style rangeOfString:@"bold" options:NSCaseInsensitiveSearch].location != NSNotFound)
            traits |= NSBoldFontMask;
        if ([style rangeOfString:@"italic" options:NSCaseInsensitiveSearch].location != NSNotFound)
            traits |= NSItalicFontMask;
        NSInteger weight = [[string substringWithRange:[match rangeAtIndex:4]] integerValue];
        font = [[NSFontManager sharedFontManager] fontWithFamily:family traits:traits weight:weight size:size];
    }
    return font;
}

+ (NSGradient*)createGradientFromString:(NSString*)string {
    NSGradient* gradient = nil;
    NSString* patternString = @"gradient\\s*\\(\\s*((?:rgba|hsba)\\s*\\(\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*\\))\\s*,\\s*((?:rgba|hsba)\\s*\\(\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*\\))\\s*\\)";
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    for (NSTextCheckingResult *match in matches) {
        NSColor* color0 = [CEEUserInterfaceStyleConfiguration createColorFromString:[string substringWithRange:[match rangeAtIndex:1]]];
        NSColor* color1 = [CEEUserInterfaceStyleConfiguration createColorFromString:[string substringWithRange:[match rangeAtIndex:2]]];
        gradient = [[NSGradient alloc] initWithStartingColor:color0 endingColor:color1];
    }
    return gradient;
}

+ (NSShadow*)createShadowFromString:(NSString*)string {
    NSShadow* shadow = nil;
    NSString* patternString = @"shadow\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*((?:rgba|hsba)\\s*\\(\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*\\))\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    for (NSTextCheckingResult *match in matches) {
        shadow = [[NSShadow alloc] init];
        NSColor* color = [CEEUserInterfaceStyleConfiguration createColorFromString:[string substringWithRange:[match rangeAtIndex:4]]];
        CGFloat radius = [[string substringWithRange:[match rangeAtIndex:1]] floatValue];
        CGFloat offsetX = [[string substringWithRange:[match rangeAtIndex:2]] floatValue];
        CGFloat offsetY = [[string substringWithRange:[match rangeAtIndex:3]] floatValue];
        [shadow setShadowColor:color];
        [shadow setShadowBlurRadius:radius];
        [shadow setShadowOffset:NSMakeSize(offsetX, offsetY)];
    }
    return shadow;
}

- (NSString*)selectorFromIdentifier:(NSString*)identifier {
    NSString* selector = nil;
    NSInteger location = [identifier rangeOfString:@"#"].location;
    if (location != NSNotFound)
        selector = [identifier substringWithRange:NSMakeRange(0, location)];
    else
        selector = identifier;
    selector = [selector stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    return selector;
}

- (CEEViewStyleState)getStyleFromSchemeSelector:(NSString*)selector {
    CEEViewStyleState state = kCEEViewStyleStateActived;
    NSString* descriptor = nil;
    NSString* pattern = @"([a-zA-Z0-9_]+)\\s*(\\[\\s*([a-zA-Z0-9]+)\\s*\\])?";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:pattern options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:selector options:0 range:NSMakeRange(0, [selector length])];
    if (matches.count) {
        for (NSTextCheckingResult *match in matches)
            if (match.numberOfRanges == 4)
                descriptor = [selector substringWithRange:[match rangeAtIndex:3]];
    }
    if ([descriptor caseInsensitiveCompare:@"actived"] == NSOrderedSame)
        state = kCEEViewStyleStateActived;
    if ([descriptor caseInsensitiveCompare:@"deactived"] == NSOrderedSame)
        state = kCEEViewStyleStateDeactived;
    if ([descriptor caseInsensitiveCompare:@"clicked"] == NSOrderedSame)
        state = kCEEViewStyleStateClicked;
    if ([descriptor caseInsensitiveCompare:@"disabled"] == NSOrderedSame)
        state = kCEEViewStyleStateDisabled;
    if ([descriptor caseInsensitiveCompare:@"highlighted"] == NSOrderedSame)
        state = kCEEViewStyleStateHighLighted;
    
    return state;
}

- (NSString*)getIdentifierFromSchemeSelector:(NSString*)selector {
    NSString* identifier = nil;
    NSString* pattern = @"([a-zA-Z0-9_]+)\\s*(\\[\\s*([a-zA-Z0-9]+)\\s*\\])?";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:pattern options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:selector options:0 range:NSMakeRange(0, [selector length])];
    if (matches.count) {
        for (NSTextCheckingResult *match in matches)
            identifier = [selector substringWithRange:[match rangeAtIndex:1]];
    }
    return identifier;
}

- (void)enumerateSchemes:(NSMutableArray**)schemes fromDescriptor:(NSDictionary*)descriptor withIdentifier:(NSString*)identifier {
    NSArray* identifiers = [identifier componentsSeparatedByString:@"."];
    BOOL isTail = NO;
    NSString* current = nil;
    NSString* suffix = nil;
    NSRange suffixRange;
    CEEUserInterfaceStyleScheme* scheme = nil;
    NSString* schemeIdentifier = nil;
    CEEViewStyleState state = kCEEViewStyleStateDeactived;
    
    current = [self selectorFromIdentifier:identifiers[0]];
    
    if (identifiers.count == 1) {
        isTail = YES;
        suffix = nil;
    }
    else {
        NSString* identifier0 = identifiers[0];
        suffixRange = NSMakeRange(identifier0.length + 1, identifier.length - (identifier0.length + 1));
        suffix = [identifier substringWithRange:suffixRange];
    }
    
    for (NSString* selector in descriptor.allKeys) {
        schemeIdentifier = [self getIdentifierFromSchemeSelector:selector];
        if ([schemeIdentifier compare:current options:NSLiteralSearch] == NSOrderedSame) {
            state = [self getStyleFromSchemeSelector:selector];
            if (isTail) {
                if (!*schemes)
                    *schemes = [[NSMutableArray alloc] init];
                
                scheme = [[CEEUserInterfaceStyleScheme alloc] init];
                scheme.styleState = state;
                scheme.descriptor = descriptor[selector];
                [(*schemes) addObject:scheme];
            }
            else {
                [self enumerateSchemes:schemes fromDescriptor:descriptor[selector] withIdentifier:suffix];
            }
        }
    }
}

- (void)configureView:(__kindof NSView*)view {
    if (!_descriptor)
        return;
    
    NSMutableArray* schemes = nil;
    NSString* selector = nil;
    
    selector = [view getSchemeObjectIdentifier];
    if (selector)
        [self enumerateSchemes:&schemes fromDescriptor:_descriptor withIdentifier:selector];
    
    if (!schemes) {
        selector = [view getSchemeClassIdentifier];
        [self enumerateSchemes:&schemes fromDescriptor:_descriptor withIdentifier:selector];
    }
    
    [view setSytleSchemes:schemes];
    
    return;
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

- (void)setStyleConfigurationComplete {
    for (NSView* view in self.subviews)
        [view setStyleConfigurationComplete];
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

+ (NSString*)createComponentIdentifier:(NSString*)identifier withPrefix:(NSString*)prefix {
    NSString* componentIdentifier = [prefix stringByAppendingFormat:@".%@", identifier];
    componentIdentifier = CreateObjectIDWithPrefix(componentIdentifier);
    return componentIdentifier;
}

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
}

@end
