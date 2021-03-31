//
//  CEESourcePoint.h
//  Cymbols
//
//  Created by qing on 2020/10/19.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "cee_symbol.h"
#import "cee_reference.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESourcePoint : NSObject
@property (strong) NSString* filePath;
@property (strong) NSString* locations;
- (instancetype)initWithFilePath:(NSString*)filePath andLocations:(NSString*)locations;
- (instancetype)initWithSourceSymbol:(CEESourceSymbol*)symbol;
- (instancetype)initWithSourceSymbolReference:(CEESourceSymbolReference*)reference;
@end

NS_ASSUME_NONNULL_END
