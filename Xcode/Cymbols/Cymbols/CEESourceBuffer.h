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

NS_ASSUME_NONNULL_BEGIN

extern NSNotificationName CEENotificationSourceBufferStateChanged;
extern NSNotificationName CEENotificationSourceBufferHighlightSymbol;
extern NSNotificationName CEENotificationSourceBufferSymbolSelected;
extern NSNotificationName CEENotificationSourceBufferSymbolSearched;

typedef struct _CEESymbolWrapper {
    CEESourceSymbol* symbol_ref;
    cee_int level;
    cee_int index;
} CEESymbolWrapper;


typedef NS_OPTIONS(NSUInteger, CEESourceBufferState) {
    kCEESourceBufferStateNormal = 0,
    kCEESourceBufferStateModified = 1 << 1,
    kCEESourceBufferStateFileUntitled = 1 << 2,
    kCEESourceBufferStateFileDeleted = 1 << 3,
    kCEESourceBufferStateReload = 1 << 4,
    kCEESourceBufferStateFilePathChanged = 1 << 5,
};

typedef NS_ENUM(NSUInteger, CEEBufferType) {
    kCEEBufferTypeUTF8 = 0,
    kCEEBufferTypeBinary = 1,
};

typedef cee_pointer CEEBufferStorageRef;

@class CEEProject;

@interface CEESourceBuffer : NSObject

@property (strong) NSString* filePath;
@property CEEBufferType type;
@property CEEBufferStorageRef storage;
@property CEESourceSymbol* highlightedSymbol;
@property NSInteger selectedSymbolIndex;
@property CEEList* symbolsSearched;
@property CEESourceBufferState states;
@property CEESourceFregment* statement;
@property CEESourceFregment* prep_directive;
@property CEESourceFregment* comment;
@property CEESourceTokenMap* source_token_map;
@property CEEList* tokens_ref;
@property CEEList* symbol_wrappers;
@property CEESourceParserRef parser_ref;
@property CEESymbolCacheRef symbol_cache;
@property CEEProject* project_ref;
@property (readonly) NSUInteger referenceCount;

- (instancetype)initWithFilePath:(nullable NSString*)filePath;
- (void)setState:(CEESourceBufferState)state;
- (void)clearState:(CEESourceBufferState)state;
- (BOOL)stateSet:(CEESourceBufferState)state;
- (void)reload;
- (BOOL)isFileModified;
- (void)updateFileModifiedDate;
- (cee_uint)symbolWrapperLength;
- (CEESymbolWrapper*)symbolWrapperByIndex:(NSInteger)index;
- (void)highlightSymbol:(CEESourceSymbol*)symbol;
- (void)referenceIncrease;
- (void)referenceDecrease;
@end

@interface CEESourceBufferManager : NSObject

@property (strong) NSMutableArray* buffers;
- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString *)filepath;
- (CEESourceBuffer*)openUntitledSourceBuffer;
- (void)closeSourceBuffer:(CEESourceBuffer*)buffer;
- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filepath;
- (void)updateSourceBuffers;
- (BOOL)fileExistsAtPath:(NSString*)filePath;
@end

NS_ASSUME_NONNULL_END
