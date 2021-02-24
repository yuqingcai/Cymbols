
//
//  CEETitleView.m
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEETitleView.h"

@interface CEETitleView()
@property CGFloat iconWidth;
@property CGFloat iconHeight;
@property CGFloat iconLeading;

@end

@implementation CEETitleView

@synthesize title = _title;
@synthesize icon = _icon;

- (void)initProperties {
    [super initProperties];
    _kern = 0.22;
    _titleLeading = 5.0;
    _titleTailing = 5.0;
    _iconLeading = 0.0;
    _iconWidth = 0.0;
    _iconHeight = 0.0;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    if (_icon) {
        if (_iconWidth < FLT_EPSILON)
            _iconWidth = 16.0;
        if (_iconHeight < FLT_EPSILON)
            _iconHeight = 16.0;
        
        CGRect r0 = CGRectMake(0.0, 0.0, _iconWidth, _iconHeight);
        CGPoint p0 = CGPointMake(_iconLeading, (dirtyRect.size.height - _iconHeight) / 2.0);
        [_icon drawAtPoint:p0 fromRect:r0 operation:NSCompositingOperationSourceOver fraction:1.0];
    }
    
    if (_title) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        NSRect titleRect;
        CGFloat diff = (self.font.ascender + fabs(self.font.descender)) / 2.0 - fabs(self.font.descender);
        [attributes setValue:self.font forKey:NSFontAttributeName];
        [attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
        [attributes setValue: @(_kern) forKey:NSKernAttributeName];

        if (self.textShadow)
            [attributes setValue:self.textShadow forKey:NSShadowAttributeName];
        
        NSSize minimalStringSize = [[[NSAttributedString alloc] initWithString:@"..." attributes:attributes] size];
        NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:_title attributes:attributes];
        NSSize drawingSize = [drawingString size];
        CGFloat titleWidth = 0.0;
        
        titleWidth = dirtyRect.size.width - (_iconLeading + _iconWidth + _titleLeading + _titleTailing);
        titleRect = NSMakeRect(_iconLeading + _iconWidth + _titleLeading, (dirtyRect.size.height / 2.0) - diff, titleWidth, dirtyRect.size.height);
        
        if (minimalStringSize.width < titleWidth) {
            if (drawingSize.width < titleWidth) {
                [drawingString drawWithRect:titleRect options:0 context:nil];
            }
            else {
                NSInteger subIndex = 1;
                NSString* subjectString = nil;
                while (1) {
                    subjectString = [@"..." stringByAppendingString:[_title substringFromIndex:subIndex]];
                    drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                    drawingSize = [drawingString size];
                    if (drawingSize.width < titleWidth)
                        break;
                    subIndex ++;
                }
                [drawingString drawWithRect:titleRect options:0 context:nil];
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

- (void)setIcon:(NSImage *)icon {
    _icon = icon;
    
    if (_iconWidth < FLT_EPSILON)
        _iconWidth = 16.0;
    if (_iconHeight < FLT_EPSILON)
        _iconHeight = 16.0;
    _iconLeading = 5.0;
    [self setNeedsDisplay:YES];
}

- (NSImage*)icon {
    return _icon;
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
