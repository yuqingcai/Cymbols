//
//  CEESessionFrameViewController.h
//  Cymbols
//
//  Created by Qing on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEViewController.h"
#import "CEETitleView.h"
#import "CEESessionFrameView.h"
#import "CEEProject.h"

typedef NS_ENUM(NSUInteger, CEEPresentSourceState) {
    kCEEPresentSourceStateSuccess = 0,
    kCEEPresentSourceStateNoBuffer = 1,
};

@class CEESessionFrameManager;

@interface CEESessionFrameViewController : CEEViewController <CEETitleViewDelegate, CEESessionFrameViewDelegate, NSWindowDelegate>
@property (weak) CEESessionFrameManager* manager;
@property (weak) CEESessionPort* port;
- (CEEPresentSourceState)presentSourceBuffer;
- (void)select;
- (void)deselect;
@end
