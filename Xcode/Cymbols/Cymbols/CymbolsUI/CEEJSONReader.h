//
//  CEEJSONReader.h
//  Cymbols
//
//  Created by qing on 2020/2/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CEEJSONReader : NSObject
+ (NSString*)stringFromFile:(NSString*)filePath;
+ (NSData*)dataFromFile:(NSString*)filePath;
+ (id)objectFromFile:(NSString*)filePath;
+ (BOOL)object:(id)object toFile:(NSString*)filePath;
@end

NS_ASSUME_NONNULL_END
