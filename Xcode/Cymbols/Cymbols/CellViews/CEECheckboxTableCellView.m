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
    checkboxRect.origin.x = 0.0;
    checkboxRect.size.width = newSize.width;
    [_checkbox setFrame:checkboxRect];
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
