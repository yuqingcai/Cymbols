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

- (BOOL)becomeFirstResponder {
    [super becomeFirstResponder];
    return YES;
}

- (BOOL)resignFirstResponder {
    [super resignFirstResponder];
    return YES;
}

@end
