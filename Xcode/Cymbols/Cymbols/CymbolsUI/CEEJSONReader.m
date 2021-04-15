//
//  CEEJSONReader.m
//  Cymbols
//
//  Created by qing on 2020/2/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEJSONReader.h"

@implementation CEEJSONReader
+ (NSString*)stringFromFile:(NSString*)filePath {
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return nil;
    
    NSString* string = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
    NSRegularExpressionOptions options = NSRegularExpressionDotMatchesLineSeparators |
                                        NSRegularExpressionCaseInsensitive |
                                        NSRegularExpressionAnchorsMatchLines;
    
    NSString* pattern = @"(?>\\/\\*.*?\\*\\/)|(?>\\/\\/.*?$)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:pattern options:options error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    
    NSInteger i = matches.count - 1;
    while (i >= 0) {
        NSTextCheckingResult *match = matches[i];
        string = [string stringByReplacingCharactersInRange:[match rangeAtIndex:0] withString:@""];
        i --;
    }
    
    return string;
}

+ (NSData*)dataFromFile:(NSString*)filePath {
    NSString* string = [CEEJSONReader stringFromFile:filePath];
    if (!string)
        return nil;
    NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:NO];
    return data;
}

+ (id)objectFromFile:(NSString*)filePath {
    NSString* string = [CEEJSONReader stringFromFile:filePath];
    if (!string)
        return nil;
    NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:NO];
    return [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
}

+ (BOOL)object:(id)object toFile:(NSString*)filePath {
    NSError* error = nil;
    NSData* data = nil;
    NSString* string = nil;
    
    if (![object isKindOfClass:[NSDictionary class]])
        return NO;
    
    data = [NSJSONSerialization dataWithJSONObject:object options:NSJSONWritingPrettyPrinted error:&error];
    if (!data)
        return NO;
    
    string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    [string writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    return YES;
}

@end
