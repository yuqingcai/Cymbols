//
//  CEENetwork.h
//  Cymbols
//
//  Created by qing on 2020/9/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>

extern NSNotificationName CEENotificationCymbolsUpdateNotify;
extern NSNotificationName CEENotificationCymbolsUpdateConfirm;

NS_ASSUME_NONNULL_BEGIN

@interface CEENetwork : NSObject
- (void)setUpdateFlag;
- (void)cleanUpdateFlag;
- (BOOL)updateFlagSet;
- (NSString*)updateInfoString;
@end

NS_ASSUME_NONNULL_END
