//
//  CEECreateFileNotificationViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"
#import "CEEImageView.h"
#import "CEETextTitle.h"
#import "CEETextLabel.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEECreateFileNotificationViewController : CEEViewController
@property (weak) IBOutlet CEEImageView *notificationIcon;
@property (weak) IBOutlet CEETextTitle *notificationTitle;
@property (weak) IBOutlet CEETextLabel *notificationLabel;

@end

NS_ASSUME_NONNULL_END
