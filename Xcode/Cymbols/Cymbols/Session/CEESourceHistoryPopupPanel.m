//
//  CEESourceHistoryPopupPanel.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/4/3.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEESourceHistoryPopupPanel.h"
#import "CEESourceHistoryViewController.h"
#import "CEETableView.h"

@implementation CEESourceHistoryPopupPanel

@synthesize port = _port;

- (CGFloat)minimumWidth {
    return CEEPopupPanelSizeDefaultWidth;
}

- (CGFloat)minimumHeight {
    CEESourceHistoryViewController* viewController = (CEESourceHistoryViewController*)self.contentViewController;
    CEETableView* tableView = [viewController tableView];
    [tableView reloadData];
    CGFloat height = [tableView rowHeight];
    if (height <= FLT_EPSILON)
        return CEEPopupPanelSizeMinimumHeight;
    return height;
}

- (CGFloat)expactedWidth {
    return 400.0;
}

- (CGFloat)expactedHeight {
    CEESourceHistoryViewController* viewController = (CEESourceHistoryViewController*)self.contentViewController;
    CEETableView* tableView = [viewController tableView];
    [tableView reloadData];
    CGFloat rowHeight = [tableView rowHeight];
    
    id<CEETableViewDataSource> dataSource = tableView.dataSource;
    CGFloat height = rowHeight * [dataSource numberOfRowsInTableView:tableView];
    if (height < FLT_EPSILON)
        return CEEPopupPanelSizeMinimumHeight;
    return height;
}

- (void)setPort:(CEESessionPort *)port {
    CEESourceHistoryViewController* viewController = (CEESourceHistoryViewController*)self.contentViewController;
    [viewController setPort:port];
}

- (CEESessionPort*)port {
    return _port;
}

- (void)orderFront:(id)sender {
    [super orderFront:sender];
    [(CEEView*)self.contentView setStyleState:kCEEViewStyleStateActived];
}

@end
