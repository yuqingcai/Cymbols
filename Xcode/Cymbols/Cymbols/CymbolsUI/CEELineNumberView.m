//
//  CEELineNumberView.m
//  Cymbols
//
//  Created by caiyuqing on 2018/12/3.
//  Copyright © 2018 caiyuqing. All rights reserved.
//

#import "CEELineNumberView.h"

@implementation CEELineNumberView

@synthesize lineNumberTags = _lineNumberTags;

- (void)initProperties {
    [super initProperties];
}

- (BOOL)isFlipped {
    return NO;
}

- (void)setLineNumberTags:(NSArray *)lineNumberTags {
    _lineNumberTags = lineNumberTags;
    [self setNeedsDisplay:YES];
}

- (NSArray*)lineNumberTags {
    return _lineNumberTags;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    NSDictionary* attribtues = @{
                                 NSFontAttributeName : self.font,
                                 NSForegroundColorAttributeName : self.textColor,
                                 };
    if (!_lineNumberTags)
        return ;
    
    for (NSDictionary* tag in _lineNumberTags) {
        NSString* label = tag[@"number"];
        NSRect bounds = NSRectFromString(tag[@"bounds"]);
        NSAttributedString* attributedLabel = [[NSAttributedString alloc] initWithString:label attributes: attribtues];
            
        NSRect rect = NSMakeRect((self.frame.size.width - [attributedLabel size].width) - 10,
                                 self.frame.size.height - bounds.origin.y - bounds.size.height,
                                 self.frame.size.width, 
                                 bounds.size.height);
        [attributedLabel drawInRect:rect];
    }
}

@end
