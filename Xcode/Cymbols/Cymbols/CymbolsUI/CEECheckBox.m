//
//  CEECheckBox.m
//  Cymbols
//
//  Created by caiyuqing on 2019/1/29.
//  Copyright © 2019 caiyuqing. All rights reserved.
//

#import "CEECheckBox.h"

@implementation CEECheckBox

- (void)initProperties { 
    [super initProperties];
    _boxOutlineColor = [NSColor gridColor];
    _boxBackgroundColor = [NSColor whiteColor];
    _boxBackgroundColorChecked = [NSColor blueColor];
    _boxContentColor = [NSColor whiteColor];
    self.borderWidth = 0.0;
    self.backgroundColor = [NSColor clearColor];
}

- (void)drawRect:(NSRect)dirtyRect {
    NSFont* font = self.font;
    NSColor* backgroundColor = self.backgroundColor;
    NSColor* borderColor = self.borderColor;
    NSColor* textColor = self.textColor;
    NSColor* iconColor = self.iconColor;
    NSShadow* textShadow = self.textShadow;
    NSGradient* gradient = self.gradient;
    CGFloat gradientAngle = self.gradientAngle;
    CGFloat borderWidth = self.borderWidth;
    CGFloat cornerRadius = self.cornerRadius;
    NSColor* boxOutlineColor = self.boxOutlineColor;
    NSColor* boxBackgroundColor = self.boxBackgroundColor;
    NSColor* boxBackgroundColorChecked = self.boxBackgroundColorChecked;
    NSColor* boxContentColor = self.boxContentColor;
    
    if (!self.isEnabled) {
        CEEUserInterfaceStyle* disableStyle = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:kCEEViewStyleStateDisabled];
        if (disableStyle) {
            if (disableStyle.font)
                font = disableStyle.font;
            
            if (disableStyle.backgroundColor)
                backgroundColor = disableStyle.backgroundColor;
            
            if (disableStyle.borderColor)
                borderColor = disableStyle.borderColor;
            
            if (disableStyle.textColor)
                textColor = disableStyle.textColor;
            
            if (disableStyle.textShadow)
                textShadow = disableStyle.textShadow;
            
            if (disableStyle.gradient)
                gradient = disableStyle.gradient;
            
            gradientAngle = disableStyle.gradientAngle;
            borderWidth = disableStyle.borderWidth;
            
            if (disableStyle.iconColor)
                iconColor = disableStyle.iconColor;
            
            cornerRadius = disableStyle.cornerRadius;
                
            if (disableStyle.boxOutlineColor)
                boxOutlineColor = disableStyle.boxOutlineColor;
            
            if (disableStyle.boxBackgroundColor)
                boxBackgroundColor = disableStyle.boxBackgroundColor;
            
            if (disableStyle.boxBackgroundColorChecked)
                boxBackgroundColorChecked = disableStyle.boxBackgroundColorChecked;
            
            if (disableStyle.boxContentColor)
                boxContentColor = disableStyle.boxContentColor;
        }
    }
    borderWidth = 0.0;
    backgroundColor = [NSColor clearColor];
    
    
    NSSize frameSize = self.frame.size;
    NSRect rect = NSMakeRect((borderWidth / 2.0),
                             (borderWidth / 2.0),
                             frameSize.width - borderWidth,
                             frameSize.height - borderWidth);    
    CGFloat base = rect.origin.y + rect.size.height / 2.0;
    NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:cornerRadius yRadius:cornerRadius];
    if (gradient) {
        [gradient drawInBezierPath:path angle:gradientAngle];
    }
    else {
        if (backgroundColor) {
            [backgroundColor setFill];
            [path fill];
        }
    }
    
    if (borderWidth >= FLT_EPSILON) {
        [path setLineWidth:borderWidth];
        if (borderColor) {
            [borderColor setStroke];
            [path stroke];
        }
    }
    
    CGFloat interval = 4.0;
    CGFloat boxSize = 13;
    NSRect boxRect = NSMakeRect(rect.origin.x + interval, 
                                base - boxSize / 2.0, 
                                boxSize, 
                                boxSize);
    CGFloat boxLineWidth = 1.0;
    CGFloat boxCornerRadius = 1.0;
    path = [NSBezierPath bezierPathWithRoundedRect:boxRect xRadius:boxCornerRadius yRadius:boxCornerRadius];
    [path setLineWidth:boxLineWidth];
    if (boxOutlineColor) {
        [boxOutlineColor setStroke];
        [path stroke];
    }
    
    NSRect contentRect = NSMakeRect(boxRect.origin.x + (boxLineWidth / 2.0), 
                                    boxRect.origin.y + (boxLineWidth / 2.0), 
                                    boxSize - boxLineWidth, 
                                    boxSize - boxLineWidth);
    path = [NSBezierPath bezierPathWithRoundedRect:contentRect xRadius:boxCornerRadius yRadius:boxCornerRadius];
    
    
    if (self.state == NSOnState) {
        if (boxBackgroundColorChecked) {
            [boxBackgroundColorChecked setFill];
            [path fill];
        }
        // draw the tick
        if (boxContentColor) {
            path = [[NSBezierPath alloc] init];
            [path setLineWidth:1.5];
            [boxContentColor setStroke];
            NSPoint p0 = NSMakePoint(boxRect.origin.x, boxRect.origin.y);
            NSPoint p1 = NSMakePoint(p0.x + 3, p0.y + 7);
            NSPoint p2 = NSMakePoint(p0.x + 5, p0.y + 4);
            NSPoint p3 = NSMakePoint(p0.x + 10, p0.y + 10);
            [path moveToPoint:p1];
            [path lineToPoint:p2];
            [path lineToPoint:p3];
            [path stroke];
        }
    }
    else {
        if (boxBackgroundColor) {
            [boxBackgroundColor setFill];
            [path fill];
        }
    }
    
    if ([self.title compare:@""] != NSOrderedSame) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        
        [attributes setValue:font forKey:NSFontAttributeName];
        [attributes setValue:textColor forKey:NSForegroundColorAttributeName];
        
        if (textShadow)
            [attributes setValue:textShadow forKey:NSShadowAttributeName];
        
        NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:self.title attributes:attributes];
        NSSize drawingSize = [drawingString size];
        NSRect titleRect = NSMakeRect(boxRect.origin.x + boxRect.size.width + interval, 
                                      0.0, 
                                      rect.size.width - boxRect.size.width - interval*2, 
                                      rect.size.height);
        
        if (drawingSize.width > titleRect.size.width) {
            NSInteger subIndex = 1;
            NSString* subjectString = nil;
            while (1) {
                subjectString = [@"..." stringByAppendingString:[self.title substringFromIndex:subIndex]];
                drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                drawingSize = [drawingString size];
                if (drawingSize.width < titleRect.size.width)
                    break;
                subIndex ++;
            }
        }
        
        CGFloat width = drawingSize.width;
        CGFloat height = font.ascender + font.descender;
        CGFloat x = titleRect.origin.x;
        CGFloat y = titleRect.origin.y + ((titleRect.size.height - height) / 2.0);
        NSRect drawRect = NSMakeRect(x, y, width, height);
        [drawingString drawWithRect:drawRect options:0 context:nil];
    }
}

- (void)updateUserInterface {
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
    
    if (current.font)
        self.font = current.font;
    
    if (current.backgroundColor)
        self.backgroundColor = current.backgroundColor;
    
    if (current.borderColor)
        self.borderColor = current.borderColor;
    
    if (current.textColor)
        self.textColor = current.textColor;
    
    if (current.textShadow)
        self.textShadow = current.textShadow;
    
    if (current.gradient)
        self.gradient = current.gradient;
    
    self.gradientAngle = current.gradientAngle;
    self.borderWidth = current.borderWidth;
    
    if (current.iconColor)
        self.iconColor = current.iconColor;
    
    self.cornerRadius = current.cornerRadius;
        
    if (current.boxOutlineColor)
        self.boxOutlineColor = current.boxOutlineColor;
    
    if (current.boxBackgroundColor)
        self.boxBackgroundColor = current.boxBackgroundColor;
    
    if (current.boxBackgroundColorChecked)
        self.boxBackgroundColorChecked = current.boxBackgroundColorChecked;
    
    if (current.boxContentColor)
        self.boxContentColor = current.boxContentColor;
}

@end
