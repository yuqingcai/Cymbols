//
//  CEETextView.m
//  Cymbols
//
//  Created by qing on 2019/11/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Carbon/Carbon.h>
#import "CEETextView.h"
#import <math.h>

@interface CEETextView()
@property BOOL autoScrolling;
@property NSTimer* autoScrollTimer;
@property NSTimer* caretBlinkTimer;
@property BOOL caretShow;
@end

typedef enum _CEETextViewScrollDirection {
    kCEETextViewScrollDirectionNone,
    kCEETextViewScrollDirectionUpLeft,
    kCEETextViewScrollDirectionUp,
    kCEETextViewScrollDirectionUpRight,
    kCEETextViewScrollDirectionLeft,
    kCEETextViewScrollDirectionRight,
    kCEETextViewScrollDirectionDownLeft,
    kCEETextViewScrollDirectionDown,
    kCEETextViewScrollDirectionDownRight,
} CEETextViewScrollDirection;

NSRect NSRectFromCEERect(CEERect rect)
{
    NSRect rect0;
    rect0.origin.x = rect.origin.x;
    rect0.origin.y = rect.origin.y;
    rect0.size.width = rect.size.width;
    rect0.size.height = rect.size.height;
    return rect0;
}

CEERect CEERectFromNSRect(NSRect rect)
{
    CEERect rect0;
    rect0.origin.x = rect.origin.x;
    rect0.origin.y = rect.origin.y;
    rect0.size.width = rect.size.width;
    rect0.size.height = rect.size.height;
    return rect0;
}

NSPoint NSPointFromCEEPoint(CEEPoint point)
{
    NSPoint point0;
    point0.x = point.x;
    point0.y = point.y;
    return point0;
}

CEEPoint CEEPointFromNSPoint(NSPoint point)
{
    CEEPoint point0;
    point0.x = point.x;
    point0.y = point.y;
    return point0;
}

@interface CEETextView() {
@protected
   CEETextEditRef _edit;
}
@end

@implementation CEETextView
@synthesize wrap = _wrap;
@synthesize caretBlinkTimeInterval = _caretBlinkTimeInterval;
@synthesize stringValue = _stringValue;
@synthesize alignment = _alignment;

static void pasteboard_string_set(cee_pointer platform_ref, const cee_uchar* str)
{
    [[NSPasteboard generalPasteboard] clearContents];
    [[NSPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:(cee_char*)str] forType:NSPasteboardTypeString];
}

static void pasteboard_string_create(cee_pointer platform_ref, cee_uchar** str)
{
    NSString* string = [[NSPasteboard generalPasteboard] stringForType:NSPasteboardTypeString];
    if (string && str)
        *str = (cee_uchar*)cee_strdup([string UTF8String]);
}

- (BOOL)becomeFirstResponder {
    if (!_editable)
        return NO;
    
    [super setStyleState:kCEEViewStyleStateActived];
    [self startCaretBlink];
    return YES;
}

- (BOOL)resignFirstResponder {
    [super setStyleState:kCEEViewStyleStateDeactived];
    [self stopCaretBlink];
    return YES;
}

- (void)setStyleState:(CEEViewStyleState)state {
    if (self.window.firstResponder != self && _editable)
        state = kCEEViewStyleStateDeactived;
    
    [super setStyleState:state];
}

- (void)initProperties {
    [super initProperties];
    
    _textBackgroundColorSelected = [NSColor selectedTextBackgroundColor];
    _textBackgroundColorSelectedOutline = [NSColor selectedTextBackgroundColor];
    _textBackgroundColorHighlight = [NSColor highlightColor];
    _textBackgroundColorHighlightOutline = [NSColor highlightColor];
    _textBackgroundColorMarked = [NSColor highlightColor];
    _textBackgroundColorMarkedOutline = [NSColor highlightColor];
    _textBackgroundColorMarkedSelected = [NSColor textColor];
    _textBackgroundColorMarkedSelectedOutline = [NSColor textColor];
    _textBackgroundColorSearched = [NSColor lightGrayColor];
    _textBackgroundColorSearchedOutline = [NSColor darkGrayColor];
    _pageGuideLineColor = [NSColor lightGrayColor];
    
    _styleState = kCEEViewStyleStateActived;
    _alignment = kCEETextLayoutAlignmentLeft;
    _editable = YES;
    _wrap = YES;
    _highLightSearched = NO;
    _enablePageGuideLine = YES;
    _enableHighlight = YES;
    _caretBlinkTimeInterval = 0.5;
    _pageGuildLineOffset = 0;
    
    _edit = cee_text_edit_create((__bridge cee_pointer)self,
                                 _alignment,
                                 pasteboard_string_set,
                                 pasteboard_string_create);
    
    CEESize size = cee_size_make(self.frame.size.width, 
                                 self.frame.size.height);
    cee_text_edit_container_size_set(_edit, size);
    cee_text_edit_wrap_set(self.edit, _wrap);
}

- (void)setAlignment:(CEETextLayoutAlignment)aligment {
    _alignment = aligment;
    cee_text_edit_aligment_set(_edit, _alignment);
    [self setNeedsDisplay:YES];
}

- (CEETextLayoutAlignment)alignment {
    return _alignment;
}

- (void)setStringValue:(NSString *)string {
    _stringValue = string;
    cee_text_edit_select_all(_edit);
    cee_text_edit_delete_backward(_edit);
    cee_text_edit_insert(_edit, (const cee_uchar*)[_stringValue UTF8String]);
    [self setNeedsDisplay:YES];
}

- (NSString*)stringValue {
    CEETextStorageRef storage = cee_text_edit_storage_get(_edit);
    return [NSString stringWithUTF8String:(cee_char*)cee_text_storage_buffer_get(storage)];
}

