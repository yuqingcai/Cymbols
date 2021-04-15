//
//  CEETextFieldTableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETextFieldTableCellView.h"

@implementation CEETextFieldTableCellView

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    
    NSRect textRect = _text.frame;
    NSRect inputRect = _input.frame;
    
    textRect.origin.x = 4.0;
    [_text setFrame:textRect];
    
    inputRect.origin.x = textRect.origin.x + textRect.size.width;
    inputRect.size.width = newSize.width - textRect.origin.x - textRect.size.width - 20.0;
    if (inputRect.size.width <= 0.0)
        inputRect.size.width = 1.0;
    
    [_input setFrame:inputRect];
}

@end
