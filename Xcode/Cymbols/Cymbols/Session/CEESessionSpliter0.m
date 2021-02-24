//
//  CEESessionSpliter0.m
//  Cymbols
//
//  Created by qing on 2020/7/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESessionSpliter0.h"

@implementation CEESessionSpliter0

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.identifier = @"IDSessionSplit0View";
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

- (void)toggleSymbolView {
    [self toggleViewByIdentifier:@"IDSessionSymbolViewController"];
}

- (void)toggleSourceBufferView {
    [self toggleViewByIdentifier:@"IDSessionSourceBufferViewController"];
}

- (void)toggleFileReferenceView {
    [self toggleViewByIdentifier:@"IDSessionFileReferenceViewController"];
}

- (void)showSymbolView:(BOOL)shown {
    [self showView:shown byIdentifier:@"IDSessionSymbolViewController"];
}

- (void)showSourceBufferView:(BOOL)shown {
    [self showView:shown byIdentifier:@"IDSessionSourceBufferViewController"];
}

- (void)showFileReferenceView:(BOOL)shown {
    [self showView:shown byIdentifier:@"IDSessionFileReferenceViewController"];
}


@end
