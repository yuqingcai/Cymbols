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
    
    self.borderWidth = 1.0;
    self.backgroundColor = nil;
}

- (void)mouseDown:(NSEvent *)event {
    if (!self.enabled)
        return;
    
    BOOL keepOn = YES;
    BOOL isInside = YES;
    NSPoint location;
        
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        location = [self convertPoint:[event locationInWindow] fromView:nil];
        isInside = [self mouse:location inRect:[self bounds]];
    
        switch ([event type]) {
            case NSEventTypeLeftMouseUp:
                if (isInside) {
                    
                    if ([self styleSet:kCEEViewStyleClicked])
                        [self clearStyle:kCEEViewStyleClicked];
                    else
                        [self setStyle:kCEEViewStyleClicked];
                                        
                    if (self.action)
                        [self sendAction:self.action to:self.target];
                }
    
                keepOn = NO;
                break;
            default:
                /* Ignore any other kind of event. */
                break;
        }
    }
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
    if ([self styleSet:kCEEViewStyleClicked]) {
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


- (void)setChecked:(BOOL)checked {
    if (!self.enabled)
        return;
    
    if (checked)
        [self setStyle:kCEEViewStyleClicked];
    else
        [self clearStyle:kCEEViewStyleClicked];
}

- (BOOL)checked {
    return [self styleSet:kCEEViewStyleClicked];
}

- (void)updateUserInterface {
    [super updateUserInterface];
    
    CEEUserInterfaceStyleScheme* current = (CEEUserInterfaceStyleScheme*)[self.styleSchemes pointerAtIndex:self.style];    
    if (!current)
        return;
    
    NSDictionary* descriptor = current.descriptor;
    
    NSString* boxOutlineColorProperty = descriptor[@"box_outline_color"];
    NSString* boxBackgroundColorProperty = descriptor[@"box_background_color"];
    NSString* boxBackgroundColorCheckedProperty = descriptor[@"box_background_color_checked"];
    NSString* boxContentColorProperty = descriptor[@"box_content_color"];
    
    if (boxOutlineColorProperty)
        self.boxOutlineColor = [CEEUserInterfaceStyleConfiguration createColorFromString:boxOutlineColorProperty];
    
    if (boxBackgroundColorProperty)
        self.boxBackgroundColor = [CEEUserInterfaceStyleConfiguration createColorFromString:boxBackgroundColorProperty];
        
    if (boxBackgroundColorCheckedProperty)
        self.boxBackgroundColorChecked = [CEEUserInterfaceStyleConfiguration createColorFromString:boxBackgroundColorCheckedProperty];
        
    if (boxContentColorProperty)
        self.boxContentColor = [CEEUserInterfaceStyleConfiguration createColorFromString:boxContentColorProperty];
}

@end
