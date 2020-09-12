//
//  CEESymbolCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/11.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEEImageView.h"
#import "CEETextLabel.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESymbolCellView : CEEView
@property (weak) IBOutlet CEEImageView *icon;
@property (weak) IBOutlet CEETextLabel *title;
@property (weak) IBOutlet NSLayoutConstraint *leading;

@end

NS_ASSUME_NONNULL_END
