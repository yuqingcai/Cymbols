//
//  CEESecurityBookmark.m
//  Cymbols
//
//  Created by qing on 2020/11/11.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESecurityBookmark.h"

@implementation CEESecurityBookmark

- (instancetype)initWithFilePath:(NSString*)filePath {
    self = [super init];
    
    if (!self)
        return nil;
    
    self.filePath = filePath;
    NSURL* fileURL = [NSURL fileURLWithPath:filePath];
    NSError* error = nil;
    NSData* bookmarkData = [fileURL bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope includingResourceValuesForKeys:nil relativeToURL:nil error:&error];
    
    if (!error)
        self.content = [bookmarkData base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
    else
        return nil;
    
    return self;
}

@end
