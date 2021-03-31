//
//  CEETextLabel.m
//  Cymbols
//
//  Created by qing on 2019/12/28.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEETextLabel.h"

@implementation CEETextLabel

- (void)initProperties {
    [super initProperties];
    self.borderColor = [NSColor clearColor];
    self.borderWidth = 1.0;
    self.editable = NO;
    
    cee_text_edit_attributes_configure(_edit, (const cee_uchar*)[[self textAttributesDescriptor] UTF8String]);
    cee_text_edit_wrap_set(self.edit, TRUE);
    cee_text_edit_wrap_with_indent_set(self.edit, FALSE);
}

- (void)setStringValue:(NSString *)string {
    [super setStringValue:string];
    cee_text_storage_modify_clear(cee_text_edit_storage_get(_edit));
}

- (BOOL)becomeFirstResponder {
    return NO;
}

- (void)mouseDown:(NSEvent *)event {
    [[self nextResponder] mouseDown: event];
}

- (void)setFont:(NSFont *)font {
    [super setFont:font];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (void)setTextColor:(NSColor *)textColor {
    [super setTextColor:textColor];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (void)drawRect:(NSRect)dirtyRect {
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
    
    layout = cee_text_edit_layout(_edit);
    if (!layout)
        return;
    
    context = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];
    CGContextSaveGState(context);
    CGContextSetTextPosition(context, 0.0, 0.0);
    
    p = cee_text_layout_lines_get(layout);
    
    while (p) {
        line = p->data;
        base = cee_text_line_base_get(line);
        q = cee_text_line_units_get(line);
        line_bounds = cee_text_line_bounds_get(line);
        
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
            
            if (background_color) {
                CGContextSetFillColorWithColor(context, background_color);
                rect = CGRectMake(position.x, 
                                  position.y - (unit_bounds.size.height - ascent), 
                                  unit_bounds.size.width, 
                                  unit_bounds.size.height);
                CGContextFillRect(context, rect);
            }
            
            if (glyph) {
                CGContextSetFillColorWithColor(context, fore_color);
                CGContextSetStrokeColorWithColor(context, fore_color);
                CTFontDrawGlyphs(font, &glyph, &position, 1, context);
            }
            q = q->next;
        }
        p = p->next;
    }
    
    CGContextRestoreGState(context);
}

- (void)resetCursorRects {
    NSRect rect = NSMakeRect(0.0, 
                             0.0, 
                             self.bounds.size.width, 
                             self.bounds.size.height);
    [self addCursorRect:rect cursor:[NSCursor arrowCursor]];
}

- (void)scrollWheel:(NSEvent *)event {
    [[self nextResponder] scrollWheel: event];
}

@end
