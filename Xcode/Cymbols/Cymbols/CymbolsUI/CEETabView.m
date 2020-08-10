//
//  CEETabView.m
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEETabView.h"

@interface CEETabView()
@end

@implementation CEETabView

- (void)mouseDragged:(NSEvent *)event {
    [super mouseDragged:event];
    
    if (!_host)
        return ;
    
    NSPoint location = [event locationInWindow];
    location = [self convertPoint:location fromView:nil];
    NSDraggingItem* item = [_host createDraggingItem];
    NSImage* hint = [_host createDraggingHint];
    NSRect rect = NSMakeRect(location.x, location.y, hint.size.width, hint.size.height);
    [item setDraggingFrame:rect contents:hint];
    [self beginDraggingSessionWithItems:@[item] event:event source:(id<NSDraggingSource>)_host];
    
}

@end
