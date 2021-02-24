//
//  CEEFilePathView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"
#import "CEETextLabel.h"
#import "CEEImageView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFilePathView : NSView
@property (weak) IBOutlet CEEImageView *icon;
@property (weak) IBOutlet CEETextLabel *filePathLabel;
@property (strong) IBOutlet CEETableView *filePathTable;

@end

NS_ASSUME_NONNULL_END
