//
//  cee_regex_macos.m
//  Cymbols
//
//  Created by qing on 2020/10/23.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "cee_regex_macos.h"
#import "cee_range.h"
#import "cee_datetime.h"

CEEList* cee_regex_macos_search(const cee_char* string,
                                const cee_char* pattern,
                                cee_boolean repeat,
                                cee_long timeout_ms,
                                cee_boolean* timeout)
{
    if (!string || !pattern)
        return NULL;
    
    NSString* contentString = [NSString stringWithUTF8String:string];
    NSString* patternString = [NSString stringWithUTF8String:pattern];
    NSRegularExpressionOptions options = NSRegularExpressionDotMatchesLineSeparators | 
                                        NSRegularExpressionAnchorsMatchLines;
    __block CEEList* ranges = NULL;
    __block cee_ulong t0 = cee_timestamp_ms();
    __block cee_ulong t1 = 0;
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:options error:NULL];
    
    [regex enumerateMatchesInString:contentString options:0 range:NSMakeRange(0, [contentString length]) usingBlock:^(NSTextCheckingResult * _Nullable result, NSMatchingFlags flags, BOOL * _Nonnull stop) {
        
        for (int i = 0; i < result.numberOfRanges; i ++) {
            NSRange matchRange = [result rangeAtIndex:i];
            CEERange* range = cee_range_create(matchRange.location,
                                               matchRange.length);
            ranges = cee_list_prepend(ranges, range);
        }
        
        t1 = cee_timestamp_ms();
        
        if (timeout_ms && (t1 - t0 >= timeout_ms)) {
            if (timeout)
                *timeout = TRUE;
            *stop = YES;
        }
        
        if (!repeat)
            *stop = YES;
    }];
    
    ranges = cee_list_reverse(ranges);
    
    return ranges;
}
