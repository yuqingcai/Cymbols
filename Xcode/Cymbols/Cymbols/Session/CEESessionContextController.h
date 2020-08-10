//
//  CEESessionContextController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/1.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETitleView.h"
#import "CEETableView.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN
 
@interface CEESessionContextController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSSplitViewDelegate, NSSplitViewDelegate>
@property (strong) CEESession* session;
@end

NS_ASSUME_NONNULL_END
