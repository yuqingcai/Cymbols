//
//  CEESearcher.m
//  Cymbols
//
//  Created by qing on 2020/9/30.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESearcher.h"


@implementation CEESearchResult

@end

@implementation CEEProjectSearcher
@synthesize options = _options;

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self setMode:kCEESearchModeReference];
    _target = @"";
    _filePattern = @"*";
    return self;
}

@end
