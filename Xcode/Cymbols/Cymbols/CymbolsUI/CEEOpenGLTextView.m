//
//  CEEOpenGLTextView.mm
//  Cymbols
//
//  Created by Qing on 2018/9/7.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <math.h>
#import "CEEStyleManager.h"
#import "CEEOpenGLTextView.h"
#import "cee_text_render_opengl.h"

@interface CEEOpenGLTextView()
@property (strong) NSOpenGLContext* openGLContext;
@property (strong) NSOpenGLPixelFormat* pixelFormat;
@property CEETextRenderOpenGL* render;
@property NSTimer* autoScrollTimer;
@property NSTimer* blinkingTimer;
@property BOOL autoScrolling;
@property BOOL showCursor;
@end

@implementation CEEOpenGLTextView

- (void)initProperties {
    [super initProperties];
    [self setupOpenGLRender];
    _autoScrolling = NO;
    _showCursor = NO;
}

- (void)dealloc {
    [_openGLContext makeCurrentContext];
    
    if (_render)
        cee_text_render_opengl_free(_render);
    
    [NSOpenGLContext clearCurrentContext];
}

- (void)setupOpenGLRender {
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24 ,
        NSOpenGLPFAAlphaSize, 8 ,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFANoRecovery,
        0
    };
    
    if ([self isRetinaDisplay])
        [self setWantsBestResolutionOpenGLSurface:YES];
    
    _pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    _openGLContext = [[NSOpenGLContext alloc] initWithFormat:_pixelFormat shareContext:nil];
    
    [_openGLContext makeCurrentContext];
    
    NSString* glyphVertexShaderPath = [[NSBundle mainBundle] pathForResource:@"glyph_shader_opengl" ofType:@"vs"];
    NSString* glyphVertexShaderString = [NSString stringWithContentsOfFile:glyphVertexShaderPath encoding:NSUTF8StringEncoding error:nil];
    NSString* glyphFragmentShaderPath = [[NSBundle mainBundle] pathForResource:@"glyph_shader_opengl" ofType:@"fs"];
    NSString* glyphFragmentShaderString = [NSString stringWithContentsOfFile:glyphFragmentShaderPath encoding:NSUTF8StringEncoding error:nil];
    NSString* effectsVertexShaderPath = [[NSBundle mainBundle] pathForResource:@"effects_shader_opengl" ofType:@"vs"];
    NSString* effectsVertexShaderString = [NSString stringWithContentsOfFile:effectsVertexShaderPath encoding:NSUTF8StringEncoding error:nil];
    NSString* effectsFragmentShaderPath = [[NSBundle mainBundle] pathForResource:@"effects_shader_opengl" ofType:@"fs"];
    NSString* effectsFragmentShaderString = [NSString stringWithContentsOfFile:effectsFragmentShaderPath encoding:NSUTF8StringEncoding error:nil];
    NSString* solidVertexShaderPath = [[NSBundle mainBundle] pathForResource:@"solid_shader_opengl" ofType:@"vs"];
    NSString* solidVertexShaderString = [NSString stringWithContentsOfFile:solidVertexShaderPath encoding:NSUTF8StringEncoding error:nil];
    NSString* solidFragmentShaderPath = [[NSBundle mainBundle] pathForResource:@"solid_shader_opengl" ofType:@"fs"];
    NSString* solidFragmentShaderString = [NSString stringWithContentsOfFile:solidFragmentShaderPath encoding:NSUTF8StringEncoding error:nil];
    
    _render = cee_text_render_opengl_create([glyphVertexShaderString UTF8String],
                                            [glyphFragmentShaderString UTF8String],
                                            [effectsVertexShaderString UTF8String],
                                            [effectsFragmentShaderString UTF8String],
                                            [solidVertexShaderString UTF8String],
                                            [solidFragmentShaderString UTF8String]);
}

