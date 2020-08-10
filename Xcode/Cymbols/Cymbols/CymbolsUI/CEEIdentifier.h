//
//  CEEIdentifier.h
//  Cymbols
//
//  Created by qing on 2020/1/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#ifndef CEEIdentifier_h
#define CEEIdentifier_h

#import <Foundation/Foundation.h>

BOOL IdentifierIsReserved(NSString* string);
NSString* CreateUnifiedIdentifier(void);
NSString* CreateUnifiedIdentifierWithPrefix(NSString* string);
NSString* CreateClassIdentifier(NSString* className);
NSString* ClassNameByUnifiedIdentifier(NSString* identifier);
NSString* IdentifierByDeletingPrefix(NSString* identifier);
NSString* IdentifierByDeletingSuffix(NSString* identifier);
NSString* CreateUnifiedIdentifierWithPrefixAndSuffix(NSString* prefix,
                                                     NSString* suffix);
#endif /* CEEIdentifier_h */
