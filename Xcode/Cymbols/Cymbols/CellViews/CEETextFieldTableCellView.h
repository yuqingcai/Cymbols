//
//  CEETextFieldTableCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"
#import "CEETableCellViewTextLabel.h"
#import "CEETextFieldSimple.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEETextFieldTableCellView : CEETableCellView
@property (weak) IBOutlet CEETableCellViewTextLabel *text;
@property (weak) IBOutlet CEETextFieldSimple *input;
@end

NS_ASSUME_NONNULL_END
