//
//  CEESecurityBookmark.h
//  Cymbols
//
//  Created by qing on 2020/11/11.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CEESecurityBookmark : NSObject
@property (strong) NSString* filePath;
@property (strong) NSString* content;
- (instancetype)initWithFilePath:(NSString*)filePath;
@end

NS_ASSUME_NONNULL_END
