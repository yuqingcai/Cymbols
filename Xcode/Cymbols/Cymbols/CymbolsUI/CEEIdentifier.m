//
//  CEEIdentifier.c
//  Cymbols
//
//  Created by qing on 2020/1/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#include "CEEIdentifier.h"

NSString* CreateObjectID(NSObject* object) {
    NSString* identifier = [object className];
    NSString* uuid = [[NSUUID UUID] UUIDString];
    
    if ([[identifier substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        identifier = [identifier stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"ID"];
    
    identifier = [NSString stringWithFormat:@"%@#%@", identifier, uuid];
    return identifier;
}

NSString* CreateObjectIDPrefix(NSObject* object)
{
    NSString* prefix = [object className];
    if ([[prefix substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        prefix = [prefix stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"ID"];
    return prefix;
}

NSString* ClassNameFromObjectID(NSString* identifier)
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

NSString* CreateObjectIDWithPrefixAndSuffix(NSString* prefix,
                                            NSString* suffix)
{
    return [prefix stringByAppendingFormat:@"#%@", suffix];
}

NSString* CreateObjectIDWithPrefix(NSString* prefix)
{
    NSString* uuid = [[NSUUID UUID] UUIDString];
    return [prefix stringByAppendingFormat:@"#%@", uuid];
}

NSString* CreateClassID(NSObject* object) {
    NSString* identifier = [object className];
    if ([[identifier substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        identifier = [identifier stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"Class"];
    
    return identifier;
}

BOOL IdentifierIsReserved(NSString* identifier)
{
    if ([[identifier substringWithRange:NSMakeRange(0, 1)] isEqualToString:@"_"])
        return YES;
    return NO;
}
