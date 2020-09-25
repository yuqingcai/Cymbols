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
extern NSNotificationName CEENotificationSessionPortCloseSourceBuffer;
extern NSNotificationName CEENotificationSessionPortActiveSourceBuffer;
extern NSNotificationName CEENotificationSessionPresent;
extern NSNotificationName CEENotificationSessionCreatePort;
extern NSNotificationName CEENotificationSessionActivePort;
extern NSNotificationName CEENotificationSessionDeletePort;
extern NSNotificationName CEENotificationSessionPortCreateContext;
extern NSNotificationName CEENotificationSessionPortSetTargetSymbol;
extern NSNotificationName CEENotificationSessionPortRequestTargetSymbolSelection;
extern NSNotificationName CEENotificationSessionPortSetActivedSymbol;
extern NSNotificationName CEENotificationSessionPortPresentHistory;
extern NSNotificationName CEENotificationSessionPortSaveSourceBuffer;
extern NSNotificationName CEENotificationSessionPortSetActivedBufferOffset;

@class CEEProject;
@class CEESession;
@class CEESessionPort;

NSArray* ExclusiveFilePaths(NSArray* paths);
NSArray* ExpandFilePaths(NSArray* paths);
NSDictionary* JSONDictionaryFromString(NSString* string);

@interface CEEProjectSetting : NSObject
@property (strong) NSString* name;
@property (strong) NSString* path;
@property (strong) NSArray* filePathsUserSelected;
@property (strong) NSArray* filePathsExpanded;
- (NSString*)databasePath;

@end

@interface CEEBufferReference : NSObject
@property NSString* filePath;
@property NSInteger bufferOffset;
- (instancetype)initWithFilePath:(NSString*)filePath;
@end

@interface CEESessionPort : NSObject <CEESerialization>
@property (weak) CEESession* session;
@property (strong, readonly) NSString* identifier;
@property (strong, readonly) NSMutableArray* bufferReferences;
@property (strong, readonly) NSMutableArray* openedSourceBuffers;
@property (readonly) CEEList* context;
@property (readonly) CEESourceSymbol* target_symbol;
@property (readonly) CEESourceSymbol* actived_symbol;
@property (readonly) NSInteger active_buffer_offset;

- (void)moveBufferReferenceNext;
- (void)moveBufferReferencePrev;
- (CEEBufferReference*)currentBufferReference;
- (void)setActivedSourceBuffer:(CEESourceBuffer*)buffer;
- (CEESourceBuffer*)activedSourceBuffer;

- (void)presentHistory:(CEEBufferReference*)reference;
- (NSArray*)openSourceBuffersWithFilePaths:(NSArray*)filePaths;
- (CEESourceBuffer*)openUntitledSourceBuffer;
- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filePath;
- (void)closeSourceBuffers:(NSArray*)buffers;
- (void)closeAllSourceBuffers;
- (void)discardReferences;
- (void)createContextByCluster:(CEETokenCluster*)cluster;
- (void)jumpToTargetSymbolByCluster:(CEETokenCluster*)cluster;
- (void)setTargetSourceSymbol:(CEESourceSymbol*)symbol;
- (void)setActivedSourceSymbol:(CEESourceSymbol*)symbol;
- (void)setActivedBufferOffset:(NSInteger)offset;
@end

@interface CEESession : NSObject <CEESerialization>
@property (weak) CEEProject* project;
@property (strong, readonly) NSString* identifier;
@property (strong, readonly) NSMutableArray* ports;
@property (strong, readonly) NSDictionary* descriptor;
@property (strong) CEESessionPort* activedPort;

- (CEESessionPort*)createPort;
- (void)deletePort:(CEESessionPort*)port;
- (void)deleteAllPorts;
- (NSArray*)filePathsFilter:(NSString*)condition;
- (NSString*)serialize;
- (void)deserialize:(NSDictionary*)dict;

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
- (void)deleteSession:(CEESession*)session;
- (void)deleteAllSessions;
@end

@interface CEEProjectController : NSDocumentController
- (CEEProject*)createProjectFromSetting:(CEEProjectSetting*)setting;
- (CEEProject*)openProjectFromURL:(NSURL*)url;
- (void)replaceSourceBufferReferenceFilePath:(NSString*)filePath0 to:(NSString*)filePath1;
@end

