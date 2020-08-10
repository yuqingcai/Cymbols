//
//  CEEProject.h
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEESerialization.h"
#import "CEESourceBuffer.h"

extern NSNotificationName CEENotificationProjectSettingProperties;
extern NSNotificationName CEENotificationProjectAddFilePaths;
extern NSNotificationName CEENotificationProjectRemoveFilePaths;
extern NSNotificationName CEENotificationSessionPortOpenSourceBuffer;
extern NSNotificationName CEENotificationSessionPortSaveSourceBuffer;
extern NSNotificationName CEENotificationSessionPortCloseSourceBuffer;
extern NSNotificationName CEENotificationSessionPortPresentSourceBuffer;
extern NSNotificationName CEENotificationSessionPresent;
extern NSNotificationName CEENotificationSessionCreatePort;
extern NSNotificationName CEENotificationSessionActivePort;
extern NSNotificationName CEENotificationSessionDeletePort;

@class CEEProject;
@class CEESession;
@class CEESessionPort;

NSArray* ExclusiveFilePaths(NSArray* paths);
NSArray* ExpandFilePaths(NSArray* paths);
NSDictionary* JSONDictionaryFromString(NSString* string);

@interface CEEProjectSetting : NSObject
@property (strong) NSString* name;
@property (strong) NSString* path;
@property (strong) NSArray* filePaths;
- (NSString*)databasePath;

@end

@interface CEEBufferReference : NSObject
@property (weak) CEESourceBuffer* buffer;
@property NSInteger paragraphIndex;
- (instancetype)initWithSourceBuffer:(CEESourceBuffer*)buffer;
@end

@interface CEESessionPort : NSObject <CEESerialization>
@property (weak) CEESession* session;
@property (strong, readonly) NSString* identifier;
@property (strong, readonly) NSMutableArray* references;
@property NSInteger referenceIndex;

- (void)appendReference:(CEESourceBuffer*)buffer;
- (void)nextReference;
- (void)prevReference;
- (CEEBufferReference*)currentReference;
- (void)presentHistory:(CEEBufferReference*)reference;
- (void)openSourceBufferWithFilePath:(NSString*)filePath;
- (void)openSourceBuffersWithFilePaths:(NSArray*)filePaths;
- (void)openUntitledSourceBuffer;
- (void)presentSourceBuffer:(CEESourceBuffer*)buffer;
- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filePath;
- (void)discardSourceBuffers;
@end

@interface CEESession : NSObject <CEESerialization>
@property (weak) CEEProject* project;
@property (strong, readonly) NSString* identifier;
@property (strong, readonly) NSMutableArray* ports;
@property (strong, readonly) NSDictionary* descriptor;
@property (strong) CEESessionPort* activedPort;
@property (strong, readonly) NSMutableArray* registeredSourceBuffers;

- (CEESessionPort*)createPort;
- (void)deletePort:(CEESessionPort*)port;
- (NSArray*)filePathsFilter:(NSString*)condition;
- (NSString*)serialize;
- (void)deserialize:(NSDictionary*)dict;
- (void)registerSourceBuffer:(CEESourceBuffer*)buffer;
@end

@interface CEEProject : NSDocument
@property (strong) NSMutableArray* sessions;
@property (strong) CEESession* currentSession;
@property (readonly) cee_pointer database;
@property (strong) CEEProjectSetting* properties;

- (void)serialize;
- (void)createSessionWithFilePaths:(NSArray*)filePaths;
- (NSArray*)getFilePathsWithCondition:(NSString*)condition;
- (void)addFilePaths:(NSArray*)filePaths;
- (void)removeFilePaths:(NSArray*)filePaths;
- (CEEProjectSetting*)createEmptyProjectSetting;

@end

@interface CEEProjectController : NSDocumentController
- (CEEProject*)createProjectFromSetting:(CEEProjectSetting*)setting;
- (CEEProject*)openProjectFromURL:(NSURL*)url;
@end

