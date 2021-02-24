//
//  CEETableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"


NSComparisonResult viewHorizontalPositionCompare(NSView* view0, NSView* view1, void* context) {
    CGFloat x0 = view0.frame.origin.x;
    CGFloat x1 = view1.frame.origin.x;
    if (x0 < x1)
        return NSOrderedAscending;
    return NSOrderedDescending;
}

@implementation CEETableCellView

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (!self)
        return nil;
    
    [self setAutoresizesSubviews:YES];
    [self setTranslatesAutoresizingMaskIntoConstraints:YES];
    
    return self;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self setAutoresizesSubviews:YES];
    [self setTranslatesAutoresizingMaskIntoConstraints:YES];
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (!self)
        return nil;
    
    [self setAutoresizesSubviews:YES];
    [self setTranslatesAutoresizingMaskIntoConstraints:YES];
    
    return self;
}

- (void)setStyleState:(CEEViewStyleState)state {
    [super setStyleState:state];
    for (NSView* view in self.subviews)
        [view setStyleState:state];
}

@end
