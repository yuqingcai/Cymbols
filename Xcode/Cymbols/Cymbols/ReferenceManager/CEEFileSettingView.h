//
//  CEEFileSettingView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEEImageView.h"
#import "CEETextTitle.h"
#import "CEETextField.h"
#import "CEECheckBox.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFileSettingView : NSView
@property (weak) IBOutlet CEEImageView *titleIcon;
@property (weak) IBOutlet CEETextTitle *titleLabel;
@property (weak) IBOutlet CEETextField *nameInput;
@property (weak) IBOutlet CEECheckBox *referenceCheckbox;
@end

NS_ASSUME_NONNULL_END
