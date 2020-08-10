//
//  CEEWindowController.h
//  Cymbols
//
//  Created by yuqingcai on 2018/10/16.
//  Copyright © 2018 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEESerialization.h"

@interface CEEWindowController : NSWindowController <NSWindowDelegate, CEESerialization>
@property (strong) NSString* identifier;
- (void)initProperties;
@end
