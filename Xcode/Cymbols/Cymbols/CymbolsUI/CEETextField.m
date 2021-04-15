//
//  CEETextField.m
//  Cymbols
//
//  Created by qing on 2019/12/28.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEETextField.h"

NSNotificationName CEENotificationTextViewNewline = @"CEENotificationTextViewNewline";

@implementation CEETextField

- (void)initProperties {
    [super initProperties];
    self.borderColor = [NSColor controlColor];
    self.borderWidth = 1.0;
    cee_text_edit_attributes_configure(_edit, (const cee_uchar*)[[self textAttributesDescriptor] UTF8String]);
    cee_text_edit_aligment_set(self.edit, self.aligment);
    cee_text_edit_wrap_set(_edit, FALSE);
}

- (void)setFont:(NSFont *)font {
    [super setFont:font];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (void)setTextColor:(NSColor *)textColor {
    [super setTextColor:textColor];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (void)insertNewline:(id)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(textViewNewLine:)])
        [self.delegate textViewNewLine:self];
}

- (BOOL)becomeFirstResponder {
    [super becomeFirstResponder];
    return YES;
}

- (BOOL)resignFirstResponder {
    [super resignFirstResponder];
    return YES;
}

- (void)scrollWheel:(NSEvent*)event {
    [[self nextResponder] scrollWheel:event];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    CEETextStorageRef storage = cee_text_edit_storage_get(_edit);
    if (!cee_text_storage_size_get(storage)) {
        if (_placeholderString)
            [self drawPlaceholderString];
        else if (_placeholderIcon)
            [self drawPlaceholderIcon];
    }
}

- (CGPoint)placeholderPosition {
    CEETextLayoutRef layout = NULL;
    CEEList* p = NULL;
    CEEList* q = NULL;
    CEETextLineRef line = NULL;
    CEETextUnitRef unit = NULL;
    CGFloat base = 0.0;
    CEERect line_bounds;
    CEERect unit_bounds;
    CGPoint position = NSMakePoint(0.0, 0.0);
    
    layout = cee_text_edit_layout(self.edit);
    if (!layout)
        goto exit;
    
    p = cee_text_layout_lines_get(layout);
    if (!p)
        goto exit;
    
    line = p->data;
    base = cee_text_line_base_get(line);
    line_bounds = cee_text_line_bounds_get(line);
    
    q = cee_text_line_units_get(line);
    if (!q)
        goto exit;
    
    unit = q->data;
    unit_bounds = cee_text_unit_bounds_get(unit);
    position = (CGPoint){
        line_bounds.origin.x + unit_bounds.origin.x,
        base
    };
    position = [self viewPointFromLayoutPoint:position];
    
exit:
    return position;
}

- (void)drawPlaceholderString {
    if (!_placeholderString)
        return;
    
    CGPoint position = [self placeholderPosition];
    NSMutableDictionary* attributes = NULL;
    NSAttributedString* drawingString = NULL;
    NSSize drawingSize;
    NSRect rect;
        
    attributes = [[NSMutableDictionary alloc] init];
    [attributes setValue:self.font forKey:NSFontAttributeName];
    [attributes setValue:self.alternativeTextColor forKey:NSForegroundColorAttributeName];
    drawingString = [[NSAttributedString alloc] initWithString:_placeholderString attributes:attributes];
    drawingSize = [drawingString size];
    rect = NSMakeRect(0.0, 0.0, self.frame.size.width, self.frame.size.height);
    if (drawingSize.width > rect.size.width) {
        NSInteger subIndex = 1;
        NSString* subjectString = nil;
        while (1) {
            subjectString = [@"..." stringByAppendingString:[_placeholderString substringFromIndex:subIndex]];
            drawingString = [[NSAttributedString alloc] initWithString:subjectString attributes:attributes];
            drawingSize = [drawingString size];
            if (drawingSize.width < rect.size.width)
                break;
            subIndex ++;
        }
    }
    
    rect = NSMakeRect(position.x,
                      position.y,
                      drawingSize.width,
                      self.font.ascender + self.font.descender + 0.5);
    [drawingString drawWithRect:rect options:0 context:nil];
}

- (void)drawPlaceholderIcon {
    if (!_placeholderIcon)
        return;
    
    CGPoint position = [self placeholderPosition];
    CGFloat iconWidth = _placeholderIcon.size.width;
    CGFloat iconHeight = _placeholderIcon.size.height;
    
    if (iconHeight < FLT_EPSILON || iconHeight > (self.frame.size.height - self.borderWidth * 2))
        iconHeight = self.frame.size.height - self.borderWidth * 2;
    iconWidth = iconHeight;
            
    CGRect r0 = CGRectMake(0.0, 0.0, iconWidth, iconHeight);
    CGPoint p0 = CGPointMake(position.x,
                             ((self.frame.size.height - self.borderWidth * 2) - iconHeight) / 2.0 + 0.5);
    [_placeholderIcon drawAtPoint:p0 fromRect:r0 operation:NSCompositingOperationSourceOver fraction:1.0];
    
    
}

@end