- (BOOL)isFlipped {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)setFrameSize:(NSSize)newSize {
    NSSize oldSize = self.frame.size;
    
    [super setFrameSize:newSize];
    [_openGLContext setView:self];
    [_openGLContext update];
    
    CGFloat factor = [self displayScaleFactor];
    cee_float width = self.frame.size.width * factor;
    cee_float height = self.frame.size.height * factor;
    cee_text_render_opengl_viewport_set(_render, width, height);
    
    if (self.delegate)
        [self.delegate textView:self setFrameSize:newSize];
}

- (CEEColor)convertColor:(NSColor*)color {
    return cee_color_rgba_make(color.redComponent,
                               color.greenComponent,
                               color.blueComponent,
                               color.alphaComponent);
}

- (void)drawRect:(NSRect)dirtyRect {
    if (!self.delegate || !self.dataSource)
        return;
    
    [_openGLContext makeCurrentContext];
    [_openGLContext setView:self];
    [_openGLContext update];
    
    CEEColor textColor = [self convertColor:self.textColor];
    CEEColor backgroundColor = [self convertColor:self.backgroundColor];
    CEEColor textBackgroundColorSelected = [self convertColor:self.textBackgroundColorSelected];
    CEEColor anchorColorMarked = [self convertColor:self.anchorColorMarked];
    CEEColor anchorColor = [self convertColor:self.anchorColor];
    
    cee_text_render_opengl_clear(_render, backgroundColor);
    
    //NSArray<NSString*>* solids = nil;
    //CEEList* rects = NULL;
    //NSRect r;
    //CEERect* rect = NULL;
    //
    //// draw selected region
    //solids = [self.dataSource selectedRectsInTextView:self];
    //for (NSString* rectString in solids) {
    //    r = NSRectFromString(rectString);
    //    rect = cee_rect_create(r.origin.x, r.origin.y, r.size.width, r.size.height);
    //    rects = cee_list_append(rects, rect);
    //}
    //if (rects) {
    //    cee_text_render_opengl_solid(_render, rects, backgroundColor, textBackgroundColorSelected, TRUE);
    //    cee_list_free_full(rects, cee_rect_free);
    //    rects = NULL;
    //}
    //
    //// draw anchors in marked region
    //solids = [self.dataSource anchorRectsInMarkedInTextView:self];
    //for (NSString* rectString in solids) {
    //    r = NSRectFromString(rectString);
    //    rect = cee_rect_create(r.origin.x, r.origin.y, r.size.width, r.size.height);
    //    rects = cee_list_append(rects, rect);
    //}
    //if (rects) {
    //    cee_text_render_opengl_solid(_render, rects, backgroundColor, anchorColorMarked, FALSE);
    //    cee_list_free_full(rects, cee_rect_free);
    //    rects = NULL;
    //}
    //
    //// draw anchors
    //solids = [self.dataSource anchorRectsInTextView:self];
    //for (NSString* rectString in solids) {
    //    NSRect r = NSRectFromString(rectString);
    //    CEERect* rect = cee_rect_create(r.origin.x, r.origin.y, r.size.width, r.size.height);
    //    rects = cee_list_append(rects, rect);
    //}
    //if (rects) {
    //    cee_text_render_opengl_solid(_render, rects, backgroundColor, anchorColor, FALSE);
    //    cee_list_free_full(rects, cee_rect_free);
    //    rects = NULL;
    //}
    
    // draw glyphs and glyphs effects
    //CEETextLayoutRef layout_ref = [self.dataSource textLayoutInTextView:self];
    //cee_text_render_opengl_glyphs(_render, layout_ref, textColor);
    //cee_text_render_opengl_effects(_render, layout_ref, textColor);
    
    [_openGLContext flushBuffer];
}

#pragma mark text edit
- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText {
    return @[
             NSBackgroundColorAttributeName,
             NSForegroundColorAttributeName,
             NSMarkedClauseSegmentAttributeName,
             NSTextAlternativesAttributeName,
             ];
}

