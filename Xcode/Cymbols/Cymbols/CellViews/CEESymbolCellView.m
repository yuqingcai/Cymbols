//
//  CEESymbolCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/11.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESymbolCellView.h"

@implementation CEESymbolCellView

- (void)setStyleState:(CEEViewStyleState)state {
    [super setStyleState:state];
    for (NSView* view in self.subviews)
        [view setStyleState:state];
}

@end
