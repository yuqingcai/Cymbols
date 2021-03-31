//
//  CEEProjectContextViewController.h
//  Cymbols
//
//  Created by qing on 2020/8/26.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEProjectContextViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource>
@property cee_long symbolIndex;
@end

NS_ASSUME_NONNULL_END
