//
//  CEESessionToolbar.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/12.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionToolbar : CEEViewController
@property BOOL shown;
@property (weak) CEESession* session;
- (NSString*)serialize;
- (void)deserialize:(NSDictionary*)dict;
@end

NS_ASSUME_NONNULL_END
