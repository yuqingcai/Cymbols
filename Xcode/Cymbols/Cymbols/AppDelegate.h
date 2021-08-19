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

#define CEE_APP_HEART_BEAT_INTERVAL                 0.1     // in second
#define TIMER_FREEZER_TRIGGER_INTERVAL              1200    // in second
#define TIMER_FREEZER_FREEZE_INTERVAL               15      // in second

extern NSNotificationName CEENotificationHeartBeat;
extern NSNotificationName CEENotificationTimeFreeze;
extern NSNotificationName CEENotificationApplicationConfigurationChanged;

extern NSString* CEEWindowSettingFilePathIndexer;
extern NSString* CEEProjectSettingFilePathIndexer;
extern NSString* CEEConfigurationTemplateFilePathIndexer;
extern NSString* CEEConfigurationFilePathIndexer;
extern NSString* CEEWelcomeGuideFilePathIndexer;
extern NSString* CEETemporaryDirectoryIndexer;
extern NSString* CEEUpdateInfoFilePathIndexer;
extern NSString* CEEApplicationInfoStringIndexer;
extern NSString* CEEApplicationVersionIndexer;
extern NSString* CEEBundleVersionIndexer;
extern NSString* CEESerializerVersionIndexer;
extern NSString* CEEBackwardCompatibleMaxVersionIndexer;
extern NSString* CEEApplicationConfigurationNameLineWrap;
extern NSString* CEEApplicationConfigurationNameCaretBlinkTimeInterval;
extern NSString* CEEApplicationConfigurationNameShowLineNumber;
extern NSString* CEEApplicationConfigurationNameUIStyle;
extern NSString* CEEApplicationConfigurationNameTextHighlightStyle;
extern NSString* CEEApplicationConfigurationNameShowPageGuideLine;
extern NSString* CEEApplicationConfigurationNamePageGuideLineOffset;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong, readonly) NSWindowController* projectCreatorWindowController;
@property (strong, readonly) NSWindowController* projectParseWindowController;
@property (strong, readonly) NSWindowController* projectCleanWindowController;
@property (strong, readonly) NSWindowController* projectSearchWindowController;
@property (strong, readonly) NSWindowController* projectContextWindowController;
@property (strong, readonly) NSWindowController* timeFreezerWindowController;
@property (strong, readonly) NSWindowController* addReferenceWindowController;
@property (strong, readonly) NSWindowController* removeReferenceWindowController;
@property (strong, readonly) NSWindowController* referenceRootScannerWindowController;
@property (strong, readonly) NSWindowController* updateInfoWindowController;
@property (strong, readonly) NSWindowController* preferenceWindowController;
@property (strong, readonly) NSWindowController* paymentSelectionWindowController;
@property (strong, readonly) NSWindowController* purchaseErrorWindowController;
@property (strong, readonly) NSWindowController* purchaseCancelWindowController;
@property (strong, readonly) NSWindowController* paymentVerifiedErrorWindowController;

@property (strong, readonly) CEEProjectController* projectController;
@property (strong, readonly) CEESourceBufferManager* sourceBufferManager;
@property (strong, readonly) NSDictionary* configuration;
@property (strong, readonly) CEENetwork* network;
@property (strong) CEEProject* currentProject;

- (NSObject*)nibObjectWithClass:(Class)class fromNibNamed:(NSString*)name;
- (NSObject*)nibObjectWithIdentifier:(NSString*)identifier fromNibNamed:(NSString*)name;
- (NSString*)propertyIndex:(NSString*)indexer;
- (void)setConfigurationForKey:(NSString*)key value:(NSString*)value;
- (void)saveWindowSettingAsDefault:(CEEWindowController*)controller;
- (void)startAccessingSecurityScopedResourceWithBookmark:(CEESecurityBookmark*)bookmark;
@end
