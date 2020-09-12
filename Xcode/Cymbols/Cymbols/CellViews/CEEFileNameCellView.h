//
//  CEEFileNameCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETextLabel.h"
#import "CEEImageView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFileNameCellView : CEEView
@property (weak) IBOutlet CEETextLabel *title;
@property (weak) IBOutlet CEEImageView *icon;
@end

NS_ASSUME_NONNULL_END
