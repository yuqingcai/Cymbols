//
//  CEEPopupPanel.m
//  Cymbols
//
//  Created by qing on 2020/7/8.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEPopupPanel.h"

@interface CEEPopupPanel()
@property (strong) id localMonitor;
@property (strong) id globalMonitor;
@end

@implementation CEEPopupPanel

- (void)registerEventMonitor {
    _localMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskLeftMouseDown handler:^(NSEvent *event) {
        NSEvent *result = event;
        if ([self isVisible]) {
            if (!NSPointInRect([NSEvent mouseLocation], self.frame))
                [self close];
        }
        return result;
    }];
    
    _globalMonitor = [NSEvent addGlobalMonitorForEventsMatchingMask:NSEventMaskLeftMouseDown handler:^(NSEvent *event) {
        if ([self isVisible])
            [self close];
    }];
}

- (void)dealloc {
    [NSEvent removeMonitor:_localMonitor];
    [NSEvent removeMonitor:_globalMonitor];
}

@end
