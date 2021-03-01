//
//  CEETrialVersionWelcomeViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/2/13.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEETrialVersionWelcomeViewController.h"
#import "CEETextLabel.h"
#import "CEETextTitle.h"

#define FREEZE_SECOND 200

@interface CEETrialVersionWelcomeViewController ()
@property (strong) NSTimer* freezeTimer;
@property NSInteger freezeCounter;
@property (weak) IBOutlet CEETextLabel *versionTitle;
@property (weak) IBOutlet CEETextTitle *freezeTitle;
@end

@implementation CEETrialVersionWelcomeViewController
- (void)viewDidAppear {
    [super viewDidAppear];
    
    AppDelegate* delegate = [NSApp delegate];
    
    _freezeCounter = FREEZE_SECOND;
    _freezeTimer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(freeze:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_freezeTimer forMode:NSRunLoopCommonModes];
    
    _versionTitle.stringValue = [NSString stringWithFormat:@"Version %@ (%@)", [delegate versionString], [delegate bundleVersionString]];
    _freezeTitle.stringValue = [NSString stringWithFormat:@"Trial Version Time Freeze %lds", _freezeCounter, nil];
}

- (void)freeze:(NSTimer *)timer {
    _freezeCounter --;
    
    _freezeTitle.stringValue = [NSString stringWithFormat:@"Trial Version Time Freeze %lds", _freezeCounter, nil];
    
    if (_freezeCounter == 0) {
        _freezeCounter = FREEZE_SECOND;
        
        if (_freezeTimer) {
            [_freezeTimer invalidate];
            _freezeTimer = nil;
        }
        
        [self.view.window close];
    }
}

@end
