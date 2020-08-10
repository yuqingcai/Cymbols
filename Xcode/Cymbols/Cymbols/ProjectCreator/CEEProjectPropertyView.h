//
//  CEEProjectPropertyView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/9.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"
#import "CEEButton.h"
#import "CEETextField.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEProjectPropertyView : NSView

@property (strong) IBOutlet CEETextField *projectNameLabel;
@property (strong) IBOutlet CEETextField *savePathLabel;
@property (strong) IBOutlet CEETextField *sourceFileLabel;
@property (strong) IBOutlet CEETableView *filePathTable;

@end

NS_ASSUME_NONNULL_END
