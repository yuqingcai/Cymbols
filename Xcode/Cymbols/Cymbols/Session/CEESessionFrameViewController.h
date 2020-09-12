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

@class CEESessionFrameManager;

@interface CEESessionFrameViewController : CEEViewController <CEETitleViewDelegate, CEESessionFrameViewDelegate, NSWindowDelegate>
@property (weak) CEESessionFrameManager* manager;
@property (weak) CEESessionPort* port;
- (void)presentSource;
- (void)select;
- (void)deselect;
@end
