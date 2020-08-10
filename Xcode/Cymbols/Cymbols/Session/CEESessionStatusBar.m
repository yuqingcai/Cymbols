
//
//  CEESessionStatusBar.m
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEESessionStatusBar.h"

@interface CEESessionStatusBar ()
@property CGFloat kern;
@property CGFloat iconWidth;
@property CGFloat iconHeight;
@end

@implementation CEESessionStatusBar

@synthesize content = _content;

- (void)initProperties {
    [super initProperties];
    _kern = 0.22;
    _iconColor = [NSColor grayColor];
    _leadingOffset = 5.0;
    _tailingOffset = 0.0;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    CGFloat diff = 0.0;
    NSRect rect;
        
    if (_content) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        [attributes setValue:self.font forKey:NSFontAttributeName];
        [attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
        [attributes setValue: @(_kern) forKey:NSKernAttributeName];

        if (self.textShadow)
            [attributes setValue:self.textShadow forKey:NSShadowAttributeName];
        
        NSSize minimalStringSize = [[[NSAttributedString alloc] initWithString:@"..." attributes:attributes] size];
        NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:_content attributes:attributes];
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
                    subjectString = [@"..." stringByAppendingString:[_content substringFromIndex:subIndex]];
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

- (void)setContent:(NSString *)content {
    _content = content;
    [self setNeedsDisplay:YES];
}

- (NSString*)content {
    return _content;
}


@end