- (BOOL)hasMarkedText {
    if ([self.dataSource numberOfMarkedRangeInTextView:self])
        return YES;
    
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

- (CEETextViewScrollDirection)scrollDirection:(NSPoint)point {
    NSInteger x = point.x;
    NSInteger y = point.y;
    NSInteger x0 = 0;
    NSInteger y0 = 0;
    NSInteger x1= self.frame.size.width;
    NSInteger y1= self.frame.size.height;
    
    if (x < x0 && y < y0)
        return kCEETextViewScrollDirectionUpLeft;
    else if (x >= x0 && x <= x1 && y < y0)
        return kCEETextViewScrollDirectionUp;
    else if (x > x1 && y < y0)
        return kCEETextViewScrollDirectionUpRight;
    else if (x < x0 && y >= y0 && y <= y1)
        return kCEETextViewScrollDirectionLeft;
    else if (x > x1 && y >= y0 && y <= y1)
        return kCEETextViewScrollDirectionRight;
    else if (x < x0 && y > y1)
        return kCEETextViewScrollDirectionDownLeft;
    else if (x >= x0 && x <= x1 && y > y1)
        return kCEETextViewScrollDirectionDown;
    else if (x > x1 && y > y1)
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
    
    if (!self.delegate || !self.dataSource)
        return;
    
    //NSEvent* event = [[self window] currentEvent];
    NSPoint point = [self convertPoint:[self.window mouseLocationOutsideOfEventStream] fromView:nil];
    CEETextViewScrollDirection direction = [self scrollDirection:point];
    [self.delegate textView:self scroll:direction];
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
    
    [self.window makeFirstResponder:self];
    
    if (!self.dataSource || !self.delegate)
        return;
    
    NSPoint point;
    NSInteger storageOffset0 = -1;
    NSInteger storageOffset1 = -1;
    BOOL keepOn = YES;
    
    point = [self convertPoint:[event locationInWindow] fromView:nil];
    storageOffset0 = [self.dataSource textLayoutUnitStorageOffsetInTextView:self atPoint:point];
    
    [self.delegate textView:self catchContextAtPoint:point];
    
    if (event.modifierFlags & NSEventModifierFlagShift && event.modifierFlags & NSEventModifierFlagCommand)
        return;
    
    if (storageOffset0 != -1 && self.delegate) {
        
        if (event.modifierFlags & NSEventModifierFlagShift) {
            [self.delegate textView:self dragAnchor:storageOffset0];
        }
        else if (event.modifierFlags & NSEventModifierFlagCommand) {
            [self.delegate textView:self setAnchor:storageOffset0];
        }
        else {
            [self.delegate discardAnchorsInTextView:self];
            [self.delegate textView:self setAnchor:storageOffset0];
        }
    }
    
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
                
                if (!_autoScrolling) {
                    storageOffset1 = [self.dataSource textLayoutUnitStorageOffsetInTextView:self atPoint:point];
                    
                    if (storageOffset1 != -1 && storageOffset0 != storageOffset1 && self.delegate)
                        [self.delegate textView:self dragAnchor:storageOffset1];
                }
                
                break;
                
            case NSEventTypeLeftMouseUp:
                keepOn = NO;
                if (_autoScrolling) {
                    [self stopAutoScroll];
                    _autoScrolling = NO;
                }
                break;
                
            default:
                break;
        }
    }
}

