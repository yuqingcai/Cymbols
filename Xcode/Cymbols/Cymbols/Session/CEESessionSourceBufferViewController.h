//
//  CEESessionSourceBufferViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEESessionSourceBufferTableView.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionSourceBufferViewController : CEEViewController <CEETableViewDelegate, CEETableViewDataSource, NSMenuDelegate>
@property (weak) CEESession* session;
@end

NS_ASSUME_NONNULL_END
