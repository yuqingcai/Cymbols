//
//  CEETableCellView.m
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"


@implementation CEETableCellView

- (void)initProperties {
    [super initProperties];
    _createdInTableView = NO;
}

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
