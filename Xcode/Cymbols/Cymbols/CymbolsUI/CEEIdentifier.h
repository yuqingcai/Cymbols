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

NSString* CreateObjectID(NSObject* object);
NSString* ClassNameFromObjectID(NSString* identifier);
NSString* IdentifierByDeletingPrefix(NSString* identifier);
NSString* IdentifierByDeletingSuffix(NSString* identifier);
NSString* CreateObjectIDPrefix(NSObject* object);
NSString* CreateObjectIDWithPrefixAndSuffix(NSString* prefix,
                                            NSString* suffix);
NSString* CreateObjectIDWithPrefix(NSString* prefix);
NSString* CreateClassID(NSObject* object);
BOOL IdentifierIsReserved(NSString* identifier);

#endif /* CEEIdentifier_h */
