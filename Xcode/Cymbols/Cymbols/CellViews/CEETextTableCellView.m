//
//  CEETextTableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/9.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETextTableCellView.h"

@implementation CEETextTableCellView

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
        
    NSRect textRect = _text.frame;
    textRect.origin.x = 0.0;
    textRect.size.width = newSize.width;
    [_text setFrame:textRect];
}

@end
