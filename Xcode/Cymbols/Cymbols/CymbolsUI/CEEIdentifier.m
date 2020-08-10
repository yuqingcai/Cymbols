//
//  CEEIdentifier.c
//  Cymbols
//
//  Created by qing on 2020/1/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#include "CEEIdentifier.h"
#include "cee_lib.h"

BOOL IdentifierIsReserved(NSString* string)
{
    if (!string)
        return NO;
    
    NSString* identifier = string;
    if ([[identifier substringWithRange:NSMakeRange(0, 1)] isEqualToString:@"_"])
        return YES;
    return NO;
}

NSString* CreateUnifiedIdentifier(void) {
    cee_char* uuid_string = cee_uuid_string_random();
    NSString* identifier = [NSString stringWithFormat:@"%s", uuid_string];
    cee_free(uuid_string);
    return identifier;
}

NSString* CreateUnifiedIdentifierWithPrefix(NSString* string)
{
    NSString* identifier = string;
    if ([[identifier substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        identifier = [identifier stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"ID"];
    else if (![[identifier substringWithRange:NSMakeRange(0, 2)] isEqualToString:@"ID"])
        identifier = [@"ID" stringByAppendingString:identifier];
    
    cee_char* uuid_string = cee_uuid_string_random();
    identifier = [NSString stringWithFormat:@"%@#%s", identifier, uuid_string];
    cee_free(uuid_string);
    return identifier;
}

NSString* CreateClassIdentifier(NSString* className)
{
    NSString* identifier = className;
    if ([[identifier substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        identifier = [identifier stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"ID"];
    return identifier;
}

NSString* ClassNameByUnifiedIdentifier(NSString* identifier)
{
    NSString* string = identifier;
    NSInteger location = [string rangeOfString:@"#"].location;
    if (location != NSNotFound)
        string = [string substringWithRange:NSMakeRange(0, location)];
    
    if ([[string substringWithRange:NSMakeRange(0, 2)] isEqualToString:@"ID"])
        string = [string stringByReplacingCharactersInRange:NSMakeRange(0, 2) withString:@"CEE"];
    
    return string;
}

NSString* IdentifierByDeletingPrefix(NSString* identifier)
{
    NSString* string = identifier;
    NSInteger location = [string rangeOfString:@"#"].location;
    NSRange range;
    if (location != NSNotFound) {
        range = NSMakeRange(location + 1, [string length] - (location + 1));
        string = [string substringWithRange:range];
    }
    return string;
}

NSString* IdentifierByDeletingSuffix(NSString* identifier)
{
    NSString* string = identifier;
    NSInteger location = [string rangeOfString:@"#"].location;
    if (location != NSNotFound)
        string = [string substringWithRange:NSMakeRange(0, location)];
    return string;
}

NSString* CreateUnifiedIdentifierWithPrefixAndSuffix(NSString* prefix,
                                                     NSString* suffix)
{
    return [prefix stringByAppendingFormat:@"#%@", suffix];
}