- (void)setWrap:(BOOL)wrap {
    _wrap = wrap;
    cee_text_edit_wrap_set(self.edit, _wrap);
}

- (BOOL)wrap {
    return _wrap;
}

- (void)startCaretBlink {
    _caretShow = YES;
    if (_caretBlinkTimer)
        return;
    
    _caretBlinkTimer = [NSTimer timerWithTimeInterval:_caretBlinkTimeInterval target:self selector:@selector(blinkCaret:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:_caretBlinkTimer forMode:NSRunLoopCommonModes];
}

- (void)stopCaretBlink {
    if (!_caretBlinkTimer)
        return;
    [_caretBlinkTimer invalidate];
    _caretBlinkTimer = nil;
    _caretShow = YES;
    [self setNeedsDisplay:YES];
}

- (BOOL)caretBlinkStop {
    return !_caretBlinkTimer;
}

- (void)setCaretBlinkTimeInterval:(CGFloat)interval {
    [self stopCaretBlink];
    _caretBlinkTimeInterval = interval;
    [self startCaretBlink];
}

- (CGFloat )caretBlinkTimeInterval {
    return _caretBlinkTimeInterval;
}

- (void)blinkCaret:(NSTimer*)timer {
    _caretShow = !_caretShow;
    [self setNeedsDisplay:YES];
}

- (void)resumeBlinkCaret {
    if ([self caretBlinkStop])
        [self startCaretBlink];
}

- (void)setTextAttributesDescriptor:(NSString*)descriptor {
    cee_text_edit_attributes_configure(_edit, (const cee_uchar*)[descriptor UTF8String]);
    [self setNeedsDisplay:YES];
}

- (NSString*)textAttributesDescriptorFromUIContext {
    NSString* fontFamilyName = [self.font familyName];
    CGFloat fontSize = [self.font pointSize];
    NSFontTraitMask fontTrait = [[NSFontManager sharedFontManager] traitsOfFont:self.font];
    CGFloat fontWeight = [[NSFontManager sharedFontManager] weightOfFont:self.font];
    NSString* traitString = @"";
    if (!(fontTrait & (NSBoldFontMask | NSItalicFontMask))) {
        traitString = @"regular";
    }
    else {
        if (fontTrait & NSBoldFontMask)
            traitString = [traitString stringByAppendingString:@"bold "];
        if (fontTrait & NSItalicFontMask)
            traitString = [traitString stringByAppendingString:@"italic "];
    }
    NSString* fontAttribute = [NSString stringWithFormat:@"font(\\\"%@\\\", %.1f, \\\"%@\\\", %.1f)",
                               fontFamilyName, fontSize, traitString, fontWeight];
    NSColor* foreColor = [self.textColor colorUsingColorSpace:[NSColorSpace genericRGBColorSpace]];
    NSString* foreColorAttribute = [NSString stringWithFormat:@"rgba(%ld, %ld, %ld, %ld)",
                                    (NSInteger)(foreColor.redComponent * 255),
                                    (NSInteger)(foreColor.greenComponent * 255),
                                    (NSInteger)(foreColor.blueComponent * 255),
                                    (NSInteger)(foreColor.alphaComponent * 100)];
    NSString* descriptor = [NSString stringWithFormat:@"{ \"font\" : \"%@\", \"forecolor\" : \"%@\" }", fontAttribute, foreColorAttribute];
    return descriptor;
}

- (void)dealloc {
    if (_edit) {
        cee_text_edit_free(_edit);
        _edit = NULL;
    }
        
    if (_caretBlinkTimer) {
        [_caretBlinkTimer invalidate];
        _caretBlinkTimer = nil;
    }
}

- (BOOL)isFlipped {
    return NO;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)resetMouseTraceArea {
    [self removeTrackingArea:self.trackingArea];
    NSTrackingAreaOptions opt = NSTrackingActiveInActiveApp |
        NSTrackingMouseEnteredAndExited |
        NSTrackingMouseMoved |
        NSTrackingEnabledDuringMouseDrag;
    self.trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds options:opt owner:self userInfo:nil];
    [self addTrackingArea:self.trackingArea];
}

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    [self resetMouseTraceArea];
    CEESize size = cee_size_make(self.frame.size.width, self.frame.size.height);
    cee_text_edit_container_size_set(_edit, size);
    [self setNeedsDisplay:YES];
    
    if (_delegate && [_delegate respondsToSelector:@selector(textViewFrameChanged:)])
        [_delegate textViewFrameChanged:self];
}

- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText {
    return @[
        NSBackgroundColorAttributeName,
        NSForegroundColorAttributeName,
        NSMarkedClauseSegmentAttributeName,
        NSTextAlternativesAttributeName,
    ];
}

- (BOOL)hasMarkedText {
    return NO;
}

- (void)unmarkText {
}

- (NSRange)markedRange {
    return NSMakeRange(0, 0);
}

- (NSRange)selectedRange {
    return NSMakeRange(0, 0);
}

- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)range actualRange:(NSRangePointer)actualRange {
    return [[NSAttributedString alloc] initWithString:@"???"];
}

- (void)keyDown:(NSEvent *)event {
    [self stopCaretBlink];
    
    if (!_editable)
        return;
    
    cee_ulong nb_character = 0;
    if (event.keyCode == kVK_Delete) {
        nb_character = cee_text_edit_marked_character_count_get(_edit);
        if (nb_character == 1) {
            [[self inputContext] discardMarkedText];
            cee_text_edit_marked_discard(_edit);
            [self setNeedsDisplay:YES];
        }
    }
    
    [[self inputContext] handleEvent:event];
}

