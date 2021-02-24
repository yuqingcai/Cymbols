//
//  CEETextTableCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/9.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"
#import "CEETableCellViewTextLabel.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEETextTableCellView : CEETableCellView
@property (weak) IBOutlet CEETableCellViewTextLabel *text;
@end

NS_ASSUME_NONNULL_END
