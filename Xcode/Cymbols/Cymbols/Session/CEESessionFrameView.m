//
//  CEESessionFrameView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/29.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionFrameView.h"

@interface CEESessionFrameView()
@property (strong) CEEHighlightView *highlightView;
@property CEEViewRegion highlingthRegion;
@end

@implementation CEESessionFrameView 

- (void)initProperties {
    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    _highlingthRegion = kCEEViewRegionNone;
}

- (CEEViewRegion)highlightRegionFromLocation:(NSPoint)location {
    location = [self convertPoint:location fromView:nil];
    CEEViewRegion region = kCEEViewRegionNone;
    NSRect frame = self.frame;
    
    NSRect rect = NSMakeRect(0.0, 0.0, frame.size.width, frame.size.height);
    NSRect left = NSMakeRect(rect.origin.x, rect.origin.y, rect.size.width * 0.2, rect.size.height);
    NSRect right = NSMakeRect(rect.origin.x + rect.size.width * 0.8, rect.origin.y, rect.size.width * 0.2, rect.size.height);
    NSRect top = NSMakeRect(rect.origin.x + rect.size.width * 0.2, rect.origin.y + rect.size.height * 0.8, rect.size.width * 0.6, rect.size.height * 0.2);
    NSRect bottom = NSMakeRect(rect.origin.x + rect.size.width * 0.2, rect.origin.y, rect.size.width * 0.6, rect.size.height * 0.2);
    
    if (NSPointInRect(location, left))
        region = kCEEViewRegionLeft;
    else if (NSPointInRect(location, right))
        region = kCEEViewRegionRight;
    else if (NSPointInRect(location, top))
        region = kCEEViewRegionTop;
    else if (NSPointInRect(location, bottom))
        region = kCEEViewRegionBottom;
    else
        region = kCEEViewRegionEntire;
    
    return region;
}

- (void)highlight:(CEEViewRegion)region {
    
    NSRect bounds = self.bounds;
    NSRect frame0 = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    NSRect frame1 = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    NSSize size = bounds.size;
    
    _highlingthRegion = region;
    
    switch (_highlingthRegion) {
        case kCEEViewRegionLeft:
            frame0 = NSMakeRect(0.0, 0.0, 0.0, size.height);
            frame1 = NSMakeRect(0.0, 0.0, size.width * 0.5, size.height);
            break;
        case kCEEViewRegionRight:
            frame0 = NSMakeRect(size.width, 0.0, 0.0, size.height);
            frame1 = NSMakeRect(size.width * 0.5, 0.0, size.width * 0.5, size.height);
            break;
        case kCEEViewRegionTop:
            frame0 = NSMakeRect(0.0, size.height, size.width, 0.0);
            frame1 = NSMakeRect(0.0, size.height * 0.5, size.width, size.height * 0.5);
            break;
        case kCEEViewRegionBottom:
            frame0 = NSMakeRect(0.0, 0.0, size.width, 0.0);
            frame1 = NSMakeRect(0.0, 0.0, size.width, size.height * 0.5);
            break;
        case kCEEViewRegionEntire:
            frame0 = NSMakeRect(size.width / 2.0, size.height / 2.0, 0.0, 0.0);
            frame1 = bounds;
            break;
        case kCEEViewRegionNone:
            if (_highlightView)
                [_highlightView removeFromSuperview];
            _highlightView = nil;
            return;
            
        default:
            break;
    }
    
    if (_highlightView) {
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.2f;
            [[self->_highlightView animator] setFrame:frame1];
        } completionHandler:^() {
        }];
    }
    else {
        _highlightView = [[CEEHighlightView alloc] initWithFrame:frame0];
        [_highlightView setStyleConfiguration:[self styleConfiguration]];
        [self addSubview:_highlightView];
        
        [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
            context.duration = 0.2f;
            [[self->_highlightView animator] setFrame:frame1];
        } completionHandler:^() {
        }];
    }
}

- (void)setStyleState:(CEEViewStyleState)state {
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
    
    _highlingthRegion = [self highlightRegionFromLocation:[sender draggingLocation]];
    [self highlight:_highlingthRegion];
    return NSDragOperationCopy;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self || ![self attachable])
        return NSDragOperationNone;
    
    CEEViewRegion region = [self highlightRegionFromLocation:[sender draggingLocation]];
    if (_highlingthRegion != region) {
        _highlingthRegion = region;
        [self highlight:_highlingthRegion];
    }
    return NSDragOperationMove;
}

- (void)draggingExited:(nullable id<NSDraggingInfo>)sender {
    _highlingthRegion = kCEEViewRegionNone;
    [self highlight:_highlingthRegion];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    if ([sender draggingSource] == self || ![self attachable])
        return NO;
    
    _highlingthRegion = [self highlightRegionFromLocation:[sender draggingLocation]];
    return YES;
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self || ![self attachable])
        return;
    
    NSPoint location = [self convertPoint:[sender draggingLocation] fromView:nil];
    if (!NSPointInRect(location, self.bounds))
        return;
    
    _highlingthRegion = [self highlightRegionFromLocation:[sender draggingLocation]];
    
    if (!_delegate || _highlingthRegion == kCEEViewRegionNone)
        return;
    
    NSDragOperation dragOperation = [sender draggingSourceOperationMask];
    if (dragOperation == NSDragOperationMove) {
        [_delegate moveFrameView:[sender draggingSource] attachAt:_highlingthRegion relateTo:self];
    }
    else {
        NSPasteboard* pasteboard = [sender draggingPasteboard];
        if ([[pasteboard types] containsObject:NSFilenamesPboardType])
            [_delegate deriveFrameViewWithFilePaths:[pasteboard propertyListForType:NSFilenamesPboardType] attachAt:_highlingthRegion relateTo:self];
    }
    
    _highlingthRegion = kCEEViewRegionNone;
    [self highlight:_highlingthRegion];
}


#pragma mark - protocol NSDraggingSource

- (void)draggingSession:(NSDraggingSession *)session willBeginAtPoint:(NSPoint)screenPoint {
}

- (void)draggingSession:(NSDraggingSession *)session endedAtPoint:(NSPoint)screenPoint operation:(NSDragOperation)operation {
    [[session draggingPasteboard] clearContents];
}

- (NSDragOperation)draggingSession:(NSDraggingSession *)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
    switch(context) {
        case NSDraggingContextOutsideApplication:
            return NSDragOperationCopy;
            break;
            
        case NSDraggingContextWithinApplication:
        default:
            return NSDragOperationMove;
            break;
    }
    return NSDragOperationGeneric;
}

#pragma mark - protocol NSPasteboardWriting
- (NSArray<NSPasteboardType> *)writableTypesForPasteboard:(NSPasteboard *)pasteboard {
    return @[NSPasteboardTypeFileURL];
}

- (NSPasteboardWritingOptions)writingOptionsForType:(NSPasteboardType)type pasteboard:(NSPasteboard *)pasteboard {
    return NSPasteboardWritingPromised;
}

- (NSDraggingItem*)createDraggingItem {
    return [[NSDraggingItem alloc] initWithPasteboardWriter:self];
}

@end
