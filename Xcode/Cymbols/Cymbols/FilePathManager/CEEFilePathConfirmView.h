//
//  CEEFilePathConfirmView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"
#import "CEETextLabel.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFilePathConfirmView : NSView

@property (weak) IBOutlet CEETextLabel *label;
@property (strong) IBOutlet CEETableView *sourceTable;

@end

NS_ASSUME_NONNULL_END
