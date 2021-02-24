//
//  CEERemoveReferenceView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/9/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableView.h"
#import "CEETextField.h"
#import "CEEButton.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEERemoveReferenceView : NSView

@property (strong) IBOutlet CEETextField *filePathInput;
@property (strong) IBOutlet CEETableView *filePathTable;

@end

NS_ASSUME_NONNULL_END
