//
//  CEECreateFileAtPathViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEETableView.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, CEECreateFileType) {
    kCEECreateFileTypeFile,
    kCEECreateFileTypeDirectory,
};

@interface CEECreateFileAtPathViewController : CEEViewController
@property CEECreateFileType createFileType;
@property NSString* relativePath;
@end

NS_ASSUME_NONNULL_END
