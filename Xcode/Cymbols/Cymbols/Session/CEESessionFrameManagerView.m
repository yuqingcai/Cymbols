//
//  CEESessionFrameManagerView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/29.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionFrameManagerView.h"
#import "CEEImageView.h"

@interface CEESessionFrameManagerView()
@property CGFloat kern;
@property NSImage* icon;
@property (strong) CEEHighlightView *highlightView;
@end

@implementation CEESessionFrameManagerView 

- (void)initProperties {
    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    _kern = 0.22;
    _icon = [NSImage imageNamed:@"icon_butterfly_128x128"];
    _iconColor = [NSColor grayColor];
}

- (void)highlight:(BOOL)enable {
    
    NSRect bounds = self.bounds;
    NSRect frame0 = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    NSRect frame1 = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    NSSize size = bounds.size;
    
    frame0 = NSMakeRect(size.width / 2.0, size.height / 2.0, 0.0, 0.0);
    frame1 = bounds;
    
    if (enable) {
        _highlightView = [[CEEHighlightView alloc] initWithFrame:frame0];
        [_highlightView setStyleConfiguration:[self styleConfiguration]];
        [self addSubview:_highlightView];
        
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.2f;
            [[self->_highlightView animator] setFrame:frame1];
        } completionHandler:^() {
        }];
    }
    else {
        if (_highlightView)
            [_highlightView removeFromSuperview];
        _highlightView = nil;
    }
}

#pragma mark - protocol NSDraggingDestination

- (BOOL)attachable {
    if (_delegate) {
        if ([_delegate respondsToSelector:@selector(frameViewAttachable:)])
            return [_delegate frameViewAttachable:self];
        else
            return YES;
    }
    return YES;
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self || ![self attachable])
        return NSDragOperationNone;
    
    [self highlight:YES];
    return NSDragOperationCopy;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self || ![self attachable])
        return NSDragOperationNone;
    
    return NSDragOperationMove;
}

- (void)draggingExited:(nullable id<NSDraggingInfo>)sender {
    [self highlight:NO];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    if ([sender draggingSource] == self || ![self attachable])
        return NO;
    return YES;
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender {
    [self highlight:NO];
    
    if ([sender draggingSource] == self || ![self attachable])
        return;
    
    NSPoint location = [self convertPoint:[sender draggingLocation] fromView:nil];
    if (!NSPointInRect(location, self.bounds))
        return;
        
    if (!_delegate)
        return;
    
    NSDragOperation dragOperation = [sender draggingSourceOperationMask];
    NSPasteboard* pasteboard = [sender draggingPasteboard];
    if ([[pasteboard types] containsObject:NSFilenamesPboardType])
        [_delegate createInitFrameViewWithFilePaths:[pasteboard propertyListForType:NSFilenamesPboardType]];
}

- (void)updateUserInterface {
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
    
    if (current.font)
        self.font = current.font;
    
    if (current.backgroundColor)
        self.backgroundColor = current.backgroundColor;
    
    if (current.borderColor)
        self.borderColor = current.borderColor;
    
    if (current.dividerColor)
        self.dividerColor = current.dividerColor;
    
    if (current.textColor)
        self.textColor = current.textColor;
    
    if (current.textShadow)
        self.textShadow = current.textShadow;
    
    if (current.gradient)
        self.gradient = current.gradient;
    
    self.gradientAngle = current.gradientAngle;
    
    if (current.borders)
        self.borders = current.borders;
    
    self.borderWidth = current.borderWidth;
    self.cornerRadius = current.cornerRadius;
    self.iconColor = current.iconColor;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    NSColor* tintedColor = self.iconColor;
    _icon = [self tintedImage:_icon withColor:tintedColor];
    CGFloat x = (self.frame.size.width - _icon.size.width) / 2.0;
    CGFloat y = (self.frame.size.height - _icon.size.height) / 2.0;
    [_icon drawAtPoint:NSMakePoint(x, y) fromRect:NSZeroRect operation:NSCompositingOperationCopy fraction:1.0];
}

@end
