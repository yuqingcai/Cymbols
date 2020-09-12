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

@synthesize exclusived = _exclusived;

- (void)setExclusived:(BOOL)exclusived {
    if (_exclusived == exclusived)
        return;
    
    _exclusived = exclusived;
    if (_exclusived) {
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
    else {
        [NSEvent removeMonitor:_localMonitor];
        [NSEvent removeMonitor:_globalMonitor];
    }
}

- (BOOL)exclusived {
    return _exclusived;
}

- (void)dealloc {
    if (_exclusived) {
        [NSEvent removeMonitor:_localMonitor];
        [NSEvent removeMonitor:_globalMonitor];
    }
}

- (void)mouseUp:(NSEvent *)event {
    [super mouseUp:event];
    if ([self isVisible])
        [self close];
}

@end