- (void)scrollWheel:(NSEvent *)event {
    
    if (!self.dataSource || !self.delegate)
        return;
    
    if (event.scrollingDeltaY < 0)
        [self.delegate textView:self scroll:kCEETextViewScrollDirectionDown];
    else if (event.scrollingDeltaY > 0)
        [self.delegate textView:self scroll:kCEETextViewScrollDirectionUp];
    
    if (event.scrollingDeltaX < 0)
        [self.delegate textView:self scroll:kCEETextViewScrollDirectionRight];
    else if (event.scrollingDeltaX > 0)
        
    [self.delegate textView:self scroll:kCEETextViewScrollDirectionLeft];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {
    if (self.delegate)
        [self.delegate textView:self insertText:string];
}

- (void)setMarkedText:(id)markedText selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {
    NSString* string = nil;
    if ([markedText isKindOfClass:[NSAttributedString class]])
        string = [markedText string];
    else
        string = markedText;

    if (self.delegate)
        [self.delegate textView:self markText:string selection:selectedRange];
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange {
    NSArray* rects = [self.dataSource anchorRectsInTextView:self];
    NSRect rect = NSRectFromString(rects[0]);
    CGFloat scaleFactor = self.displayScaleFactor;
    NSPoint viewPoint = NSMakePoint((rect.origin.x / scaleFactor), ((rect.origin.y + rect.size.height) / scaleFactor));
    NSPoint windowPoint = [self convertPoint:viewPoint toView:nil];
    NSRect screenRect = [[self window] convertRectToScreen:NSMakeRect(windowPoint.x, windowPoint.y, 40, 40)];
    return screenRect;
}

- (void)keyDown:(NSEvent *)event {
    if (event.keyCode == kVK_Delete) {
        if ([self.dataSource numberOfMarkedCharacterInTextView:self] == 1) {
            [[self inputContext] discardMarkedText];
            [self.delegate unmarkTextInTextView:self];
        }
    }
    else if (event.keyCode == kVK_ForwardDelete) {
        
    }
    [[self inputContext] handleEvent:event];
}

- (BOOL)performKeyEquivalent:(NSEvent *)event {
    return NO;
}

- (void)moveLeft:(id)sender {
    if (self.delegate)
        [self.delegate textView:self anchorMove:kCEETextViewAnchorMoveDirectionLeft];
}

- (void)moveRight:(id)sender {
    if (self.delegate)
        [self.delegate textView:self anchorMove:kCEETextViewAnchorMoveDirectionRight];
}

- (void)moveUp:(id)sender {
    if (self.delegate)
        [self.delegate textView:self anchorMove:kCEETextViewAnchorMoveDirectionUp];
}

- (void)moveDown:(id)sender {
    if (self.delegate)
        [self.delegate textView:self anchorMove:kCEETextViewAnchorMoveDirectionDown];
}

- (void)insertNewline:(id)sender {
    [self.delegate insertNewlineInTextView:self];
}

- (void)insertTab:(id)sender {
    [self.delegate insertTabInTextView:self];
}

- (void)deleteBackward:(id)sender {
    [self.delegate deleteBackwardInTextView:self];
}

- (void)deleteForward:(id)sender {
    [self.delegate deleteForwardInTextView:self];
}

- (void)drawText {
    if (!self.dataSource || !self.delegate)
        return;
    
    [self setNeedsDisplay:YES];
}

- (void)acceptStyleConfiguration:(CEEStyleConfiguration*)configuration {
    // acceptStyleConfiguration called twice, may be a bug.
    // NSLog(@"oops!!");
    [configuration configureOpenGLTextView:self];
    
    //if (_textAttribute)
    //    cee_text_attribute_free(_textAttribute);
    //
    //CEEFontDescriptor* fontDescriptor = [CEEStyleManager CEEFontDescriptorFromNSFont:self.font];
    //fontDescriptor->size *= [self displayScaleFactor];
    //CEEColor* color = [CEEStyleManager CEEColorFromNSColor:self.textColor];
    //_textAttribute = cee_text_attribute_create("default_text_attribute",
    //                                            fontDescriptor,
    //                                            FALSE,
    //                                            FALSE,
    //                                            *color);
    //cee_font_descriptor_free(fontDescriptor);
    //cee_color_free(color);
    //cee_text_render_opengl_atlas_create(_render);
    
    if (self.delegate)
        [self.delegate textAttributeChangedInTextView:self];
    
    [super acceptStyleConfiguration:configuration];
}

- (void)updateStyleConfiguration:(CEEStyleConfiguration*)configuration {
    [configuration configureOpenGLTextView:self];
    [super updateStyleConfiguration:configuration];
}

- (void)resetCursorRects {
    [super resetCursorRects];
    NSRect rect = self.bounds;
    rect.size.width -= 15;
    rect.size.height -= 15;
    
    [self addCursorRect:rect cursor:[NSCursor IBeamCursor]];
}

@end
