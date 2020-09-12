//
//  CEEJSONReader.m
//  Cymbols
//
//  Created by qing on 2020/2/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEJSONReader.h"

@implementation CEEJSONReader
+ (NSString*)stringFromFile:(NSString*)filepath {
    NSString* string = [NSString stringWithContentsOfFile:filepath encoding:NSUTF8StringEncoding error:nil];
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

+ (NSData*)dataFromFile:(NSString*)filepath {
    NSString* string = [CEEJSONReader stringFromFile:filepath];
    NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:NO];
    return data;
}

+ (id)objectFromFile:(NSString*)filepath {
    NSString* string = [CEEJSONReader stringFromFile:filepath];
    NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:NO];
    return [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
}

@end
