//
//  CEEFilePathCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/9.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEFilePathCellView.h"

@implementation CEEFilePathCellView

- (void)setStyleState:(CEEViewStyleState)state {
    [super setStyleState:state];
    for (NSView* view in self.subviews)
        [view setStyleState:state];
}

@end
