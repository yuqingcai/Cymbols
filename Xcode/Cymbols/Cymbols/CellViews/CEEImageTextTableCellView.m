//
//  CEEImageTextTableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEImageTextTableCellView.h"

@implementation CEEImageTextTableCellView

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    
    NSRect iconRect = _icon.frame;
    NSRect textRect = _text.frame;
    
    iconRect.origin.x = 0.0;
    [_icon setFrame:iconRect];
    
    textRect.origin.x = iconRect.size.width;
    textRect.size.width = newSize.width - iconRect.origin.x - iconRect.size.width;
    if (textRect.size.width <= 0.0)
        textRect.size.width = 1.0;
    
    [_text setFrame:textRect];
}

@end
