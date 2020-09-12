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

extern NSNotificationName CEENotificationSourceBufferStateChanged;

typedef struct _CEESymbolWrapper {
    CEESourceSymbol* symbol_ref;
    cee_int level;
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

@class CEEProject;

@interface CEESourceBuffer : NSObject

@property (strong) NSString* filePath;
@property CEEBufferType type;
@property CEETextStorageRef storage;
@property CEESourceBufferState state;
@property CEESourceFregment* comment;
@property CEESourceFregment* prep_directive;
@property CEESourceFregment* statement;
@property CEESourceTokenMap* source_token_map;
@property CEETree* prep_directive_symbol_tree;
@property CEETree* statement_symbol_tree;

@property CEEList* tokens_ref;
@property CEESourceParserRef parser_ref;
@property CEEList* symbol_wrappers;

- (instancetype)initWithFilePath:(nullable NSString*)filePath;
- (void)setState:(CEESourceBufferState)state;
- (void)clearState:(CEESourceBufferState)state;
- (BOOL)stateSet:(CEESourceBufferState)state;
- (void)reload;
- (BOOL)isFileModified;
- (void)updateFileModifiedDate;
- (void)referenceIncrease;
- (void)referenceDecrease;

@end

typedef enum _CEESourceBufferParserOption {
    kCEESourceBufferParserOptionCreateSymbolWrapper = 1 << 0,
} CEESourceBufferParserOption;

void cee_source_buffer_parse(CEESourceBuffer* buffer,
                             CEESourceBufferParserOption options);

@interface CEESourceBufferManager : NSObject

@property (strong) NSMutableArray* buffers;
- (CEESourceBuffer*)openSourceBufferWithFilePath:(NSString *)filePath;
- (CEESourceBuffer*)openUntitledSourceBuffer;
- (void)closeSourceBuffer:(CEESourceBuffer*)buffer;
- (void)saveSourceBuffer:(CEESourceBuffer*)buffer atPath:(NSString*)filePath;
- (CEESourceBuffer*)getSourceBufferWithFilePath:(NSString*)filePath;
- (void)updateSourceBuffers;
- (BOOL)fileExistsAtPath:(NSString*)filePath;
- (BOOL)hasModifiedBuffer;

@end

NS_ASSUME_NONNULL_END
