//
//  CEESessionFrameManagerView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/29.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionFrameManagerView.h"

@interface CEESessionFrameManagerView()
@property CGFloat kern;
@property CGFloat iconWidth;
@property CGFloat iconHeight;
@property (strong) CEEHighlightView *highlightView;
@end

@implementation CEESessionFrameManagerView 

- (void)initProperties {
    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    _kern = 0.22;
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

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    //NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
    //[attributes setValue:self.font forKey:NSFontAttributeName];
    //[attributes setValue:self.textColor forKey:NSForegroundColorAttributeName];
    //[attributes setValue: @(_kern) forKey:NSKernAttributeName];
    //
    //NSString* string = @"Cymbols";
    //NSAttributedString *drawingString = [[NSAttributedString alloc] initWithString:string attributes:attributes];
    //NSSize drawingSize = [drawingString size];
    //NSRect rect = dirtyRect;
    //rect.origin.x = (rect.size.width - drawingSize.width) / 2.0;
    //rect.origin.y = (rect.size.height - drawingSize.height) / 2.0;
    //[drawingString drawWithRect:rect options:0 context:nil];
}

@end
