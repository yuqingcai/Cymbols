//
//  CEESessionSpliter1.m
//  Cymbols
//
//  Created by qing on 2020/7/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESessionSpliter1.h"

@implementation CEESessionSpliter1

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.identifier = @"IDSessionSplit1View";
}

- (void)setSession:(CEESession *)session {
    _session = session;
    for (NSSplitViewItem* item in [self splitViewItems]) {
        if ([[item viewController] respondsToSelector:@selector(setSession:)])
            [[item viewController] performSelector:@selector(setSession:) withObject:session];
    }
}

- (CEESession*)session {
    return _session;
}


@end
