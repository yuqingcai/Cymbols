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
    NSSize frameSize = self.frame.size;
    NSRect rect = NSMakeRect((self.borderWidth / 2.0),
                             (self.borderWidth / 2.0),
                             frameSize.width - self.borderWidth,
                             frameSize.height - self.borderWidth);    
    CGFloat base = rect.origin.y + rect.size.height / 2.0;
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
    if (_boxOutlineColor) {
        [_boxOutlineColor setStroke];
        [path stroke];
    }
    
    NSRect contentRect = NSMakeRect(boxRect.origin.x + (boxLineWidth / 2.0), 
                                    boxRect.origin.y + (boxLineWidth / 2.0), 
                                    boxSize - boxLineWidth, 
                                    boxSize - boxLineWidth);
    path = [NSBezierPath bezierPathWithRoundedRect:contentRect xRadius:boxCornerRadius yRadius:boxCornerRadius];
    
    if (self.state == NSOnState) {
        if (_boxBackgroundColorChecked) {
            [_boxBackgroundColorChecked setFill];
            [path fill];
        }
        // draw the tick
        if (_boxContentColor) {
            path = [[NSBezierPath alloc] init];
            [path setLineWidth:1.5];
            [_boxContentColor setStroke];
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
        if (_boxBackgroundColor) {
            [_boxBackgroundColor setFill];
            [path fill];
        }
    }
    
    if ([self.title compare:@""] != NSOrderedSame) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        
        [attributes setValue:self.font forKey:NSFontAttributeName];
        [attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
        
        if (self.textShadow)
            [attributes setValue:self.textShadow forKey:NSShadowAttributeName];
        
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
        CGFloat height = self.font.ascender + self.font.descender;
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
