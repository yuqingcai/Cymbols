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
@synthesize state = _state;

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
    _state = NSOffState;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
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
            
            if (disableStyle.iconColor)
                iconColor = disableStyle.iconColor;
            
            gradientAngle = disableStyle.gradientAngle;
            borderWidth = disableStyle.borderWidth;
            cornerRadius = disableStyle.cornerRadius;
        }
    }
    
    NSSize frameSize = self.frame.size;
    NSRect rect = NSMakeRect((borderWidth / 2.0),
                             (borderWidth / 2.0),
                             frameSize.width - borderWidth,
                             frameSize.height - borderWidth);
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
    
    if ([_title compare:@""] != NSOrderedSame) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        
        [attributes setValue:font forKey:NSFontAttributeName];
        [attributes setValue:textColor forKey:NSForegroundColorAttributeName];
        
        if (textShadow)
            [attributes setValue:textShadow forKey:NSShadowAttributeName];
        
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
        CGFloat height = font.ascender + font.descender;
        CGFloat x = (rect.size.width - drawingSize.width) / 2.0;
        CGFloat y = (rect.size.height - height) / 2.0;
        NSRect drawRect = NSMakeRect(x, y, width, height);
        [drawingString drawWithRect:drawRect options:0 context:nil];
    }
    
    if (_icon) {
        if (iconColor)
            _icon = [self tintedImage:_icon withColor:iconColor];
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
    
    if (!self.isEnabled)
        return;
    
    self.state = NSOnState;
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        location = [self convertPoint:[event locationInWindow] fromView:nil];
        isInside = [self mouse:location inRect:[self bounds]];
        
        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                if (isInside)
                    self.state = NSOnState;
                else
                    self.state = NSOffState;
                break;
                
            case NSEventTypeLeftMouseUp:
                self.state = NSOffState;
                
                if (isInside && self.action)
                    [self sendAction:self.action to:self.target];
                
                keepOn = NO;
                break;
            default:
                /* Ignore any other kind of event. */
                break;
        }
    }
}

- (void)setState:(NSControlStateValue)state {
    if (!self.isEnabled)
        return ;
    
    _state = state;
    if (_state == NSOnState)
        [self setStyleState:kCEEViewStyleStateClicked];
    else if (_state == NSOffState) {
        if (self.superview && [self.superview styleState] == kCEEViewStyleStateDeactived)
            [self setStyleState:kCEEViewStyleStateDeactived];
        else
            [self setStyleState:kCEEViewStyleStateActived];
    }
}

- (NSControlStateValue)state {
    return _state;
}

- (NSString*)title {
    return _title;
}

- (void)setTitle:(NSString *)title {
    _title = title;
    [self setNeedsDisplay:YES];
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
    
    self.iconColor = current.iconColor;
    self.cornerRadius = current.cornerRadius;
}

@end
