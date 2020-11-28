//
//  AppDelegate.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEProject.h"
#import "CEESourceBuffer.h"
#import "CEENetwork.h"
#import "CEEWindowController.h"

#define CEE_APP_HEART_BEAT_INTERVAL 0.1

extern NSNotificationName CEENotificationHeartBeat;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) CEEProjectController* projectController;
@property (strong) CEESourceBufferManager* sourceBufferManager;
@property (strong, readonly) NSDictionary* configurations;
@property (strong) CEEProject* currentProject;
@property (strong) CEENetwork* network;
@property (strong, readonly) NSString* supportDirectory;
@property (strong, readonly) NSString* defaultWindowSettingPath;
@property (strong, readonly) NSString* defaultProjectSettingPath;

- (NSObject*)nibObjectWithClass:(Class)class fromNibNamed:(NSString*)name;
- (NSObject*)nibObjectWithIdentifier:(NSString*)identifier fromNibNamed:(NSString*)name;
- (NSString*)welcomeGuidePath;
- (NSString*)serializerVersionString;
- (NSString*)infoString;
- (void)setConfiguration:(NSString*)configuration value:(NSString*)value;
- (NSString*)configurationFilePath;
- (void)saveWindowSettingAsDefault:(CEEWindowController*)controller;
@end
