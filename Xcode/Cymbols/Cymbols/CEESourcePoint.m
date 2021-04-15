//
//  CEESourcePoint.m
//  Cymbols
//
//  Created by qing on 2020/10/19.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESourcePoint.h"

@implementation CEESourcePoint


- (instancetype)initWithSourceSymbol:(CEESourceSymbol*)symbol {
    self = [super init];
    if (!self)
        return nil;
    
    _filePath = [NSString stringWithUTF8String:symbol->file_path];
    _locations = [NSString stringWithUTF8String:cee_string_from_ranges(symbol->ranges)];
    _lineNumber = symbol->line_no;
    
    return self;
}

- (instancetype)initWithSourceSymbolReference:(CEESourceSymbolReference*)reference {
    
    self = [super init];
    if (!self)
        return nil;
    _filePath = [NSString stringWithUTF8String:reference->file_path];
    _locations = [NSString stringWithUTF8String:cee_string_from_ranges(reference->ranges)];
    _lineNumber = reference->line_no;
    
    return self;
}



@end
