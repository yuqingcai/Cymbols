//
//  CEEProjectSearchViewController.h
//  Cymbols
//
//  Created by qing on 2020/9/28.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEEProject.h"
#import "CEETableView.h"
#import "CEETextField.h"

NS_ASSUME_NONNULL_BEGIN


@interface CEEProjectSearchViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, CEETextViewDelegate>
@property BOOL autoStart;
@property CEESourcePoint* selectedResult;
@end

NS_ASSUME_NONNULL_END
