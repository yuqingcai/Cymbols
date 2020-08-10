//
//  CEEProjectSettingView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/9.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"
#import "CEETextField.h"
#import "CEEButton.h"
#import "CEETextLabel.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEProjectSettingView : NSView
@property (weak) IBOutlet CEETextLabel *projectNameLabel;

@property (strong) IBOutlet CEEButton *saveAtButton;
@property (strong) IBOutlet CEEButton *addFilePathButton;
@property (strong) IBOutlet CEEButton *removeFilePathsButton;
@property (strong) IBOutlet CEETableView *filePathTable;
@property (strong) IBOutlet CEETextField *nameInput;
@property (strong) IBOutlet CEETextField *savePathInput;
@end

NS_ASSUME_NONNULL_END
