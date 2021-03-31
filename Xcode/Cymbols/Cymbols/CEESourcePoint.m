//
//  CEESourcePoint.m
//  Cymbols
//
//  Created by qing on 2020/10/19.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESourcePoint.h"

@implementation CEESourcePoint

- (instancetype)initWithFilePath:(NSString*)filePath andLocations:(NSString*)locations {
    self = [super init];
    if (!self)
        return nil;
    
    _filePath = filePath;
    _locations = locations;
    
    return self;
}

- (instancetype)initWithSourceSymbol:(CEESourceSymbol*)symbol {
    self = [super init];
    if (!self)
        return nil;
    
    _filePath = [NSString stringWithUTF8String:symbol->file_path];
    _locations = [NSString stringWithUTF8String:cee_string_from_ranges(symbol->ranges)];
    
    return self;
}

- (instancetype)initWithSourceSymbolReference:(CEESourceSymbolReference*)reference {
    
    self = [super init];
    if (!self)
        return nil;
    _filePath = [NSString stringWithUTF8String:reference->file_path];
    _locations = [NSString stringWithUTF8String:cee_string_from_ranges(reference->ranges)];
    
    return self;
}
@end
