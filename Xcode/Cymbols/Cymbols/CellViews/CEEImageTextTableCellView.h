//
//  CEEImageTextTableCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"
#import "CEETableCellViewTextLabel.h"
#import "CEEImageView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEImageTextTableCellView : CEETableCellView
@property (weak) IBOutlet CEETableCellViewTextLabel *text;
@property (weak) IBOutlet CEEImageView *icon;
@end

NS_ASSUME_NONNULL_END
