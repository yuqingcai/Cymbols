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

@synthesize stringValue = _stringValue;

- (void)initProperties {
    [super initProperties];
    self.borderColor = [NSColor controlColor];
    self.borderWidth = 1.0;
    cee_text_edit_attributes_configure(_edit, (const cee_uchar*)[[self textAttributesDescriptor] UTF8String]);
    cee_text_edit_aligment_set(self.edit, self.aligment);
    cee_text_edit_wrap_set(_edit, FALSE);
}

- (void)setStringValue:(NSString *)string {
    _stringValue = string;
    cee_text_edit_select_all(_edit);
    cee_text_edit_delete_backward(_edit);
    cee_text_edit_insert(_edit, (const cee_uchar*)[_stringValue UTF8String]);
    [self setNeedsDisplay:YES];
}

- (void)setFont:(NSFont *)font {
    [super setFont:font];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (void)setTextColor:(NSColor *)textColor {
    [super setTextColor:textColor];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (NSString*)stringValue {
    return [NSString stringWithUTF8String:(cee_char*)cee_text_storage_buffer_get(_storage)];
}

- (void)insertNewline:(id)sender {
    if (self.delegate && [self.delegate respondsToSelector:@selector(textViewNewLine:)])
        [self.delegate textViewNewLine:self];
}

@end
