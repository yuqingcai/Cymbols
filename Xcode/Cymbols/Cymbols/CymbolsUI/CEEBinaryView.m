//
//  CEEBinaryView.m
//  Cymbols
//
//  Created by qing on 2020/10/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEBinaryView.h"

@implementation CEEBinaryView {
@protected
   CEEBinaryEditRef _edit;
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
    
    _edit = cee_binary_edit_create((__bridge cee_pointer)self);
    CEESize size = cee_size_make(self.frame.size.width,
                                 self.frame.size.height);
    cee_binary_edit_container_size_set(_edit, size);
}

- (void)setTextAttributesDescriptor:(NSString*)descriptor {
    cee_binary_edit_attributes_configure(_edit, (const cee_uchar*)[descriptor UTF8String]);
    [self setNeedsDisplay:YES];
}

- (void)dealloc {
    if (_edit) {
        cee_binary_edit_free(_edit);
        _edit = NULL;
    }
}

- (void)setFrameSize:(NSSize)newSize {
    [super setFrameSize:newSize];
    CEESize size = cee_size_make(self.frame.size.width, self.frame.size.height);
    cee_binary_edit_container_size_set(_edit, size);
    [self setNeedsDisplay:YES];
    
    if (_delegate && [_delegate respondsToSelector:@selector(binaryViewFrameChanged:)])
        [_delegate binaryViewFrameChanged:self];
}

- (NSPoint)viewPointFromLayoutPoint:(NSPoint)point {
    return NSMakePoint(point.x, self.frame.size.height - point.y);
}

- (void)drawUnderlineAtUnit:(CEEBinaryUnitRef)unit inLine:(CEEBinaryLineRef)line {
    NSBezierPath* path = nil;
    CEERect line_bounds;
    CEEBinaryLayoutRef layout = NULL;
    CGPoint position;
    cee_float ascent = 0.0;
    CEERect unit_bounds;
    CGFloat base = 0.0;
    
    layout = cee_binary_edit_layout_get(_edit);
    if (!layout)
        return;

    base = cee_binary_line_base_get(line);
    line_bounds = cee_binary_line_bounds_get(line);
    unit_bounds = cee_binary_unit_bounds_get(unit);
    ascent = cee_binary_unit_ascent_get(unit);
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

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    CEERect line_bounds;
    CGContextRef context = NULL;
    CEEBinaryLineRef line = NULL;
    CGFloat base = 0.0;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEEBinaryUnitRef unit = NULL;
    CGGlyph glyph = 0;
    CTFontRef font = NULL;
    CGColorRef fore_color = NULL;
    CGColorRef background_color = NULL;
    CEERect unit_bounds;
    CGPoint position;
    cee_float ascent = 0.0;
    cee_boolean under_line = FALSE;
    CEEBinaryLayoutRef layout = NULL;
    CGRect rect;
    cee_pointer platform = NULL;
    BOOL drawString = NO;
    
    layout = cee_binary_edit_layout_get(_edit);
    if (!layout)
        return;
    
    context = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];
    CGContextSaveGState(context);
    CGContextSetTextPosition(context, 0.0, 0.0);
    
    platform = cee_binary_layout_platform_get(layout);
    background_color = (CGColorRef)cee_text_platform_background_color_get(platform, -1, 0);
    if (background_color) {
        CGContextSetFillColorWithColor(context, background_color);
        rect = CGRectMake(0.0,
                          0.0,
                          self.frame.size.width,
                          self.frame.size.height);
        CGContextFillRect(context, rect);
    }
    
    p = cee_binary_layout_lines_get(layout);
    
    while (p) {
        line = p->data;
        base = cee_binary_line_base_get(line);
        line_bounds = cee_binary_line_bounds_get(line);
        
        q = cee_binary_line_units_get(line);
        while (q) {
            unit = q->data;
            glyph = (CGGlyph)cee_binary_unit_glyph_get(unit);
            font = (CTFontRef)cee_binary_unit_font_get(unit);
            fore_color = (CGColorRef)cee_binary_unit_fore_color_get(unit);
            background_color = (CGColorRef)cee_binary_unit_background_color_get(unit);
            unit_bounds = cee_binary_unit_bounds_get(unit);
            position = (CGPoint){
                line_bounds.origin.x + unit_bounds.origin.x,
                base
            };
            ascent = cee_binary_unit_ascent_get(unit);
            under_line = cee_binary_unit_under_line_get(unit);
            position = [self viewPointFromLayoutPoint:position];
                        
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

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
    
    if ([self.window firstResponder] != self)
        [self.window makeFirstResponder:self];
}

- (void)scrollWheel:(NSEvent *)event {
    if (event.scrollingDeltaY < 0)
        [self scrollLineDown:self];
    else if (event.scrollingDeltaY > 0)
        [self scrollLineUp:self];
}

- (void)scrollLineUp:(id)sender {
    cee_binary_edit_scroll_line_up(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(binaryViewScrolled:)])
        [_delegate binaryViewScrolled:self];
}

- (void)scrollLineDown:(id)sender {
    cee_binary_edit_scroll_line_down(_edit);
    [self setNeedsDisplay:YES];
    if (_delegate && [_delegate respondsToSelector:@selector(binaryViewScrolled:)])
        [_delegate binaryViewScrolled:self];
}

@end
