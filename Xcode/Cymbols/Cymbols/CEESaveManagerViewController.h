//
//  CEESaveManagerViewController.h
//  Cymbols
//
//  Created by qing on 2020/7/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEECheckboxTableCellView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESaveManagerViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSOpenSavePanelDelegate, CEECheckboxTableCellViewDelegate>
@property (strong, nullable) NSArray* modifiedSourceBuffers;
@end

NS_ASSUME_NONNULL_END
