//
//  CEESessionFrameTitleView.m
//  Cymbols
//
//  Created by qing on 2020/8/18.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESessionFrameTitleView.h"

@implementation CEESessionFrameTitleView

/*- (void)drawRect:(NSRect)dirtyRect {
    CGFloat diff = 0.0;
    NSRect rect;
    
    if (self.cornerRadius > FLT_EPSILON) {
        [self setWantsLayer:YES];
        [self.layer setCornerRadius:self.cornerRadius];
    }
    
    NSSize frameSize = self.frame.size;
    NSRect contentRect = NSMakeRect(0.0, 0.0,
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
    
    if (self.title) {
        NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
        [attributes setValue:self.font forKey:NSFontAttributeName];
        [attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
        [attributes setValue: @(self.kern) forKey:NSKernAttributeName];

        if (self.textShadow)
            [attributes setValue:self.textShadow forKey:NSShadowAttributeName];
        
        NSSize minimalStringSize = [[[NSAttributedString alloc] initWithString:@"..." attributes:attributes] size];
        NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:self.title attributes:attributes];
        NSSize drawingSize = [drawingString size];
        CGFloat captionWidth = self.frame.size.width - (self.leadingOffset + self.tailingOffset);
        if (minimalStringSize.width < captionWidth) {
            if (drawingSize.width < captionWidth) {
                rect = dirtyRect;
                rect.origin.x = self.leadingOffset;
                diff = (self.font.ascender + fabs(self.font.descender)) / 2.0 - fabs(self.font.descender);
                rect.origin.y = (self.frame.size.height / 2.0) - diff;
                [drawingString drawWithRect:rect options:0 context:nil];
            }
            else {
                NSInteger subIndex = 1;
                NSString* subjectString = nil;
                while (1) {
                    subjectString = [@"..." stringByAppendingString:[self.title substringFromIndex:subIndex]];
                    drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
                    drawingSize = [drawingString size];
                    if (drawingSize.width < captionWidth)
                        break;
                    subIndex ++;
                }
                rect = dirtyRect;
                rect.origin.x = self.leadingOffset;
                diff = (self.font.ascender + fabs(self.font.descender)) / 2.0 - fabs(self.font.descender);
                rect.origin.y = (self.frame.size.height / 2.0) - diff;
                [drawingString drawWithRect:rect options:0 context:nil];
            }
        }
    }
}
*/
@end
