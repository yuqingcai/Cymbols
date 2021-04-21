//
//  CEEImageLinkView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/7.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEImageLinkView.h"

@interface CEEImageLinkView ()
@property (strong) NSTrackingArea *trackingArea;
@property (strong) NSCursor* currentCursor;
@end

@implementation CEEImageLinkView

- (void)initProperties {
    [super initProperties];
    [self addMouseTraceArea];
}

- (void)addMouseTraceArea {
    NSTrackingAreaOptions opt = NSTrackingActiveInActiveApp |
        NSTrackingMouseEnteredAndExited |
        NSTrackingMouseMoved |
        NSTrackingEnabledDuringMouseDrag;
    _trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds options:opt owner:self userInfo:nil];
    [self addTrackingArea:_trackingArea];
}

- (void)setFrame:(NSRect)frame {
    [super setFrame:frame];
    
    if (_trackingArea)
        [self removeTrackingArea:_trackingArea];
    [self addMouseTraceArea];
}

- (void)mouseEntered:(NSEvent *)event {
    _currentCursor = [NSCursor currentCursor];
    [[NSCursor pointingHandCursor] set];
}

- (void)mouseExited:(NSEvent *)event {
    [_currentCursor set];
}

- (void)mouseDown:(NSEvent *)event {
    if (!_URLString)
        return;
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:_URLString]];
}

@end