- (void)keyUp:(NSEvent*)event {
    [self startCaretBlink];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {
    cee_text_edit_insert(_edit, (const cee_uchar*)[string UTF8String]);
    [self setNeedsDisplay:YES];

    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
}

- (void)setMarkedText:(id)markedText selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {
    
    NSString* string = nil;
    if ([markedText isKindOfClass:[NSAttributedString class]])
        string = [markedText string];
    else
        string = markedText;
    
    cee_text_edit_mark_text(_edit, (const cee_uchar*)[string UTF8String]);
    
    CEETextStorageRef storage = cee_text_edit_storage_get(_edit);
    CEERange marked_text_range = cee_text_edit_marked_range_get(_edit);
    cee_long paragraph = cee_text_storage_paragraph_beginning_get(storage,
                                                                  marked_text_range.location);
    cee_long buffer_offset = -1;
    cee_ulong length = 0;
    CEERange range = cee_range_make(-1, 0);
    cee_long index =  cee_text_storage_character_index_in_paragraph(storage,
                                                                    paragraph, 
                                                                    marked_text_range.location);
    buffer_offset = cee_text_storage_buffer_offset_by_character_index(storage,
                                                                      paragraph, 
                                                                      index + selectedRange.location);
    range.location = buffer_offset;
    
    for (int i = 0; i < selectedRange.length; i ++) {
        buffer_offset = cee_text_storage_buffer_offset_by_character_index(storage,
                                                                          paragraph, 
                                                                          index + selectedRange.location + i);
        cee_text_storage_buffer_character_next(storage,
                                               buffer_offset,
                                               NULL,
                                               NULL,
                                               &length);
        range.length += length;
    }
    
    cee_text_edit_marked_selection_set(_edit, range);
    [self setNeedsDisplay:YES];
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange {
    NSRect screenRect = [[self window] convertRectToScreen:NSMakeRect(0.0, 0.0, 40, 40)];
    return screenRect;
}

- (NSPoint)layoutPointFromViewPoint:(NSPoint)point {
    return NSMakePoint(point.x, self.frame.size.height - point.y);
}

- (CEETextViewScrollDirection)scrollDirection:(NSPoint)point {
    NSInteger x = point.x;
    NSInteger y = point.y;
    NSInteger x0 = 0;
    NSInteger y0 = 0;
    NSInteger x1 = self.frame.size.width;
    NSInteger y1 = self.frame.size.height;
        
    if (x < x0 && y > y1)
        return kCEETextViewScrollDirectionUpLeft;
    else if (x >= x0 && x <= x1 && y > y1)
        return kCEETextViewScrollDirectionUp;
    else if (x > x1 && y > y1)
        return kCEETextViewScrollDirectionUpRight;
    else if (x < x0 && y >= y0 && y <= y1)
        return kCEETextViewScrollDirectionLeft;
    else if (x > x1 && y >= y0 && y <= y1)
        return kCEETextViewScrollDirectionRight;
    else if (x < x0 && y < y0)
        return kCEETextViewScrollDirectionDownLeft;
    else if (x >= x0 && x <= x1 && y < y0)
        return kCEETextViewScrollDirectionDown;
    else if (x > x1 && y < y0)
        return kCEETextViewScrollDirectionDownRight;
        
    return kCEETextViewScrollDirectionNone;
}

- (BOOL)shouldAutoScroll:(NSPoint)point {
    return [self scrollDirection:point] == kCEETextViewScrollDirectionNone ? NO : YES;
}

- (void)startAutoScroll {
    _autoScrollTimer = [NSTimer timerWithTimeInterval:0.02 target:self selector:@selector(autoScroll:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:_autoScrollTimer forMode:NSRunLoopCommonModes];
}

- (void)stopAutoScroll {
    if (_autoScrollTimer) {
        [_autoScrollTimer invalidate];
        _autoScrollTimer = nil;
    }
}

- (void)autoScroll:(NSTimer*)timer {
    NSPoint point = [self convertPoint:[self.window mouseLocationOutsideOfEventStream] fromView:nil];
    
    CEETextViewScrollDirection direction = [self scrollDirection:point];
    
    if (direction == kCEETextViewScrollDirectionDown) {
        cee_text_edit_scroll_line_down(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionDownRight) {
        cee_text_edit_scroll_line_down(_edit);
        cee_text_edit_scroll_right(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionDownLeft) {
        cee_text_edit_scroll_line_down(_edit);
        cee_text_edit_scroll_left(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionUp) {
        cee_text_edit_scroll_line_up(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionUpLeft) {
        cee_text_edit_scroll_line_up(_edit);
        cee_text_edit_scroll_left(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionUpRight) {
        cee_text_edit_scroll_line_up(_edit);
        cee_text_edit_scroll_right(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionLeft) {
        cee_text_edit_scroll_left(_edit);
    }
    else if (direction == kCEETextViewScrollDirectionRight) {
        cee_text_edit_scroll_right(_edit);
    }
    
    point = [self layoutPointFromViewPoint:point];
    
    CEEPoint p = CEEPointFromNSPoint(point);
    cee_text_edit_caret_position_set(_edit, p);
    cee_text_edit_selection_complete_position_set(_edit, p);
    
    [self setNeedsDisplay:YES];
    
    if (_delegate && [_delegate respondsToSelector:@selector(textViewScrolled:)])
        [_delegate textViewScrolled:self];
}

- (void)mouseDown:(NSEvent *)event {
    
    if (!_editable)
        return;
    
    [super mouseDown:event];
    
    [self stopCaretBlink];
    
    if ([self.window firstResponder] != self)
        [self.window makeFirstResponder:self];
        
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    point = [self layoutPointFromViewPoint:point];
    CEEPoint p = CEEPointFromNSPoint(point);
    
    if (event.clickCount == 2) {
        cee_text_edit_caret_position_set(_edit, p);
        cee_text_edit_selection_anchor_position_set(_edit, p);
        cee_text_edit_select_word(_edit);

        if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
            [_delegate textViewSelectionChanged:self];
        
        if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
            [_delegate textViewCaretSet:self];
        
        [self setNeedsDisplay:YES];
        
        return;
    }
    if (event.clickCount == 3) {
        cee_text_edit_caret_position_set(_edit, p);
        cee_text_edit_selection_anchor_position_set(_edit, p);
        cee_text_edit_select_paragraph(_edit);
        
        if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
            [_delegate textViewSelectionChanged:self];
        
        if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
            [_delegate textViewCaretSet:self];
        
        [self setNeedsDisplay:YES];
        
        return;
    }
    
    cee_text_edit_caret_position_set(_edit, p);
    
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
        
    if (event.modifierFlags & NSEventModifierFlagShift) {
        cee_text_edit_selection_complete_position_set(_edit, p);
        if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
            [_delegate textViewSelectionChanged:self];
    }
    else {
        cee_text_edit_selection_anchor_position_set(_edit, p);
    }
        
    BOOL keepOn = YES;
    
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        point = [self convertPoint:[event locationInWindow] fromView:nil];
        
        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                
                if ([self shouldAutoScroll:point] && !_autoScrolling) {
                    [self startAutoScroll];
                    _autoScrolling = YES;
                }
                else if (![self shouldAutoScroll:point] && _autoScrolling) {
                    [self stopAutoScroll];
                    _autoScrolling = NO;
                }


                if (_delegate && [_delegate respondsToSelector:@selector(textViewDragged:)])
                    [_delegate textViewDragged:self];
                
                if (!_autoScrolling) {
                    point = [self layoutPointFromViewPoint:point];
                    p = CEEPointFromNSPoint(point);
                    
                    cee_text_edit_caret_position_set(_edit, p);
                    cee_text_edit_selection_complete_position_set(_edit, p);
                    
                    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
                        [_delegate textViewCaretSet:self];
                    
                    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
                        [_delegate textViewSelectionChanged:self];
                    
                    [self setNeedsDisplay:YES];
                }
                
                break;
                
            case NSEventTypeLeftMouseUp:
                keepOn = NO;
                if (_autoScrolling) {
                    [self stopAutoScroll];
                    _autoScrolling = NO;
                }
                
                if ((event.modifierFlags & NSEventModifierFlagCommand) &&  
                    _delegate && [_delegate respondsToSelector:@selector(textViewSelectWithCommand:)])
                    [_delegate textViewSelectWithCommand:self];
                
                [self startCaretBlink];
                
                break;
                
            default:
                break;
        }
    }
    
    [self setNeedsDisplay:YES];
}

- (NSPoint)viewPointFromLayoutPoint:(NSPoint)point {
    return NSMakePoint(point.x, self.frame.size.height - point.y);
}

- (void)drawContinuousRegion:(CEEList*)rects {
    CEEList* points = cee_rects_outline_create(rects);
    cee_ulong count = cee_list_length(points);
    
    NSBezierPath* path = [NSBezierPath bezierPath];
    
    if (!count)
        return;
    
    NSUInteger groupCount = count / 4;
    cee_uint i = 0;
    CEEPoint* point = NULL;
    NSPoint p1;
    NSPoint p2;
    NSPoint c1;
    NSPoint c2;
    
    point = cee_list_nth_data(points, i*4+0);
    p1 = NSPointFromCEEPoint(*point);
    p1 = [self viewPointFromLayoutPoint:p1];
    
    point = cee_list_nth_data(points, i*4+1);
    p2 = NSPointFromCEEPoint(*point);
    p2 = [self viewPointFromLayoutPoint:p2];
    
    c1 = NSMakePoint(FLT_MAX, FLT_MAX);
    point = cee_list_nth_data(points, i*4+2);
    if (point) {
        c1 = NSPointFromCEEPoint(*point);
        c1 = [self viewPointFromLayoutPoint:c1];
    }
    
    c2 = NSMakePoint(FLT_MAX, FLT_MAX);
    point = cee_list_nth_data(points, i*4+3);
    if (point) {
        c2 = NSPointFromCEEPoint(*point);
        c2 = [self viewPointFromLayoutPoint:c2];
    }
    
    [path moveToPoint:p1];
    
    if (c1.x == FLT_MAX)
        [path lineToPoint:p2];
    else
        [path curveToPoint:p2 controlPoint1:c1 controlPoint2:c2];
    
    for (i = 1; i < groupCount; i ++) {

        point = cee_list_nth_data(points, i*4+0);
        p1 = NSPointFromCEEPoint(*point);
        p1 = [self viewPointFromLayoutPoint:p1];
        
        point = cee_list_nth_data(points, i*4+1);
        p2 = NSPointFromCEEPoint(*point);
        p2 = [self viewPointFromLayoutPoint:p2];
        
        c1 = NSMakePoint(FLT_MAX, FLT_MAX);
        point = cee_list_nth_data(points, i*4+2);
        if (point) {
            c1 = NSPointFromCEEPoint(*point);
            c1 = [self viewPointFromLayoutPoint:c1];
        }
        
        c2 = NSMakePoint(FLT_MAX, FLT_MAX);
        point = cee_list_nth_data(points, i*4+3);
        if (point) {
            c2 = NSPointFromCEEPoint(*point);
            c2 = [self viewPointFromLayoutPoint:c2];
        }
        
        [path lineToPoint:p1];
        
        if (c1.x == FLT_MAX)
            [path lineToPoint:p2];
        else
            [path curveToPoint:p2 controlPoint1:c1 controlPoint2:c2];
    }
    
    [path closePath];
    
    [path setLineWidth:1.0];
    [path stroke];
    [path fill];
    
    cee_list_free_full(points, cee_point_free);
}

- (void)drawRegions:(CEEList*)rects {
    CEEList* p = rects;
    while (p) {
        CEERect* rect = p->data;
        NSPoint origin = [self viewPointFromLayoutPoint:NSMakePoint(rect->origin.x, rect->origin.y)];
        if (rect->size.width < FLT_EPSILON)
            rect->size.width = 2.0;
        
        NSRect _rect = NSMakeRect(origin.x, 
                                  origin.y - rect->size.height, 
                                  rect->size.width, 
                                  rect->size.height);
        NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:_rect xRadius:2.0 yRadius:2.0];
        [path setLineWidth:1.0];
        [path stroke];
        [path fill];
        p = p->next;
    }
}

- (void)drawSelectionRegions {
    CEETextSelection* selection = cee_text_edit_selection_get(_edit);
    CEETextLayoutRef layout = cee_text_edit_layout_get(_edit);
    
    if (!selection->range.length)
        return;
    
    CEEList* rects = cee_text_layout_rects_create(layout, selection->range);
    if (!rects)
        return;
    
    [_textBackgroundColorSelectedOutline setStroke];
    [_textBackgroundColorSelected setFill];
    [self drawContinuousRegion:rects];
    cee_list_free_full(rects, cee_range_free);
}

- (void)drawMarkedRegions {
    CEETextMarked* marked = cee_text_edit_marked_get(_edit);
    CEETextLayoutRef layout = cee_text_edit_layout_get(_edit);
    
    if (!marked->range.length)
        return;
        
    CEEList* rects = cee_text_layout_rects_create(layout, marked->range);
    if (!rects)
        return;
    
    [_textBackgroundColorMarkedOutline setStroke];
    [_textBackgroundColorMarked setFill];
    [self drawContinuousRegion:rects];
    cee_list_free_full(rects, cee_range_free);
}

- (void)drawMarkedSelectedRegions {
    CEETextMarked* marked = cee_text_edit_marked_get(_edit);
    CEETextLayoutRef layout = cee_text_edit_layout_get(_edit);
    
    if (!marked->range.length)
        return;
    
    CEEList* rects = cee_text_layout_rects_create(layout, marked->selected);
    if (!rects)
        return;
        
    cee_ulong count = cee_list_length(rects);
    if (!count)
        return;
    
    [_textBackgroundColorMarkedSelectedOutline setStroke];
    [_textBackgroundColorMarkedSelected setFill];
    
    CEEList* p = rects;
    while (p) {
        NSRect rect = NSRectFromCEERect(*(CEERect*)p->data);
        
        rect.origin = [self viewPointFromLayoutPoint:rect.origin];
        rect.origin.y -= rect.size.height;
        
        if (rect.size.width < FLT_EPSILON) {
            
            NSBezierPath* path = [NSBezierPath bezierPath];
            NSPoint p0 = rect.origin;
            NSPoint p1 = rect.origin;
            p1.y = p0.y + rect.size.height;
            [path moveToPoint:p0];
            [path lineToPoint:p1];
            [path setLineWidth:1.0];
            [path stroke];
        }
        else {
            NSRectFill(rect);
        }
        
        p = p->next;
    }
    
    cee_list_free_full(rects, cee_rect_free);
}

- (void)drawSearchedRegions {
    if (!_delegate || ![_delegate respondsToSelector:@selector(textViewSearchRanges:)])
        return;
    
    CEETextLayoutRef layout = cee_text_edit_layout_get(_edit);
    
    CEEList* rects = NULL;
    CEEList* p = [_delegate textViewSearchRanges:self];
    while (p) {
        CEERange* range = p->data;
        CEEList* q = cee_text_layout_rects_create(layout, *range);
        rects = cee_list_concat(rects, q);
        p = p->next;
    }
    
    [_textBackgroundColorSearchedOutline setStroke];
    [_textBackgroundColorSearched setFill];
    [self drawRegions:rects];
    cee_list_free_full(rects, cee_range_free);
}

- (void)drawHighlightRegions {
    if (!_delegate || ![_delegate respondsToSelector:@selector(textViewHighlightRanges:)])
          return;
        
    CEETextLayoutRef layout = cee_text_edit_layout_get(_edit);
    
    CEEList* rects = NULL;
    CEEList* p = [_delegate textViewHighlightRanges:self];
    while (p) {
        CEERange* range = p->data;
        CEEList* q = cee_text_layout_rects_create(layout, *range);
        rects = cee_list_concat(rects, q);
        p = p->next;
    }
    
    if (!rects)
        return;
    
    [_textBackgroundColorHighlightOutline setStroke];
    [_textBackgroundColorHighlight setFill];
    [self drawRegions:rects];
    cee_list_free_full(rects, cee_range_free);
}

- (void)drawPageGuideLine {
    
    if (_pageGuildLineOffset <= 0)
        return;
    
    CEETextLayoutRef layout = cee_text_edit_layout_get(_edit);
    cee_pointer platform = cee_text_layout_platform_get(layout);
    CGFloat fontTypeWidth = cee_text_platform_width_get(platform, 'a', kCEETagTypePlainText);
    CGFloat horizontalOffset = cee_text_layout_horizontal_offset_get(layout);
    NSBezierPath* path = [NSBezierPath bezierPath];
    [_pageGuideLineColor setStroke];
    [path setLineWidth:1.0];
    [path moveToPoint:NSMakePoint((_pageGuildLineOffset + 1) * fontTypeWidth - horizontalOffset, 0.0)];
    [path lineToPoint:NSMakePoint((_pageGuildLineOffset + 1) * fontTypeWidth - horizontalOffset, self.frame.size.height)];
    [path stroke];
}

- (void)drawCaretAtUnit:(CEETextUnitRef)unit inLine:(CEETextLineRef)line {
    CEERect line_bounds;
    CEERect caret_bounds;
    CEETextLayoutRef layout = NULL;
    cee_float line_space = 0;
    NSRect rect;
    cee_pointer platform = NULL;
       
    layout = cee_text_edit_layout_get(_edit);
    if (!layout)
        return;
    
    platform = cee_text_layout_platform_get(layout);
    line_space = cee_text_layout_line_space_get(layout);
    line_bounds = cee_text_line_bounds_get(line);
    caret_bounds = cee_text_unit_bounds_get(unit);
    caret_bounds.origin.x += line_bounds.origin.x;
    caret_bounds.origin.y = line_bounds.origin.y - line_space;
    caret_bounds.size.height = line_bounds.size.height + line_space;
    
    rect = NSMakeRect(caret_bounds.origin.x - 2.0, 
                      self.frame.size.height - caret_bounds.origin.y - caret_bounds.size.height,
                      2.0, 
                      caret_bounds.size.height);
    CGColorRef color = (CGColorRef)cee_text_platform_fore_color_get(platform,
                                                                    -1, 
                                                                    0);
    NSColor* caretColor = [NSColor colorWithCGColor:color];
    [caretColor setFill];
    NSRectFill(rect);
}

- (void)drawUnderlineAtUnit:(CEETextUnitRef)unit inLine:(CEETextLineRef)line {
    NSBezierPath* path = nil;
    CEERect line_bounds;
    CEETextLayoutRef layout = NULL;
    CGPoint position;
    cee_float ascent = 0.0;
    CEERect unit_bounds;
    CGFloat base = 0.0;
    
    layout = cee_text_edit_layout_get(_edit);
    if (!layout)
        return;

    base = cee_text_line_base_get(line);
    line_bounds = cee_text_line_bounds_get(line);
    unit_bounds = cee_text_unit_bounds_get(unit);
    ascent = cee_text_unit_ascent_get(unit);
    position = (CGPoint){ 
        line_bounds.origin.x + unit_bounds.origin.x,
        base
    };
    position = [self viewPointFromLayoutPoint:position];
    
    path = [[NSBezierPath alloc] init];
    position.y -= (unit_bounds.size.height - ascent);
    position.y += 1.0;
    [path moveToPoint:position];
    position.x += unit_bounds.size.width;
    [path lineToPoint:position];
    [path stroke];
}

- (void)drawUnitRect:(NSRect)rect {
    NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:1.0 yRadius:1.0];
    [[NSColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:0.3] setStroke];
    [[NSColor colorWithRed:1.0 green:0.0 blue:0.0 alpha:0.2] setFill];
    [path stroke];
    [path fill];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    NSInteger caret_offset = 0;
    CEERect line_bounds;
    CGContextRef context = NULL;
    CEETextLineRef line = NULL;
    CGFloat base = 0.0;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEETextUnitRef unit = NULL;
    CGGlyph glyph = 0;
    CTFontRef font = NULL;
    CGColorRef fore_color = NULL;
    CGColorRef background_color = NULL;
    CEERect unit_bounds;
    cee_long buffer_offset = 0;
    CGPoint position;
    cee_float ascent = 0.0;
    cee_boolean under_line = FALSE;
    CEETextLayoutRef layout = NULL;
    CGRect rect;
    cee_pointer platform = NULL;
    BOOL drawString = NO;
    
    layout = cee_text_edit_layout_get(_edit);
    if (!layout)
        return;
    
    context = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];
    CGContextSaveGState(context);
    CGContextSetTextPosition(context, 0.0, 0.0);
    
    platform = cee_text_layout_platform_get(layout);
    background_color = (CGColorRef)cee_text_platform_background_color_get(platform, -1, 0);
    if (background_color) {
        CGContextSetFillColorWithColor(context, background_color);
        rect = CGRectMake(0.0,
                          0.0,
                          self.frame.size.width,
                          self.frame.size.height);
        CGContextFillRect(context, rect);
    }
    
    if (_enablePageGuideLine)
        [self drawPageGuideLine];
    
    [self drawSelectionRegions];
    [self drawMarkedRegions];
    [self drawMarkedSelectedRegions];
    
    if (_highLightSearched)
        [self drawSearchedRegions];
    
    if (_enableHighlight)
        [self drawHighlightRegions];
    
    caret_offset = cee_text_edit_caret_buffer_offset_get(_edit);
    p = cee_text_layout_lines_get(layout);
    
    while (p) {
        line = p->data;
        base = cee_text_line_base_get(line);
        line_bounds = cee_text_line_bounds_get(line);
        
        q = cee_text_line_units_get(line);
        while (q) {
            unit = q->data;
            glyph = (CGGlyph)cee_text_unit_glyph_get(unit);
            font = (CTFontRef)cee_text_unit_font_get(unit);
            fore_color = (CGColorRef)cee_text_unit_fore_color_get(unit);
            background_color = (CGColorRef)cee_text_unit_background_color_get(unit);
            unit_bounds = cee_text_unit_bounds_get(unit);
            buffer_offset = cee_text_unit_buffer_offset_get(unit);
            position = (CGPoint){ 
                line_bounds.origin.x + unit_bounds.origin.x,
                base
            };
            ascent = cee_text_unit_ascent_get(unit);
            under_line = cee_text_unit_under_line_get(unit);
            position = [self viewPointFromLayoutPoint:position];
            
            if ([self.window isKeyWindow] &&
                self.window.firstResponder == self && _editable &&
                self.styleState == kCEEViewStyleStateActived &&
                buffer_offset == caret_offset && _caretShow)
                [self drawCaretAtUnit:unit inLine:line];
            
            rect = CGRectMake(position.x,
                              position.y - (unit_bounds.size.height - ascent),
                              unit_bounds.size.width,
                              unit_bounds.size.height);
            
            if (background_color) {
                CGContextSetFillColorWithColor(context, background_color);
                CGContextFillRect(context, rect);
            }
            
            //[self drawUnitRect:rect];
            
            if (glyph) {
                CGContextSetFillColorWithColor(context, fore_color);
                CGContextSetStrokeColorWithColor(context, fore_color);
                CTFontDrawGlyphs(font, &glyph, &position, 1, context);
                if (under_line)
                    [self drawUnderlineAtUnit:unit inLine:line];
                
                if (!drawString)
                    drawString = TRUE;
            }
            q = q->next;
        }
        p = p->next;
    }
    
    CGContextRestoreGState(context);
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
        
    if (current.textBackgroundColorSelected)
        self.textBackgroundColorSelected = current.textBackgroundColorSelected;
    
    if (current.textBackgroundColorSelectedOutline)
        self.textBackgroundColorSelectedOutline = current.textBackgroundColorSelectedOutline;
        
    if (current.textBackgroundColorSearched)
        self.textBackgroundColorSearched = [current.textBackgroundColorSearched colorWithAlphaComponent:0.7];
    
    if (current.textBackgroundColorSearchedOutline)
        self.textBackgroundColorSearchedOutline = current.textBackgroundColorSearchedOutline;
    
    if (current.textBackgroundColorHighlight)
        self.textBackgroundColorHighlight = [current.textBackgroundColorHighlight colorWithAlphaComponent:0.7];
        
    if (current.textBackgroundColorHighlightOutline)
        self.textBackgroundColorHighlightOutline = current.textBackgroundColorHighlightOutline;
        
    if (current.textBackgroundColorMarked)
        self.textBackgroundColorMarked = [current.textBackgroundColorMarked colorWithAlphaComponent:0.7];
    
    if (current.textBackgroundColorMarkedOutline)
        self.textBackgroundColorMarkedOutline = current.textBackgroundColorMarkedOutline;
    
    if (current.textBackgroundColorMarkedSelected)
        self.textBackgroundColorMarkedSelected = [current.textBackgroundColorMarkedSelected colorWithAlphaComponent:0.7];
    
    if (current.textBackgroundColorMarkedSelectedOutline)
        self.textBackgroundColorMarkedSelectedOutline = current.textBackgroundColorMarkedSelectedOutline;
    
    if (current.alternativeTextColor)
        self.alternativeTextColor = current.alternativeTextColor;
    
    if (current.pageGuideLineColor)
        self.pageGuideLineColor = current.pageGuideLineColor;
            
    if (current.aligment) {
        if ([current.aligment caseInsensitiveCompare:@"left"] == NSOrderedSame)
            self.alignment = kCEETextLayoutAlignmentLeft;
        else if ([current.aligment caseInsensitiveCompare:@"right"] == NSOrderedSame)
            self.alignment = kCEETextLayoutAlignmentRight;
        else if ([current.aligment caseInsensitiveCompare:@"center"] == NSOrderedSame)
            self.alignment = kCEETextLayoutAlignmentCenter;
    }
}

// move caret
- (void)moveRight:(id)sender {
    cee_text_edit_caret_move_right(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveLeft:(id)sender {
    cee_text_edit_caret_move_left(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveUp:(id)sender {
    cee_text_edit_caret_move_up(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveDown:(id)sender {
    cee_text_edit_caret_move_down(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordRight:(id)sender {
    cee_text_edit_caret_move_word_right(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordLeft:(id)sender {
    cee_text_edit_caret_move_word_left(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveForward:(id)sender {
    cee_text_edit_caret_move_right(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveBackward:(id)sender {
    cee_text_edit_caret_move_left(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordForward:(id)sender {
    cee_text_edit_caret_move_word_right(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordBackward:(id)sender {
    cee_text_edit_caret_move_word_left(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToLeftEndOfLine:(id)sender {
    cee_text_edit_caret_move_paragraph_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToRightEndOfLine:(id)sender {
    cee_text_edit_caret_move_paragraph_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToBeginningOfLine:(id)sender {
    cee_text_edit_caret_move_paragraph_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToEndOfLine:(id)sender {
    cee_text_edit_caret_move_paragraph_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToBeginningOfParagraph:(id)sender {
    cee_text_edit_caret_move_paragraph_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToEndOfParagraph:(id)sender {
    cee_text_edit_caret_move_paragraph_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToBeginningOfDocument:(id)sender {
    cee_text_edit_caret_move_document_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToEndOfDocument:(id)sender {
    cee_text_edit_caret_move_document_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)pageDown:(id)sender {
    cee_text_edit_page_down(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

- (void)pageUp:(id)sender {
    cee_text_edit_page_up(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewCaretSet:)])
        [_delegate textViewCaretSet:self];
    
    [self resumeBlinkCaret];
}

// move caret and modify selection
- (void)moveRightAndModifySelection:(id)sender {
    cee_text_edit_caret_move_right_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveLeftAndModifySelection:(id)sender {
    cee_text_edit_caret_move_left_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveUpAndModifySelection:(id)sender {
    cee_text_edit_caret_move_up_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveDownAndModifySelection:(id)sender {
    cee_text_edit_caret_move_down_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordRightAndModifySelection:(id)sender {
    cee_text_edit_caret_move_word_right_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    if ([self caretBlinkStop])
        [self startCaretBlink];
}

- (void)moveWordLeftAndModifySelection:(id)sender {
    cee_text_edit_caret_move_word_left_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToLeftEndOfLineAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_beginning_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToRightEndOfLineAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_end_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveForwardAndModifySelection:(id)sender {
    cee_text_edit_caret_move_right_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveBackwardAndModifySelection:(id)sender {
    cee_text_edit_caret_move_left_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordForwardAndModifySelection:(id)sender {
    cee_text_edit_caret_move_word_right_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveWordBackwardAndModifySelection:(id)sender {
    cee_text_edit_caret_move_word_left_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToBeginningOfLineAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_beginning_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToEndOfLineAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_end_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToBeginningOfParagraphAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_beginning_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToEndOfParagraphAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_end_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToBeginningOfDocumentAndModifySelection:(id)sender {
    cee_text_edit_caret_move_document_beginning_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveToEndOfDocumentAndModifySelection:(id)sender {
    cee_text_edit_caret_move_document_end_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveParagraphForwardAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_forward_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)moveParagraphBackwardAndModifySelection:(id)sender {
    cee_text_edit_caret_move_paragraph_backward_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)pageDownAndModifySelection:(id)sender {
    cee_text_edit_page_down_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)pageUpAndModifySelection:(id)sender {
    cee_text_edit_page_up_selection(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChangedWhenCaretMove:)])
        [_delegate textViewSelectionChangedWhenCaretMove:self];
    
    [self resumeBlinkCaret];
}

- (void)scrollLineUp:(id)sender {
    cee_text_edit_scroll_line_up(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewScrolled:)])
        [_delegate textViewScrolled:self];
    
    [self resumeBlinkCaret];
}

- (void)scrollLineDown:(id)sender {
    cee_text_edit_scroll_line_down(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewScrolled:)])
        [_delegate textViewScrolled:self];
    
    [self resumeBlinkCaret];
}

- (void)scrollToBeginningOfDocument:(id)sender {
    cee_text_edit_caret_move_document_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewScrolled:)])
        [_delegate textViewScrolled:self];
    
    [self resumeBlinkCaret];
}

- (void)scrollToEndOfDocument:(id)sender {
    cee_text_edit_caret_move_document_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewScrolled:)])
        [_delegate textViewScrolled:self];
    
    [self resumeBlinkCaret];
}

- (void)selectAll:(id)sender {
    cee_text_edit_select_all(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
        [_delegate textViewSelectionChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)selectParagraph:(id)sender {
    cee_text_edit_select_paragraph(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
        [_delegate textViewSelectionChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)selectLine:(id)sender {
    cee_text_edit_select_paragraph(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
        [_delegate textViewSelectionChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)selectWord:(id)sender {
    cee_text_edit_select_word(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewSelectionChanged:)])
        [_delegate textViewSelectionChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)insertTab:(id)sender {
    cee_text_edit_insert(_edit, (const cee_uchar*)"\t");
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)insertNewline:(id)sender {
    cee_text_edit_insert(_edit, (const cee_uchar*)"\n");
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteForward:(id)sender {
    cee_text_edit_delete_forward(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteBackward:(id)sender {
    cee_text_edit_delete_backward(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteWordForward:(id)sender {
    cee_text_edit_delete_word_forward(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteWordBackward:(id)sender {
    cee_text_edit_delete_word_backward(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteToBeginningOfLine:(id)sender {
    cee_text_edit_delete_to_paragraph_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteToEndOfLine:(id)sender {
    cee_text_edit_delete_to_paragraph_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteToBeginningOfParagraph:(id)sender {
    cee_text_edit_delete_to_paragraph_beginning(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)deleteToEndOfParagraph:(id)sender {
    cee_text_edit_delete_to_paragraph_end(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)copy:(id)sender {
    cee_text_edit_copy(_edit);
    [self setNeedsDisplay:YES];
    
    [self resumeBlinkCaret];
}

- (void)cut:(id)sender {
    cee_text_edit_cut(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)paste:(id)sender {
    cee_text_edit_paste(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)undo:(id)sender {
    cee_text_edit_undo(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)redo:(id)sender {
    cee_text_edit_redo(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(textViewTextChanged:)])
        [_delegate textViewTextChanged:self];
    
    [self resumeBlinkCaret];
}

- (void)scrollWheel:(NSEvent *)event {
    if (event.scrollingDeltaY < 0)
        [self scrollLineDown:self];
    else if (event.scrollingDeltaY > 0)
        [self scrollLineUp:self];
}

- (void)resetCursorRects {
    [super resetCursorRects];
    NSRect rect = NSMakeRect(0.0, 
                             0.0, 
                             self.bounds.size.width, 
                             self.bounds.size.height);
    [self addCursorRect:rect cursor:[NSCursor IBeamCursor]];
}

- (void)cancelOperation:(id)sender {
    if (_delegate && [_delegate respondsToSelector:@selector(textViewEscape:)])
        [_delegate textViewEscape:self];
}

- (IBAction)find:(id)sender {
    if (self.editable && _delegate && [_delegate respondsToSelector:@selector(textViewSearchText:)])
        [_delegate textViewSearchText:self];
}

- (void)mouseMoved:(NSEvent *)event {
    [super mouseMoved:event];
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    point = [self layoutPointFromViewPoint:point];
    
    if (event.modifierFlags & NSEventModifierFlagCommand) {
        if (_delegate && [_delegate respondsToSelector:@selector(textView:pickWordAtPoint:)])
            [_delegate textView:self pickWordAtPoint:point];
    }
}

- (void)flagsChanged:(NSEvent *)event {
    [super flagsChanged:event];
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    point = [self layoutPointFromViewPoint:point];
    
    if (event.modifierFlags & NSEventModifierFlagCommand) {
        if (_delegate && [_delegate respondsToSelector:@selector(textView:pickWordAtPoint:)])
            [_delegate textView:self pickWordAtPoint:point];
    }
    else if (!(event.modifierFlags & NSEventModifierFlagCommand)) {
        if (_delegate && [_delegate respondsToSelector:@selector(textView:pickWordCompleteAtPoint:)])
            [_delegate textView:self pickWordCompleteAtPoint:point];
    }
}

- (NSMenu*)menuForEvent:(NSEvent *)event {
    if (!_editable)
        return nil;
    
    NSMenu* menu = [super menuForEvent:event];
    if (_delegate && [_delegate respondsToSelector:@selector(textView:modifyMenu:)])
        [_delegate textView:self modifyMenu:&menu];
    return menu;
}

@end
