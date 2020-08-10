//
//  CEEUIStyleManager.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEStyleManager.h"
#import "CEEJSONReader.h"
#import "NSView+UIStyle.h"

NSNotificationName CEENotificationUserInterfaceStyleUpdate = @"CEENotificationUserInterfaceStyleUpdate";
NSNotificationName CEENotificationTextHighlightStyleUpdate = @"CEENotificationTextHighlightStyleUpdate";


@interface CEEStyleManager()
@property (strong) NSString* homeDirectory;
@end

@implementation CEEStyleManager
static CEEStyleManager* gStyleManager = nil;

@synthesize userInterfaceStyleName = _userInterfaceStyleName;
@synthesize textHighlightStyleName = _textHighlightStyleName;


+ (CEEStyleManager*)defaultStyleManager {
    if (!gStyleManager)
        gStyleManager = [[CEEStyleManager alloc] init];
    return gStyleManager;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    _userInterfaceConfiguration = [[CEEUserInterfaceStyleConfiguration alloc] init];
    
    return self;
}

- (void)setStyleHomeDirectory:(NSString *)directory {
    _homeDirectory = directory;
    [self copyStylesToHomeDirectory];
}

- (void)copyStylesToHomeDirectory {
    if (![[NSFileManager defaultManager] fileExistsAtPath:[self homeDirectory] isDirectory:nil])
        [[NSFileManager defaultManager] createDirectoryAtPath:[self homeDirectory] withIntermediateDirectories:YES attributes:nil error:nil];
    
    NSString* directoryInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Styles"];
    NSArray* contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:directoryInBundle error:nil];
    for (NSString* fileName in contents) {
        NSString* src = [directoryInBundle stringByAppendingPathComponent:fileName];
        NSString* dest = [[self homeDirectory] stringByAppendingPathComponent:fileName];
        if ([[NSFileManager defaultManager] fileExistsAtPath:dest])
            [[NSFileManager defaultManager] removeItemAtPath:dest error:nil];
        [[NSFileManager defaultManager] copyItemAtPath:src toPath:dest error:nil];
    }
}

- (NSArray*)filenamesWithExtension:(NSString*)extension inDirectory:(NSString*)directory {
    NSMutableArray* filepaths = [[NSMutableArray alloc] init];
    NSArray* contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:directory error:nil];
    NSString* filepath = nil;
    
    for (NSString* filename in contents) {
        if ([[filename pathExtension] caseInsensitiveCompare:extension] == NSOrderedSame) {
            filepath = [directory stringByAppendingPathComponent:filename];
            [filepaths addObject: filepath];
        }
    }
        
    NSArray* sort = [filepaths sortedArrayUsingComparator:^(id a, id b) {
        return [a compare:b options:NSLiteralSearch];
    }];
    
    return sort;
}

- (NSArray*)userInterfaceStylePaths {
    return [self filenamesWithExtension:@"ui" inDirectory:[self homeDirectory]];
}

- (NSArray*)userInterfaceStyleNames {
    NSArray* filepaths = [self userInterfaceStylePaths];
    NSMutableArray* filenames = [[NSMutableArray alloc] init];
    
    for (NSString* filepath in filepaths) 
        [filenames addObject:[[filepath lastPathComponent] stringByDeletingPathExtension]];
    
    NSArray* sort = [filenames sortedArrayUsingComparator:^(id a, id b) {
        return [a compare:b options:NSLiteralSearch];
    }];
    
    return sort;
}

- (NSArray*)textHighlightStylePaths {
    return [self filenamesWithExtension:@"syntax" inDirectory:[self homeDirectory]];
}

- (NSArray*)textHighlightStyleNames {
    NSArray* filepaths = [self textHighlightStylePaths];
    NSMutableArray* filenames = [[NSMutableArray alloc] init];
    
    for (NSString* filepath in filepaths) 
        [filenames addObject:[[filepath lastPathComponent] stringByDeletingPathExtension]];
    
    NSArray* sort = [filenames sortedArrayUsingComparator:^(id a, id b) {
        return [a compare:b options:NSLiteralSearch];
    }];
    
    return sort;
}

- (void)setUserInterfaceStyleName:(NSString *)name {
    _userInterfaceStyleName = name;    
    NSString* filepath = [[self homeDirectory] stringByAppendingPathComponent:[_userInterfaceStyleName stringByAppendingPathExtension:@"ui"]];
    NSDictionary* descriptor = [CEEJSONReader objectFromFile:filepath];
    [_userInterfaceConfiguration setDescriptor:descriptor];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationUserInterfaceStyleUpdate object:self];
}

