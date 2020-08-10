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
+ (NSString*)stringFromFile:(NSString*)filepath;
+ (NSData*)dataFromFile:(NSString*)filepath;
+ (id)objectFromFile:(NSString*)filepath;
@end

NS_ASSUME_NONNULL_END
