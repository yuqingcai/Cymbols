//
//  CEEButton.m
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEEButton.h"

@implementation CEEButton

@synthesize title = _title;

- (void)initProperties { 
    [super initProperties];
    self.font = [NSFont systemFontOfSize:11.0];
    _backgroundColor = [NSColor controlColor];
    _borderColor = [NSColor gridColor];
    _textColor = [NSColor textColor];
    _textShadow = nil;
    _gradient = nil;
    _gradientAngle = 270.0;
    _borders = nil;
    _borderWidth = 0.0;
    _cornerRadius = 1.0;
    _title = @"";
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    NSSize frameSize = self.frame.size;
    NSRect rect = NSMakeRect((self.borderWidth / 2.0),
                             (self.borderWidth / 2.0),
                             frameSize.width - self.borderWidth,
                             frameSize.height - self.borderWidth);
    NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:self.cornerRadius yRadius:self.cornerRadius];
    
    if (self.gradient) {
        [self.gradient drawInBezierPath:path angle:self.gradientAngle];
    }
    else {
        if (self.backgroundColor) {
            [self.backgroundColor setFill];
            [path fill];
        }
    }
    
    if (self.borderWidth >= FLT_EPSILON) {
        [path setLineWidth:self.borderWidth];
        if (self.borderColor) {
            [self.borderColor setStroke];
            [path stroke];
        }
    }
    
    if ([_title compare:@""] != NSOrderedSame) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        
        [attributes setValue:self.font forKey:NSFontAttributeName];
        [attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
        
        if (self.textShadow)
            [attributes setValue:self.textShadow forKey:NSShadowAttributeName];
        
        NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:_title attributes:attributes];
        NSSize drawingSize = [drawingString size];
        NSRect rect = NSMakeRect(0.0, 0.0, self.frame.size.width, self.frame.size.height);
        
        if (drawingSize.width > rect.size.width) {
            NSInteger subIndex = 1;
            NSString* subjectString = nil;
            while (1) {
                subjectString = [@"..." stringByAppendingString:[_title substringFromIndex:subIndex]];
                drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                drawingSize = [drawingString size];
                if (drawingSize.width < rect.size.width)
                    break;
                subIndex ++;
            }
        }
        
        CGFloat width = drawingSize.width;
        CGFloat height = self.font.ascender + self.font.descender;
        CGFloat x = (rect.size.width - drawingSize.width) / 2.0;
        CGFloat y = (rect.size.height - height) / 2.0;
        NSRect drawRect = NSMakeRect(x, y, width, height);
        [drawingString drawWithRect:drawRect options:0 context:nil];
    }
    
    if (_icon) {
        _icon = [self tintedImage:_icon withColor:_iconColor];
        CGFloat iconWidth = _icon.size.width;
        CGFloat iconHeight = _icon.size.height;
        CGRect r0 = CGRectMake(0, 0, iconWidth, iconHeight);
        CGFloat offsetX = (self.frame.size.width - iconWidth) / 2.0;
        CGFloat offsetY = (self.frame.size.height - iconHeight) / 2.0;
        CGPoint p0 = CGPointMake(offsetX, offsetY);
        [_icon drawAtPoint:p0 fromRect:r0 operation:NSCompositingOperationSourceOver fraction:1.0];
    }
}

- (void)mouseDown:(NSEvent *)event {    
    BOOL keepOn = YES;
    BOOL isInside = YES;
    NSPoint location;
    
    if (!self.enabled)
        return;
    
    [self setStyle:kCEEViewStyleClicked];
    
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        location = [self convertPoint:[event locationInWindow] fromView:nil];
        isInside = [self mouse:location inRect:[self bounds]];
        
        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                if (isInside)
                    [self setStyle:kCEEViewStyleClicked];
                else
                    [self clearStyle:kCEEViewStyleClicked];
                break;
                
            case NSEventTypeLeftMouseUp:
                [self clearStyle:kCEEViewStyleClicked];
                
                if (isInside && self.action)
                    [self sendAction:self.action to:self.target];
                
                keepOn = NO;
                break;
            default:
                /* Ignore any other kind of event. */
                break;
        }
    };
}

- (NSString*)title {
    return _title;
}

- (void)setTitle:(NSString *)title {
    _title = title;
    [self setNeedsDisplay:YES];
}

- (void)updateUserInterface {
    
    [super updateUserInterface];
    
    CEEUserInterfaceStyleScheme* current = (CEEUserInterfaceStyleScheme*)[self.styleSchemes pointerAtIndex:self.style];    
    if (!current)
        return;
    
    NSDictionary* descriptor = current.descriptor;
    
    NSString* fontProperty = descriptor[@"font"];
    NSString* backgroundColorProperty = descriptor[@"background_color"];
    NSString* borderColorProperty = descriptor[@"border_color"];
    NSString* textColorProperty = descriptor[@"text_color"];
    NSString* textShadowProperty = descriptor[@"text_shadow"];
    NSString* gradientProperty = descriptor[@"gradient"];
    NSString* gradientAngleProperty = descriptor[@"gradient_angle"];
    NSString* borderWidthProperty = descriptor[@"border_width"];
    NSString* iconColorProperty = descriptor[@"icon_color"];
    NSString* radiusProperty = descriptor[@"radius"];
    
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
    
    if (borderWidthProperty)
        self.borderWidth = [borderWidthProperty floatValue];
    
    if (iconColorProperty)
        self.iconColor = [CEEUserInterfaceStyleConfiguration createColorFromString:iconColorProperty];
    
    if (radiusProperty)
        self.cornerRadius = [radiusProperty floatValue];    
}

@end
