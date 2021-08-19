//
//  CEEPaymentSelectionTableCellView.h
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/11.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"
#import "CEETableCellViewTextTitle.h"
#import "CEETableCellViewTextLabel.h"


NS_ASSUME_NONNULL_BEGIN

@interface CEEPaymentSelectionTableCellView : CEETableCellView
@property (weak) IBOutlet CEETableCellViewTextTitle *title;
@property (weak) IBOutlet CEETableCellViewTextLabel *text;
@end

NS_ASSUME_NONNULL_END
