//
//  CEEAddReferenceView.h
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

@interface CEEAddReferenceView : NSView
@property (strong) IBOutlet CEETableView *filePathTable;
@property (strong) IBOutlet CEEButton *openFileButton;
@property (strong) IBOutlet CEEButton *removeFileButton;
@end

NS_ASSUME_NONNULL_END
