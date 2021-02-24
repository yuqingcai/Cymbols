//
//  CEETimeFreezerViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/2/13.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEETimeFreezerViewController.h"
#import "CEETextLabel.h"
#import "CEETextTitle.h"

#define FREEZE_SECOND 10

@interface CEETimeFreezerViewController ()
@property (strong) NSTimer* freezeTimer;
@property NSInteger freezeCounter;
@property (weak) IBOutlet CEETextTitle *freezeTitle;
@end

@implementation CEETimeFreezerViewController
- (void)viewDidAppear {
    [super viewDidAppear];
    
    _freezeCounter = FREEZE_SECOND;
    _freezeTimer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(freeze:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_freezeTimer forMode:NSRunLoopCommonModes];
    
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
        
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
        
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseOK];
    }
}

- (IBAction)buyReleaseVersion:(id)sender {
    NSURL* URL = [NSURL URLWithString:@"https://apps.apple.com/cn/app/cymbols/id1535168866"];
    [[NSWorkspace sharedWorkspace] openURL:URL];
}

@end
