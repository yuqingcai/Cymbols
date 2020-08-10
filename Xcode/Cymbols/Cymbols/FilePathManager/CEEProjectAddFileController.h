//
//  CEEProjectAddFileController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEProjectAddFileController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSOpenSavePanelDelegate>
@property (weak) CEESession* session;
@end

NS_ASSUME_NONNULL_END
