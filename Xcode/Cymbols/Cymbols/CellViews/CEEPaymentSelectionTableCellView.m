//
//  CEEPaymentSelectionTableCellView.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/11.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEPaymentSelectionTableCellView.h"
@implementation CEEPaymentSelectionTableCellView

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    
    NSRect rect = _text.frame;
    rect.origin.x = 4.0;
    rect.size.width = newSize.width - rect.origin.x;
    [_text setFrame:rect];
        
    rect = _title.frame;
    rect.origin.x = 4.0;
    rect.size.width = newSize.width - rect.origin.x;
    [_title setFrame:rect];
}

@end
