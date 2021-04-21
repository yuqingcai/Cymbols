//
//  CEEPopupPanel.m
//  Cymbols
//
//  Created by qing on 2020/7/8.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEPopupPanel.h"
#import "CEEViewController.h"
#import "CEEView.h"
#import "CEEIdentifier.h"

const CGFloat CEEPopupPanelSizeDefaultWidth = 400.0;
const CGFloat CEEPopupPanelSizeDefaultHeight = 400.0;
const CGFloat CEEPopupPanelSizeMinimumWidth = 400.0;
const CGFloat CEEPopupPanelSizeMinimumHeight = 120.0;

@interface CEEPopupPanel()
@property (strong) id localMonitor;
@property (strong) id globalMonitor;
@end

@implementation CEEPopupPanel

@synthesize exclusived = _exclusived;

- (void)initProperties {
    [self setStyleMask:NSWindowStyleMaskBorderless];
    [self setHasShadow:YES];
    [self setBackgroundColor:[NSColor clearColor]];
    [self setFloatingPanel:YES];
    [self setExclusived:YES];
    // Hightest window level
    [self setLevel:kCGCursorWindowLevel];
    // This is very import to a Popup Panel, when some other window
    // is being run modally, the Popup Panel can receive message normally.
    [self setWorksWhenModal:YES];
}

- (void)setContentView:(__kindof NSView *)contentView {
    [super setContentView:contentView];
    self.contentView.wantsLayer = YES;
    self.contentView.layer.cornerRadius = 6.0;
    self.contentView.layer.masksToBounds   = YES;
}

- (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag {
    self = [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag];
    if (!self)
        return nil;
    
    [self initProperties];
    [self setBackingType:backingStoreType];
    
    return self;
}

- (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag screen:(NSScreen *)screen {
    self = [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag screen:screen];
    if (!self)
        return nil;
    
    [self initProperties];
    [self setBackingType:backingStoreType];
    
    return self;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

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
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)mouseUp:(NSEvent *)event {
    [super mouseUp:event];
    if ([self isVisible]) {
        [self close];
    }
}

- (void)close {
    [super close];
    if (_popupDelegate)
        [_popupDelegate closePopupPanel:self];
}

- (CGFloat)expactedWidth {
    return CEEPopupPanelSizeDefaultWidth;
}

- (CGFloat)expactedHeight {
    return CEEPopupPanelSizeDefaultHeight;
}

- (CGFloat)minimumWidth {
    return CEEPopupPanelSizeMinimumWidth;
}

- (CGFloat)minimumHeight {
    return CEEPopupPanelSizeMinimumHeight;
}

@end
