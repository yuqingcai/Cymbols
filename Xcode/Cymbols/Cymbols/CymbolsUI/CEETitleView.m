
//
//  CEETitleView.m
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEETitleView.h"

@interface CEETitleView ()
@property CGFloat kern;
@property CGFloat iconWidth;
@property CGFloat iconHeight;
@end

@implementation CEETitleView

@synthesize title = _title;

- (void)initProperties {
    [super initProperties];
    _kern = 0.22;
    _iconColor = [NSColor grayColor];
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

@end
