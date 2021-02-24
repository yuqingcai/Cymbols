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
#import "CEESearcher.h"
#import "CEESourcePoint.h"
#import "CEESecurityBookmark.h"

extern NSNotificationName CEENotificationProjectSettingProperties;
extern NSNotificationName CEENotificationProjectAddReference;
extern NSNotificationName CEENotificationProjectRemoveReference;
extern NSNotificationName CEENotificationProjectCreateFile;
extern NSNotificationName CEENotificationProjectRemoveFile;
extern NSNotificationName CEENotificationSessionPortOpenSourceBuffer;
extern NSNotificationName CEENotificationSessionPortCloseSourceBuffer;
extern NSNotificationName CEENotificationSessionPortActiveSourceBuffer;
extern NSNotificationName CEENotificationSessionPresent;
extern NSNotificationName CEENotificationSessionCreatePort;
extern NSNotificationName CEENotificationSessionActivePort;
extern NSNotificationName CEENotificationSessionDeletePort;
extern NSNotificationName CEENotificationSessionPortCreateContext;
extern NSNotificationName CEENotificationSessionPortJumpToSourcePoint;
extern NSNotificationName CEENotificationSessionPortRequestJumpSourcePointSelection;
extern NSNotificationName CEENotificationSessionPortSetSelectedSymbol;
extern NSNotificationName CEENotificationSessionPortPresentHistory;
extern NSNotificationName CEENotificationSessionPortSaveSourceBuffer;
extern NSNotificationName CEENotificationSessionPortSetDescriptor;
extern NSNotificationName CEENotificationSessionPortSearchReference;

@class CEEProject;
@class CEESession;
@class CEESessionPort;

NSArray* ExpandFilePaths(NSArray* paths);
NSDictionary* JSONDictionaryFromString(NSString* string);

@interface CEEProjectSetting : NSObject
@property (strong) NSString* name;
@property (strong) NSString* path;
@property (strong) CEESecurityBookmark* pathBookmark;
@property (strong) NSString* databasePath;
@property (strong) NSArray* referenceRoots;
- (void)saveAsDefaultSetting;
@end

CEESecurityBookmark* CreateBookmarkWithFilePath(NSString* filePath);
NSArray* CreateBookmarksWithFilePaths(NSArray* filePaths);

@interface CEESourceBufferReferenceContext : NSObject
@property NSString* filePath;
@property NSInteger lineBufferOffset;
@property NSInteger caretBufferOffset;
- (instancetype)initWithFilePath:(NSString*)filePath;
@end

@interface CEESessionPort : NSObject <CEESerialization>
@property (weak) CEESession* session;
@property (strong, readonly) NSString* identifier;
@property (strong, readonly) NSMutableArray* sourceBufferReferences;
@property (strong, readonly) NSMutableArray* openedSourceBuffers;
@property (readonly) CEEList* context;
@property (readonly) CEESourceSymbol* selected_symbol;
@property (readonly) CEESourcePoint* jumpPoint;
@property NSString* descriptor;

- (void)moveSourceBufferReferenceNext;
- (void)moveSourceBufferReferencePrev;
- (CEESourceBufferReferenceContext*)currentSourceBufferReference;
- (void)setActivedSourceBuffer:(CEESourceBuffer*)buffer;
- (CEESourceBuffer*)activedSourceBuffer;
- (void)presentHistory:(CEESourceBufferReferenceContext*)reference;
- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString*)filePath;
- (CEESourceBuffer*)openUntitledSourceBuffer;
- (void)closeSourceBuffer:(CEESourceBuffer*)buffer;
- (void)closeSourceBuffers:(NSArray*)buffers;
- (void)closeAllSourceBuffers;
- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath;
- (void)discardReferences;
- (void)createContextByCluster:(CEETokenCluster*)cluster;
- (void)setSelectedSourceSymbol:(CEESourceSymbol*)symbol;
- (void)searchReferencesByCluster:(CEETokenCluster*)cluster;
- (void)jumpToSourceSymbolByCluster:(CEETokenCluster*)cluster;
- (void)jumpToSourcePoint:(CEESourcePoint*)sourcePoint;

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
@property (strong, readonly) NSMutableArray* sessions;
@property (strong) CEESession* currentSession;
@property (readonly) cee_pointer database;
@property (readonly) NSString* name;
@property (readonly) NSString* databasePath;
@property (strong, readonly) CEEProjectSearcher* searcher;
- (void)setProperties:(CEEProjectSetting*)properties;
- (void)serialize;
- (void)deserialize:(NSString*)filePath;
- (void)createSessionWithFilePaths:(NSArray*)filePaths;
- (void)deleteAllSessions;
- (NSArray*)getReferenceRoots;
- (NSArray*)getReferenceFilePathsWithCondition:(NSString*)condition;
- (void)addReferenceRoot:(NSString*)filePath;
- (void)addReferenceRoots:(NSArray*)roots;
- (void)removeReferenceRoot:(NSString*)filePath;
- (void)removeReferenceRoots:(NSArray*)roots;
- (void)addReference:(NSString*)filePath;
- (void)addReferences:(NSArray*)filePaths;
- (void)removeReference:(NSString*)filePath;
- (void)removeReferences:(NSArray*)filePaths;
- (BOOL)filePathIsReferenced:(NSString*)filePath;
- (BOOL)filePathIsReferenceRoot:(NSString*)filePath;
- (void)syncSourceSymbols:(CEESourceBuffer*)buffer;
- (BOOL)isUntitled;
- (BOOL)createFile:(NSString*)filePath;
- (BOOL)createDirectory:(NSString*)directoryPath;
- (BOOL)removeFile:(NSString*)filePath;
- (BOOL)removeFiles:(NSArray*)filePaths;
- (CEESecurityBookmark*)addSecurityBookmarkWithFilePath:(NSString*)filePath;
- (CEESecurityBookmark*)getSecurityBookmarkWithFilePath:(NSString*)filePath;
- (NSString*)shortFilePath:(NSString*)filePath;
@end

@interface CEEProjectController : NSDocumentController
- (CEEProject*)createProjectFromSetting:(CEEProjectSetting*)setting;
- (CEEProject*)openProjectFromURL:(NSURL*)url;
- (void)replaceSourceBufferReferenceFilePath:(NSString*)filePath0 to:(NSString*)filePath1;
@end
