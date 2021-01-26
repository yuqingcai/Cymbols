//
//  CEESymbolCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/11.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEEImageView.h"
#import "CEETableCellViewTextLabel.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESymbolCellView : CEEView
@property (weak) IBOutlet CEEImageView *icon;
@property (weak) IBOutlet CEETableCellViewTextLabel *title;

@end

NS_ASSUME_NONNULL_END
