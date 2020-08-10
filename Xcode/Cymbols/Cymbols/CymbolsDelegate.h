//
//  CymbolsDelegate.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEProject.h"
#import "CEESourceBuffer.h"

@interface CymbolsDelegate : NSObject <NSApplicationDelegate>
@property (strong) CEEProjectController* projectController;
@property (strong) CEESourceBufferManager* sourceBufferManager;
@property (strong) NSDictionary* configurations;

- (NSString*)cymbolsHome;
- (NSObject*)nibObjectWithClass:(Class)class fromNibNamed:(NSString*)name;
- (NSObject*)nibObjectWithIdentifier:(NSString*)identifier fromNibNamed:(NSString*)name;
- (NSString*)welcomeGuidePath;
- (CEEProject*)currentProject;

@end
