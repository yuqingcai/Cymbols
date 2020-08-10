//
//  CEEProjectRemoveFileController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEProjectRemoveFileController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSOpenSavePanelDelegate, NSTextFieldDelegate>
@property (weak) CEESession* session;
@end

NS_ASSUME_NONNULL_END
