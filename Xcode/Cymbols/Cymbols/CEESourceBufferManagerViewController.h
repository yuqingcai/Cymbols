//
//  CEESourceBufferManagerViewController.h
//  Cymbols
//
//  Created by qing on 2020/7/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEESourceBufferSaveConfirmCellView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESourceBufferManagerViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSOpenSavePanelDelegate, CEESourceBufferSaveConfirmCellDelegate>

@end

NS_ASSUME_NONNULL_END
