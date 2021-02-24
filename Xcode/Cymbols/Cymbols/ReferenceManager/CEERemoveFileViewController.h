//
//  CEERemoveFileViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEERemoveFileViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSOpenSavePanelDelegate>
@property (strong) NSArray* removingFilePaths;
@end

NS_ASSUME_NONNULL_END
