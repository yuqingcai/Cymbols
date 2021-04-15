//
//  CEEComboBoxTableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEComboBoxTableCellView.h"

@implementation CEEComboBoxTableCellView

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    
    NSRect textRect = _text.frame;
    NSRect comboBoxRect = _comboBox.frame;
    
    textRect.origin.x = 4.0;
    [_text setFrame:textRect];
    
    comboBoxRect.origin.x = textRect.origin.x + textRect.size.width;
    comboBoxRect.size.width = newSize.width - textRect.origin.x - textRect.size.width - 20.0;
    if (comboBoxRect.size.width <= 0.0)
        comboBoxRect.size.width = 1.0;
    
    [_comboBox setFrame:comboBoxRect];
}

@end
