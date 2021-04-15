//
//  CEEStyleManager.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "cee_symbol.h"

NS_ASSUME_NONNULL_BEGIN

extern NSNotificationName CEENotificationUserInterfaceStyleUpdate;
extern NSNotificationName CEENotificationTextHighlightStyleUpdate;
extern NSNotificationName CEENotificationUserInterfaceStyleUpdateComplete;
extern NSNotificationName CEENotificationTextHighlightStyleUpdateComplete;

@class CEEUserInterfaceStyleConfiguration;

@interface CEEStyleManager : NSObject
@property (strong) CEEUserInterfaceStyleConfiguration* userInterfaceConfiguration;
@property (strong) NSString* userInterfaceStyleName;
@property (strong) NSString* textHighlightStyleName;
@property (strong, readonly) NSString* textHighlightDescriptor;
+ (CEEStyleManager*)defaultStyleManager;
- (void)setDirectory:(NSString*)directory;
- (NSArray*)userInterfaceStyleNames;
- (NSArray*)textHighlightStyleNames;
- (NSImage*)iconFromFileName:(NSString*)fileName;
- (NSImage*)iconFromFilePath:(NSString*)filePath;
- (NSImage*)iconFromSymbol:(CEESourceSymbol*)symbol;
- (NSImage*)iconFromName:(NSString*)name;

@end


NS_ASSUME_NONNULL_END
