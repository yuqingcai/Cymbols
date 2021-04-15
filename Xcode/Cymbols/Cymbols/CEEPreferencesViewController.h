//
//  CEEPreferencesViewController.h
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/4/6.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEEComboBox.h"
#import "CEETextField.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEPreferencesViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, CEEComboBoxDelegate, CEETextViewDelegate>

@end

NS_ASSUME_NONNULL_END
