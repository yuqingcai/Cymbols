
//
//  CEETitleView.m
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEETitleView.h"

@implementation CEETitleView

@synthesize title = _title;

- (void)initProperties {
    [super initProperties];
    _kern = 0.22;
    _leadingOffset = 5.0;
    _tailingOffset = 5.0;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    CGFloat diff = 0.0;
    NSRect rect;
        
    if (_title) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        [attributes setValue:self.font forKey:NSFontAttributeName];
        [attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
        [attributes setValue: @(_kern) forKey:NSKernAttributeName];

        if (self.textShadow)
            [attributes setValue:self.textShadow forKey:NSShadowAttributeName];
        
        NSSize minimalStringSize = [[[NSAttributedString alloc] initWithString:@"..." attributes:attributes] size];
        NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:_title attributes:attributes];
        NSSize drawingSize = [drawingString size];
        CGFloat captionWidth = self.frame.size.width - (_leadingOffset + _tailingOffset);
        if (minimalStringSize.width < captionWidth) {
            if (drawingSize.width < captionWidth) {
                rect = dirtyRect;
                rect.origin.x = _leadingOffset;
                diff = (self.font.ascender + fabs(self.font.descender)) / 2.0 - fabs(self.font.descender);
                rect.origin.y = (self.frame.size.height / 2.0) - diff;
                [drawingString drawWithRect:rect options:0 context:nil];
            }
            else {
                NSInteger subIndex = 1;
                NSString* subjectString = nil;
                while (1) {
                    subjectString = [@"..." stringByAppendingString:[_title substringFromIndex:subIndex]];
                    drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                    drawingSize = [drawingString size];
                    if (drawingSize.width < captionWidth)
                        break;
                    subIndex ++;
                }
                rect = dirtyRect;
                rect.origin.x = _leadingOffset;
                diff = (self.font.ascender + fabs(self.font.descender)) / 2.0 - fabs(self.font.descender);
                rect.origin.y = (self.frame.size.height / 2.0) - diff;
                [drawingString drawWithRect:rect options:0 context:nil];
            }
        }
    }
}

- (void)setFrame:(NSRect)frame {
    [super setFrame:frame];
    if (self.trackingArea)
        [self removeTrackingArea:self.trackingArea];
    [self addMouseTraceArea];
}

- (void)mouseEntered:(NSEvent *)event {
    if (_delegate)
        [_delegate expandTitleViewDetail:self];
}

- (void)mouseExited:(NSEvent *)event {
    if (_delegate)
        [_delegate collapseTitleViewDetail:self];
}

- (void)setTitle:(NSString *)title {
    _title = title;
    [self setNeedsDisplay:YES];
}

- (NSString*)title {
    return _title;
}

- (NSImage*)createDraggingHint {    
    NSDictionary* attributes = @{
                                 NSFontAttributeName : self.font,
                                 NSForegroundColorAttributeName : self.textColor,
                                 NSKernAttributeName : @(0.22)
                                 };
    
    NSAttributedString *attributedString = [[NSAttributedString alloc] initWithString:self.title attributes:attributes];
    
    NSRect drawingRect = NSMakeRect(0.0,
                                    0.0,
                                    attributedString.size.width,
                                    attributedString.size.height);
    
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(drawingRect.size.width, drawingRect.size.height)];
    [image lockFocus];
    [[NSColor clearColor] set];
    NSRectFill(NSMakeRect(0, 0, image.size.width, image.size.height));
    [attributedString drawInRect:drawingRect];
    [image unlockFocus];
    
    return image;
}


- (void)mouseDragged:(NSEvent *)event {
    [super mouseDragged:event];
    
    if (!_draggingSource)
        return ;
    
    NSPoint location = [event locationInWindow];
    location = [self convertPoint:location fromView:nil];
    NSDraggingItem* item = [_draggingSource createDraggingItem];
    NSImage* hint = [self createDraggingHint];
    NSRect rect = NSMakeRect(location.x, location.y, hint.size.width, hint.size.height);
    [item setDraggingFrame:rect contents:hint];
    [self beginDraggingSessionWithItems:@[item] event:event source:(id<NSDraggingSource>)_draggingSource];
    
}

@end
