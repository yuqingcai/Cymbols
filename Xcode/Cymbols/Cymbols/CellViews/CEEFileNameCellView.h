//
//  CEEFileNameCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableCellViewTextLabel.h"
#import "CEEImageView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFileNameCellView : CEEView
@property (weak) IBOutlet CEETableCellViewTextLabel *title;
@property (weak) IBOutlet CEEImageView *icon;
@end

NS_ASSUME_NONNULL_END