- (void)setUserInterfaceStyle:(NSDictionary*)descriptor {
    [_userInterfaceConfiguration setDescriptor:descriptor];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationUserInterfaceStyleUpdate object:self];
}

- (NSString*)userInterfaceStyleName {
    return _userInterfaceStyleName;
}

- (void)setTextHighlightStyleName:(NSString *)name {
    _textHighlightStyleName = name;
    NSString* filepath = [[self homeDirectory] stringByAppendingPathComponent:[_textHighlightStyleName stringByAppendingPathExtension:@"syntax"]];
    _textHighlighDescriptor = [CEEJSONReader stringFromFile:filepath];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationTextHighlightStyleUpdate object:self];
}

- (void)setTextHighlightDescriptor:(NSString*)descriptor {
    _textHighlighDescriptor = descriptor;
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationTextHighlightStyleUpdate object:self];
}

- (NSString*)textHighlightStyleName {
    return _textHighlightStyleName;
}

@end


@implementation CEEUserInterfaceStyleConfiguration

+ (NSColor*)createColorFromString:(NSString*)string {
    NSColor* color = nil;
    NSString* patternString = @"(rgba|hsba)\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    
    for (NSTextCheckingResult *match in matches) {
        if ([[string substringWithRange:[match rangeAtIndex:1]] caseInsensitiveCompare:@"rgba"] == NSOrderedSame) {
            CGFloat r = [[string substringWithRange:[match rangeAtIndex:2]] floatValue];
            CGFloat g = [[string substringWithRange:[match rangeAtIndex:3]] floatValue];
            CGFloat b = [[string substringWithRange:[match rangeAtIndex:4]] floatValue];
            CGFloat a = [[string substringWithRange:[match rangeAtIndex:5]] floatValue];
            color = [NSColor colorWithRed:r/256.0 green:g/256.0 blue:b/256.0 alpha:a/100.0];
        }
        else if ([[string substringWithRange:[match rangeAtIndex:1]] caseInsensitiveCompare:@"hsba"] == NSOrderedSame) {
            CGFloat h = [[string substringWithRange:[match rangeAtIndex:2]] floatValue];
            CGFloat s = [[string substringWithRange:[match rangeAtIndex:3]] floatValue];
            CGFloat b = [[string substringWithRange:[match rangeAtIndex:4]] floatValue];
            CGFloat a = [[string substringWithRange:[match rangeAtIndex:5]] floatValue];
            color = [NSColor colorWithHue:h/360.0 saturation:s/100.0 brightness:b/100.0 alpha:a/100.0];
        }
    }
    return color;
}

+ (NSFont*)createFontFromString:(NSString*)string {
    NSFont* font = nil;
    NSString* patternString = @"font\\s*\\(\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*,\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    for (NSTextCheckingResult *match in matches) {
        NSString* family = [string substringWithRange:[match rangeAtIndex:1]];
        CGFloat size = [[string substringWithRange:[match rangeAtIndex:2]] floatValue];
        NSFontTraitMask traits = 0;
        NSString* style = [string substringWithRange:[match rangeAtIndex:3]];
        if ([style rangeOfString:@"bold" options:NSCaseInsensitiveSearch].location != NSNotFound)
            traits |= NSBoldFontMask;
        if ([style rangeOfString:@"italic" options:NSCaseInsensitiveSearch].location != NSNotFound)
            traits |= NSItalicFontMask;
        NSInteger weight = [[string substringWithRange:[match rangeAtIndex:4]] integerValue];
        font = [[NSFontManager sharedFontManager] fontWithFamily:family traits:traits weight:weight size:size];
    }
    return font;
}

+ (NSGradient*)createGradientFromString:(NSString*)string {
    NSGradient* gradient = nil;
    NSString* patternString = @"gradient\\s*\\(\\s*((?:rgba|hsba)\\s*\\(\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*\\))\\s*,\\s*((?:rgba|hsba)\\s*\\(\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*\\))\\s*\\)";
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    for (NSTextCheckingResult *match in matches) {
        NSColor* color0 = [CEEUserInterfaceStyleConfiguration createColorFromString:[string substringWithRange:[match rangeAtIndex:1]]];
        NSColor* color1 = [CEEUserInterfaceStyleConfiguration createColorFromString:[string substringWithRange:[match rangeAtIndex:2]]];
        gradient = [[NSGradient alloc] initWithStartingColor:color0 endingColor:color1];
    }
    return gradient;
}

