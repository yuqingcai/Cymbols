//
//  CEESourceBufferSaveConfirmCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESourceBufferSaveConfirmCellView.h"

@implementation CEESourceBufferSaveConfirmCellView

- (void)setStyleState:(CEEViewStyleState)state {
    [super setStyleState:state];
    for (NSView* view in self.subviews)
        [view setStyleState:state];
}

- (IBAction)select:(id)sender {
    if (!self.sourceBufferIdentifier || !_delegate)
        return;
    
    if (_check.state == NSControlStateValueOn)
        [_delegate sourceBufferSelect:self.sourceBufferIdentifier];
    else if (_check.state == NSControlStateValueOff)
        [_delegate sourceBufferDeselect:self.sourceBufferIdentifier];
}

@end
