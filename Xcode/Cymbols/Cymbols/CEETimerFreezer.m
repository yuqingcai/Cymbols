//
//  CEETimerFreezer.m
//  Cymbols
//
//  Created by qing on 2020/9/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETimerFreezer.h"
#import "AppDelegate.h"
#include <CoreFoundation/CoreFoundation.h>

#define TRIGGER_SECOND 600

@interface CEETimerFreezer()
@property NSInteger beatCount;
@property NSInteger notifyInterval;
@property (strong) NSWindowController* timeFreezerWindowController;
@end

@implementation CEETimerFreezer

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    _timeFreezerWindowController = [[NSStoryboard storyboardWithName:@"TimeFreezer" bundle:nil] instantiateControllerWithIdentifier:@"IDTimeFreezerWindowController"];
    
    _notifyInterval = (NSInteger)(TRIGGER_SECOND / CEE_APP_HEART_BEAT_INTERVAL);
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(heartBeatResponse:) name:CEENotificationHeartBeat object:nil];
    return self;
}

- (void)heartBeatResponse:(NSNotification*)notification {
    _beatCount ++;
    if (_beatCount % _notifyInterval)
        return;
    
    [self freeze];
}

- (void)freeze {
    AppDelegate* delegate = [NSApp delegate];
    NSArray* windowControllers = [[delegate currentProject] windowControllers];
    
    for (NSWindowController* windowController in windowControllers) {
        if ([windowController.window isKeyWindow]) {
            [windowController.window beginSheet:_timeFreezerWindowController.window completionHandler:(^(NSInteger result) {
                [NSApp stopModalWithCode:result];
            })];
            [NSApp runModalForWindow:windowController.window];
            break;
        }
    }
}

@end
