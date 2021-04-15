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


NSNotificationName CEENotificationTimeFreeze = @"CEENotificationTimeFreeze";

@interface CEETimerFreezer()
@property NSInteger beatCount;
@property NSInteger notifyInterval;
@end

@implementation CEETimerFreezer

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    _notifyInterval = (NSInteger)(TIMER_FREEZER_TRIGGER_INTERVAL / CEE_APP_HEART_BEAT_INTERVAL);
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
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationTimeFreeze object:self];
}

@end
