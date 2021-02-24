//
//  CEEFilePathScannerView.h
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/2/1.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETextLabel.h"
#import "CEETextTitle.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEFilePathScannerView : NSView

@property (weak) IBOutlet CEETextLabel *scanningLabel;
@property (weak) IBOutlet NSProgressIndicator *progressBar;

@end

NS_ASSUME_NONNULL_END
