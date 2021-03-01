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
#import "CEEWindowController.h"
#import "CEESecurityBookmark.h"
#import "CEENetwork.h"

//#define TRIAL_VERSION
#define CEE_APP_HEART_BEAT_INTERVAL 0.1

extern NSNotificationName CEENotificationHeartBeat;
extern NSNotificationName CEENotificationTimeFreeze;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) CEEProjectController* projectController;
@property (strong) CEESourceBufferManager* sourceBufferManager;
@property (strong, readonly) NSDictionary* configurations;
@property (strong) CEEProject* currentProject;
@property (strong, readonly) NSString* supportDirectory;
@property (strong, readonly) NSString* defaultWindowSettingPath;
@property (strong, readonly) NSString* defaultProjectSettingPath;
@property (strong) CEENetwork* network;

- (NSObject*)nibObjectWithClass:(Class)class fromNibNamed:(NSString*)name;
- (NSObject*)nibObjectWithIdentifier:(NSString*)identifier fromNibNamed:(NSString*)name;
- (NSString*)welcomeGuidePath;
- (NSString*)serializerVersionString;
- (NSString*)infoString;
- (NSString*)versionString;
- (NSString*)bundleVersionString;
- (void)setConfiguration:(NSString*)configuration value:(NSString*)value;
- (NSString*)configurationFilePath;
- (void)saveWindowSettingAsDefault:(CEEWindowController*)controller;
- (void)startAccessingSecurityScopedResourceWithBookmark:(CEESecurityBookmark*)bookmark;
@end
