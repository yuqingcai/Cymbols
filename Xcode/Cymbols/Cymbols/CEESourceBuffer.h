//
//  CEESourceBuffer.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/19.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "cee_lib.h"
#import "cee_source_parsers.h"
#import "cee_text_storage.h"
#import "cee_binary_storage.h"
#import "cee_datetime.h"
#import "cee_symbol.h"
#import "cee_reference.h"

NS_ASSUME_NONNULL_BEGIN

extern NSNotificationName CEENotificationSourceBufferChangeState;
extern NSNotificationName CEENotificationSourceBufferReload;
extern NSNotificationName CEENotificationSourceBufferSaved;

typedef NS_OPTIONS(NSUInteger, CEESourceBufferState) {
    kCEESourceBufferStateNormal = 0,
    kCEESourceBufferStateShouldSyncToFile = 1 << 1,
    kCEESourceBufferStateFileTemporary = 1 << 2,
    kCEESourceBufferStateFileDeleted = 1 << 3,
};

typedef NS_ENUM(NSUInteger, CEESourceBufferType) {
    kCEESourceBufferTypeText,
    kCEESourceBufferTypeBinary,
};

@interface CEESourceBuffer : NSObject
@property (strong) NSString* filePath;
@property (readonly) CEESourceBufferType type;
@property (readonly) cee_pointer storage;
@property (readonly) CEESourceBufferState state;

@property (readonly) CEESourceFregment* comment_fregment;
@property (readonly) CEESourceFregment* prep_directive_fregment;
@property (readonly) CEESourceFregment* statement_fregment;
@property (readonly) CEESourceTokenMap* source_token_map;
@property (readonly) CEETree* prep_directive_symbol_tree;
@property (readonly) CEETree* statement_symbol_tree;
@property (readonly) CEEList* tokens;
@property (readonly) CEESourceParserRef parser_ref;

- (void)setState:(CEESourceBufferState)state;
- (void)clearState:(CEESourceBufferState)state;
- (BOOL)stateSet:(CEESourceBufferState)state;
- (void)reload;
- (BOOL)isFileModified;
- (void)updateFileModifiedDate;
- (void)increaseReferenceCount;
- (void)decreaseReferenceCount;
- (void)parse;
- (CEEList*)symbols;
- (CEEList*)referencesInRange:(CEERange)range;
- (CEEList*)tagsInRange:(CEERange)range withDataBase:(cee_pointer)database;
@end

@interface CEESourceBufferManager : NSObject
@property (readonly, strong) NSMutableArray* buffers;
@property (strong, readonly) NSString* temporaryDirectory;

- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString *)filePath;
- (CEESourceBuffer*)openUntitledSourceBuffer;
- (void)closeSourceBuffer:(CEESourceBuffer*)buffer;
- (BOOL)saveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath;
- (void)discardUntitleSourceBuffers;
- (void)syncSourceBuffersFromFiles;
- (BOOL)isTemporaryFilePath:(NSString*)filePath;
- (NSString*)sourceType:(CEESourceBuffer*)buffer;

@end

NS_ASSUME_NONNULL_END
