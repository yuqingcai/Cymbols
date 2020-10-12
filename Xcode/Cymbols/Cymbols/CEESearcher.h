//
//  CEESearcher.h
//  Cymbols
//
//  Created by qing on 2020/9/30.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_OPTIONS(NSUInteger, CEESearchOption) {
    kCEESearchOptionNormal = 0,
    kCEESearchOptionCaseSensitive = 1 << 3,
    kCEESearchOptionMatchWord = 1 << 4,
};

typedef NS_ENUM(NSUInteger, CEESearchMode) {
    kCEESearchModeReference = 1 << 0,
    kCEESearchModeString = 1 << 1,
    kCEESearchModeRegex = 1 << 2,
};

@interface CEESearchResult : NSObject
@property (strong) NSString* filePath;
@property (strong) NSString* locations;
@end

@interface CEEProjectSearcher : NSObject
@property NSString* target;
@property NSString* filePattern;
@property NSArray* _Nullable results;
@property CEESearchOption options;
@property CEESearchMode mode;

@end

NS_ASSUME_NONNULL_END