+ (NSShadow*)createShadowFromString:(NSString*)string {
    NSShadow* shadow = nil;
    NSString* patternString = @"shadow\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*((?:rgba|hsba)\\s*\\(\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*,\\s*[0-9.]+\\s*\\))\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    for (NSTextCheckingResult *match in matches) {
        shadow = [[NSShadow alloc] init];
        NSColor* color = [CEEUserInterfaceStyleConfiguration createColorFromString:[string substringWithRange:[match rangeAtIndex:4]]];
        CGFloat radius = [[string substringWithRange:[match rangeAtIndex:1]] floatValue];
        CGFloat offsetX = [[string substringWithRange:[match rangeAtIndex:2]] floatValue];
        CGFloat offsetY = [[string substringWithRange:[match rangeAtIndex:3]] floatValue];
        [shadow setShadowColor:color];
        [shadow setShadowBlurRadius:radius];
        [shadow setShadowOffset:NSMakeSize(offsetX, offsetY)];
    }
    return shadow;
}

- (NSString*)selectorFromIdentifier:(NSString*)identifier {
    NSString* selector = nil;
    NSInteger location = [identifier rangeOfString:@"#"].location;
    if (location != NSNotFound)
        selector = [identifier substringWithRange:NSMakeRange(0, location)];
    else
        selector = identifier;
    selector = [selector stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    return selector;
}

- (CEEViewStyle)getStyleFromSchemeSelector:(NSString*)selector {
    CEEViewStyle style = kCEEViewStyleInit;
    NSString* descriptor = nil;
    NSInteger colon = [selector rangeOfString:@":"].location;
    if (colon != NSNotFound) {
        descriptor = [selector substringWithRange:NSMakeRange(colon, selector.length - colon)];
        if ([descriptor containsString:@"selected"])
            style |= kCEEViewStyleSelected;
        if ([descriptor containsString:@"actived"])
            style |= kCEEViewStyleActived;
        if ([descriptor containsString:@"clicked"])
            style |= kCEEViewStyleClicked;
        if ([descriptor containsString:@"disabled"])
            style |= kCEEViewStyleDisabled;
        if ([descriptor containsString:@"highlighted"])
            style |= kCEEViewStyleHeighLighted;
    }
    return style;
}

- (BOOL)selectorContainStyleDescriptor:(NSString*)key {
    return [key rangeOfString:@":"].location != NSNotFound;
}

- (NSString*)getIdentifierFromSchemeSelector:(NSString*)selector {
    NSString* identifier = nil;
    NSInteger colon = [selector rangeOfString:@":"].location;
    if (colon != NSNotFound)
        identifier = [selector substringWithRange:NSMakeRange(0, colon)];
    else
        identifier = selector;
    
    identifier = [identifier stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    return identifier;
}

- (void)enumerateSchemes:(NSMutableArray**)schemes fromDescriptor:(NSDictionary*)descriptor withIdentifier:(NSString*)identifier {
    NSArray* identifiers = [identifier componentsSeparatedByString:@"."];
    BOOL isTail = NO;
    NSString* current = nil;
    NSString* suffix = nil;
    NSRange suffixRange;
    CEEUserInterfaceStyleScheme* scheme = nil;
    NSString* schemeIdentifier = nil;
    CEEViewStyle style = kCEEViewStyleInit;
    
    current = [self selectorFromIdentifier:identifiers[0]];
    if (identifiers.count == 1) {
        isTail = YES;
        suffix = nil;
    }
    else {
        suffixRange = NSMakeRange(current.length + 1, identifier.length - (current.length + 1));
        suffix = [identifier substringWithRange:suffixRange];
    }
    
    for (NSString* selector in descriptor.allKeys) {
        schemeIdentifier = [self getIdentifierFromSchemeSelector:selector];
        if ([schemeIdentifier compare:current options:NSLiteralSearch] == NSOrderedSame) {
            style = [self getStyleFromSchemeSelector:selector];
            if (isTail) {
                if (!*schemes)
                    *schemes = [[NSMutableArray alloc] init];
                
                scheme = [[CEEUserInterfaceStyleScheme alloc] init];
                scheme.style = style;
                scheme.descriptor = descriptor[selector];
                [(*schemes) addObject:scheme];
            }
            else {
                [self enumerateSchemes:schemes fromDescriptor:descriptor[selector] withIdentifier:suffix];
            }
        }
    }
}

- (void)configureView:(__kindof NSView*)view {
    if (!_descriptor)
        return;
    
    NSString* selector = [view getSchemeIdentifier];
    if (!selector)
        selector = [view getSchemeClassName];
    
    NSMutableArray* schemes = nil;
    [self enumerateSchemes:&schemes fromDescriptor:_descriptor withIdentifier:selector];
    [view setSytleSchemes:schemes];
}

@end
