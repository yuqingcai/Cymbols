//
//  CEEView.m
//  Cymbols
//
//  Created by Qing on 2018/9/13.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"

@implementation CEEHighlightView
@end

@implementation CEEView

- (void)initProperties {
    _font = [NSFont systemFontOfSize:12.0];
    _backgroundColor = [NSColor clearColor];
    _borderColor = [NSColor clearColor];
    _textColor = [NSColor textColor];
    _dividerColor = [NSColor clearColor];
    _textShadow = nil;
    _gradient = nil;
    _gradientAngle = 270.0;
    _borders = nil;
    _borderWidth = 0.0;
    _cornerRadius = 0.0;
    _styleState = kCEEViewStyleStateActived;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    self = [super initWithCoder:decoder];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (void)addMouseTraceArea {
    NSTrackingAreaOptions opt = NSTrackingActiveInActiveApp |
        NSTrackingMouseEnteredAndExited |
        NSTrackingMouseMoved |
        NSTrackingEnabledDuringMouseDrag;
    _trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds options:opt owner:self userInfo:nil];
    [self addTrackingArea:_trackingArea];
}

- (NSImage*)tintedImage:(NSImage*)image withColor:(NSColor *)tint {
    NSImage *tintedImage = [image copy];
    if (tint) {
        [tintedImage lockFocus];
        [tint set];
        NSRect imageRect = { NSZeroPoint, [tintedImage size] };
        NSRectFillUsingOperation(imageRect, NSCompositingOperationSourceAtop);
        [tintedImage unlockFocus];
    }
    return tintedImage;
}

