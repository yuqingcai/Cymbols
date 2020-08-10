//
//  CEESessionViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEViewController.h"
#import "CEEProject.h"

@interface CEESessionViewController : CEEViewController <NSOpenSavePanelDelegate>
@property (readonly) CGFloat sheetOffset;
@property (weak) CEESession* session;
- (void)setFrameAttachable:(BOOL)enable;
- (NSString*)serialize;
- (void)deserialize:(NSDictionary*)dict;
@end

