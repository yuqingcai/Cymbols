//
//  CEESessionFilePathController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETitleView.h"
#import "CEETableView.h"
#import "CEETreeView.h"
#import "CEETextField.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionFilePathController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, CEETextViewDelegate, CEETreeViewDelegate, CEETreeViewDataSource>
@property (weak) CEESession* session;
@end

NS_ASSUME_NONNULL_END
