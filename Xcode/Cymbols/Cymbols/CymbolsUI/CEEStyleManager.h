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

@class CEEUserInterfaceStyleConfiguration;

@interface CEEStyleManager : NSObject
@property (strong) CEEUserInterfaceStyleConfiguration* userInterfaceConfiguration;
@property (strong) NSString* userInterfaceStyleName;
@property (strong) NSString* textHighlightStyleName;
@property (strong) NSString* textHighlighDescriptor;
+ (CEEStyleManager*)defaultStyleManager;
- (void)setDirectory:(NSString*)directory;
- (NSArray*)userInterfaceStylePaths;
- (NSArray*)textHighlightStylePaths;
- (NSArray*)userInterfaceStyleNames;
- (NSArray*)textHighlightStyleNames;
- (void)setUserInterfaceStyle:(NSDictionary*)descriptor;
- (void)setTextHighlightDescriptor:(NSString*)descriptor;
- (NSImage*)filetypeIconFromFileName:(NSString*)fileName;
- (NSImage*)filetypeIconFromFilePath:(NSString*)filePath;
- (NSImage*)symbolIconFromSymbolType:(CEESourceSymbolType)type;
- (NSImage*)iconFromName:(NSString*)name;
@end


@interface CEEUserInterfaceStyleConfiguration : NSObject
@property (strong) NSDictionary* descriptor;
+ (NSColor*)createColorFromString:(NSString*)string;
+ (NSFont*)createFontFromString:(NSString*)string;
+ (NSGradient*)createGradientFromString:(NSString*)string;
+ (NSShadow*)createShadowFromString:(NSString*)string;
- (void)configureView:(__kindof NSView*)view;
@end

NS_ASSUME_NONNULL_END
