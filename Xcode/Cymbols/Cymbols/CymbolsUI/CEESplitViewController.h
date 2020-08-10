//
//  CEESplitViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/5.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEViewController.h"
#import "CEESerialization.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESplitViewController : NSSplitViewController <CEESerialization>
- (void)initProperties;
- (NSViewController*)viewControllerByIdentifier:(NSString*)identifier;
- (void)toggleViewByIdentifier:(NSString*)identifier;
- (void)showView:(BOOL)shown byIdentifier:(NSString*)identifier;
@end

NS_ASSUME_NONNULL_END
