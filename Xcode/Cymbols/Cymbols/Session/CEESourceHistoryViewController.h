//
//  CEESourceHistoryViewController.h
//  Cymbols
//
//  Created by qing on 2020/3/20.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESourceHistoryViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource>
@property (weak) CEESessionPort* port;
@end

NS_ASSUME_NONNULL_END
