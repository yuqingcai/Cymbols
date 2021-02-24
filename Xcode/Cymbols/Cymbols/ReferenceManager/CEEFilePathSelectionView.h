//
//  CEEFilePathSelectionView.h
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/2/1.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETextTitle.h"
#import "CEETableView.h"
#import "CEECheckBox.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFilePathSelectionView : NSView
@property (weak) IBOutlet CEETextTitle *filePathLabel;
@property (weak) IBOutlet CEETableView *filePathTable;
@property (weak) IBOutlet CEECheckBox *selectAllButton;
@end

NS_ASSUME_NONNULL_END
