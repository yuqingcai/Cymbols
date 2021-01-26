//
//  CEEBinaryView.m
//  Cymbols
//
//  Created by qing on 2020/10/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEBinaryView.h"

@implementation CEEBinaryView

- (void)initProperties {
    [super initProperties];
    cee_text_edit_wrap_set(_edit, YES);
    cee_text_edit_insert(_edit, (const cee_uchar*)[@"It seems you open a binary file, Cymbols does't support binary file yet.\nIf the file you opened is a text file but not encoding in UTF-8, please \nconvert it to UTF-8 then open again." UTF8String]);
    [self setEditable:NO];
}

- (void)setFont:(NSFont *)font {
    [super setFont:font];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (void)setTextColor:(NSColor *)textColor {
    [super setTextColor:textColor];
    [self setTextAttributesDescriptor:[self textAttributesDescriptor]];
}

- (BOOL)becomeFirstResponder {
    [super becomeFirstResponder];
    return YES;
}

- (BOOL)resignFirstResponder {
    [super resignFirstResponder];
    return YES;
}

@end
