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
    
    self.filePath = filePath;
    self.locations = locations;
    
    return self;
}

@end