- (void)drawRect:(NSRect)dirtyRect {    
    [super drawRect:dirtyRect];
    
    if (self.cornerRadius > FLT_EPSILON) {
        [self setWantsLayer:YES];
        [self.layer setCornerRadius:self.cornerRadius];
    }
    
    NSSize frameSize = self.frame.size;
    NSRect contentRect = NSMakeRect(0.0,
                                    0.0,
                             frameSize.width,
                             frameSize.height);
        
    NSBezierPath* contentPath = [NSBezierPath bezierPathWithRoundedRect:contentRect xRadius:self.cornerRadius yRadius:self.cornerRadius];
    
    if (self.backgroundColor) {
        [self.backgroundColor setFill];
        [contentPath fill];
    }
    
    if (!self.borders) {
        if (self.borderColor && self.borderWidth > FLT_EPSILON) {
            NSRect borderRect = NSMakeRect(self.borderWidth / 2.0, 
                                           self.borderWidth / 2.0,
                                           frameSize.width - self.borderWidth,
                                           frameSize.height - self.borderWidth);

            NSBezierPath* borderPath = [NSBezierPath bezierPathWithRoundedRect:borderRect xRadius:self.cornerRadius yRadius:self.cornerRadius];
            [self.borderColor setStroke];
            [borderPath setLineWidth: self.borderWidth];
            [borderPath stroke];
        }
    }
    else {
        if (self.borderColor && self.borderWidth > FLT_EPSILON) {
            [self.borderColor setStroke];
            NSBezierPath* border = [NSBezierPath bezierPath];
            [border setLineWidth: self.borderWidth];
            
            if ([self.borders containsString:@"top"]) {
                [border moveToPoint: NSMakePoint(0.0, frameSize.height - (self.borderWidth/2.0))];
                [border lineToPoint: NSMakePoint(frameSize.width, frameSize.height - (self.borderWidth/2.0))];
                [border stroke];
            }
            
            if ([self.borders containsString:@"bottom"]) {
                [border moveToPoint: NSMakePoint(0.0, (self.borderWidth/2.0))];
                [border lineToPoint: NSMakePoint(frameSize.width, (self.borderWidth/2.0))];
                [border stroke];
            }
            
            if ([self.borders containsString:@"left"]) {
                [border moveToPoint: NSMakePoint((self.borderWidth/2.0), 0.0)];
                [border lineToPoint: NSMakePoint((self.borderWidth/2.0), frameSize.height)];
                [border stroke];
            }
            
            if ([self.borders containsString:@"right"]) {
                [border moveToPoint: NSMakePoint(frameSize.width - (self.borderWidth/2.0), 0.0)];
                [border lineToPoint: NSMakePoint(frameSize.width - (self.borderWidth/2.0), frameSize.height)];
                [border stroke];
            }
        }
    }
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {    
    _styleConfiguration = configuration;
    [configuration configureView:self];
    [super setStyleConfiguration:configuration];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return _styleConfiguration;
}

- (void)setStyleState:(CEEViewStyleState)state {
    _styleState = state;
    [super setStyleState:state];
}

- (CEEViewStyleState)styleState {
    return _styleState;
}

- (NSImage*)createDraggingHint {
    return nil;
}

- (NSDraggingItem*)createDraggingItem {
    return nil;
}

- (void)drawString:(NSString*)string inRect:(NSRect)rect {
    NSDictionary* attributes = @{
                                 NSFontAttributeName : self.font,
                                 NSForegroundColorAttributeName : self.textColor,
                                 NSKernAttributeName : @(0.22)
                                 };
    NSAttributedString* attributedString = [self string:string withAttributes:attributes inRect:rect truncate:NSLineBreakByTruncatingTail];
    
    NSRect drawingRect = rect;
    CGFloat diff = (self.font.ascender + fabs(self.font.descender)) / 2.0 - fabs(self.font.descender);
    if (self.isFlipped)
        drawingRect.origin.y = (rect.size.height / 2.0) + diff;
    else
        drawingRect.origin.y = (rect.size.height / 2.0) - diff;
        
    [attributedString drawWithRect:drawingRect options:0 context:nil];
}

- (NSAttributedString*)string:(NSString*)string withAttributes:(NSDictionary*)attributes inRect:(NSRect)rect truncate:(NSLineBreakMode)mode {
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithString:string attributes:attributes];
    NSSize stringSize = [attributedString size];
    CGFloat fontWidth = [attributes[NSFontAttributeName] pointSize];
    if (stringSize.width < rect.size.width) {
        return attributedString;
    }
    else {
        NSInteger subIndex = 1;
        NSString* subjectString = nil;
        
        while (1) {
            if (mode == NSLineBreakByTruncatingHead) {
                subjectString = [@"..." stringByAppendingString:[string substringFromIndex:subIndex]];
                attributedString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                if ([attributedString size].width < rect.size.width)
                    return attributedString;
            }
            else {
                subjectString = [[string substringToIndex:subIndex] stringByAppendingString:@"..."];
                attributedString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                stringSize = [attributedString size];
                if ([attributedString size].width >= (rect.size.width - fontWidth))
                    return attributedString;
            }
            subIndex ++;
        }
    }
    return nil;
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
    
    if (current.dividerColor)
        self.dividerColor = current.dividerColor;
    
    if (current.textColor)
        self.textColor = current.textColor;
    
    if (current.textShadow)
        self.textShadow = current.textShadow;
    
    if (current.gradient)
        self.gradient = current.gradient;
    
    self.gradientAngle = current.gradientAngle;
    
    if (current.borders)
        self.borders = current.borders;
        
    self.borderWidth = current.borderWidth;
    self.cornerRadius = current.cornerRadius;
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
    self.userInterfaceStyles = [[NSPointerArray alloc] init];
    
    for (NSUInteger i = 0; i < kCEEViewStyleStateMax; i ++)
        [self.userInterfaceStyles addPointer:NULL];
    
    for (CEEUserInterfaceStyleScheme* scheme in schemes) {
        CEEUserInterfaceStyle* style = [[CEEUserInterfaceStyle alloc] initWithScheme:scheme];
        [self.userInterfaceStyles replacePointerAtIndex:scheme.styleState withPointer:(void*)style];
    }
}
@end
