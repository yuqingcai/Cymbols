//
//  CEECheckboxTableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEECheckboxTableCellView.h"

@implementation CEECheckboxTableCellView

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    
    NSRect checkboxRect = _checkbox.frame;
    NSRect textRect = _text.frame;
    
    checkboxRect.origin.x = 4.0;
    [_checkbox setFrame:checkboxRect];
    
    textRect.origin.x = checkboxRect.origin.x + checkboxRect.size.width;
    textRect.size.width = newSize.width - checkboxRect.origin.x - checkboxRect.size.width;
    if (textRect.size.width <= 0.0)
        textRect.size.width = 1.0;
    
    [_text setFrame:textRect];
}

- (IBAction)check:(id)sender {
    if (!self.selectedIdentifier || !_delegate)
        return;
    
    if (_checkbox.state == NSControlStateValueOn)
        [_delegate select:self.selectedIdentifier];
    else if (_checkbox.state == NSControlStateValueOff)
        [_delegate deselect:self.selectedIdentifier];
}

@end
