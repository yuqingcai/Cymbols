//
//  CEEAddFileSelectionView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"
#import "CEEButton.h"
#import "CEETextField.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEAddFileSelectionView : NSView
@property (strong) IBOutlet CEETableView *sourceTable;
@property (strong) IBOutlet CEEButton *openFilePanelButton;
@property (strong) IBOutlet CEEButton *removeFilePathsButton;

@end

NS_ASSUME_NONNULL_END
