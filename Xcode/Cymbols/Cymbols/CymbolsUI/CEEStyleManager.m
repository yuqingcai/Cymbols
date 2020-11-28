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
#import "CEEScroller.h"

NSNotificationName CEENotificationUserInterfaceStyleUpdate = @"CEENotificationUserInterfaceStyleUpdate";
NSNotificationName CEENotificationTextHighlightStyleUpdate = @"CEENotificationTextHighlightStyleUpdate";


@interface CEEStyleManager()
@property (strong) NSString* directory;
@end

@implementation CEEStyleManager
static CEEStyleManager* gStyleManager = nil;

@synthesize userInterfaceStyleName = _userInterfaceStyleName;
@synthesize textHighlightStyleName = _textHighlightStyleName;
@synthesize directory = _directory;

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

- (void)setDirectory:(NSString *)directory {
    _directory = directory;
    [self copyStylesToDirectory];
}

- (NSString*)directory {
    return _directory;
}

- (void)copyStylesToDirectory {
    if (![[NSFileManager defaultManager] fileExistsAtPath:[self directory] isDirectory:nil])
        [[NSFileManager defaultManager] createDirectoryAtPath:[self directory] withIntermediateDirectories:YES attributes:nil error:nil];
    
    NSString* directoryInBundle = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Styles"];
    NSArray* contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:directoryInBundle error:nil];
    for (NSString* fileName in contents) {
        NSString* src = [directoryInBundle stringByAppendingPathComponent:fileName];
        NSString* dest = [[self directory] stringByAppendingPathComponent:fileName];
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
    return [self filenamesWithExtension:@"ui" inDirectory:[self directory]];
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
    return [self filenamesWithExtension:@"syntax" inDirectory:[self directory]];
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
    NSString* filepath = [[self directory] stringByAppendingPathComponent:[_userInterfaceStyleName stringByAppendingPathExtension:@"ui"]];
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
    NSString* filepath = [[self directory] stringByAppendingPathComponent:[_textHighlightStyleName stringByAppendingPathExtension:@"syntax"]];
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

- (NSImage*)filetypeIconFromFileName:(NSString*)fileName {
    NSString* identifier = @"icon_file_type_common_16x16";
    NSString* extension = [fileName pathExtension];
    
    if ([fileName caseInsensitiveCompare:@"Makefile"] == NSOrderedSame)
        identifier = @"icon_file_type_makefile_16x16";
    
    if ([extension caseInsensitiveCompare:@"c"] == NSOrderedSame)
        identifier = @"icon_file_type_c_16x16";
        
    if ([extension caseInsensitiveCompare:@"cpp"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"cc"] == NSOrderedSame)
        identifier = @"icon_file_type_cpp_16x16";
    
    if ([extension caseInsensitiveCompare:@"h"] == NSOrderedSame)
        identifier = @"icon_file_type_c_header_16x16";
    
    if ([extension caseInsensitiveCompare:@"hpp"] == NSOrderedSame)
        identifier = @"icon_file_type_c_header_16x16";
    
    if ([extension caseInsensitiveCompare:@"m"] == NSOrderedSame)
        identifier = @"icon_file_type_objective_c_16x16";
    
    if ([extension caseInsensitiveCompare:@"mm"] == NSOrderedSame)
        identifier = @"icon_file_type_cpp_16x16";
        
    if ([extension caseInsensitiveCompare:@"asm"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"s"] == NSOrderedSame)
        identifier = @"icon_file_type_asm_16x16";
    
    if ([extension caseInsensitiveCompare:@"txt"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"text"]== NSOrderedSame)
        identifier = @"icon_file_type_text_16x16";
    
    if ([extension caseInsensitiveCompare:@"html"] == NSOrderedSame)
        identifier = @"icon_file_type_html_16x16";
    
    if ([extension caseInsensitiveCompare:@"js"] == NSOrderedSame)
        identifier = @"icon_file_type_javascript_16x16";
    
    if ([extension caseInsensitiveCompare:@"java"] == NSOrderedSame)
        identifier = @"icon_file_type_java_16x16";
    
    if ([extension caseInsensitiveCompare:@"swift"] == NSOrderedSame)
        identifier = @"icon_file_type_swift_16x16";
    
    if ([extension caseInsensitiveCompare:@"png"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"jpg"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"jpeg"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"tft"] == NSOrderedSame ||
        [extension caseInsensitiveCompare:@"bmp"] == NSOrderedSame)
        identifier = @"icon_file_type_image_16x16";
    
    if ([extension caseInsensitiveCompare:@"css"] == NSOrderedSame)
        identifier = @"icon_file_type_css_16x16";
    
    if ([extension caseInsensitiveCompare:@"cs"] == NSOrderedSame)
        identifier = @"icon_file_type_csharp_16x16";
    
    return [NSImage imageNamed:identifier];
}

- (NSImage*)filetypeIconFromFilePath:(NSString*)filePath {
    BOOL isDirectory = NO;
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory])
        return [NSImage imageNamed:@"icon_file_not_existed_16x16"];
    
    if (isDirectory) 
        return [NSImage imageNamed:@"icon_directory_16x16"];
    
    return [self filetypeIconFromFileName:[filePath lastPathComponent]];
}

- (NSImage*)symbolIconFromSymbolType:(CEESourceSymbolType)type {
    NSString* identifier = nil;
    switch (type) {
            
        case kCEESourceSymbolTypeFunctionDefinition:
            identifier = @"icon_function_definition_16x16";
            break;
        case kCEESourceSymbolTypeMessageDefinition:
            identifier = @"icon_message_definition_16x16";
            break;
        case kCEESourceSymbolTypeFunctionDeclaration:
            identifier = @"icon_function_declaration_16x16";
            break;
        case kCEESourceSymbolTypeMessageDeclaration:
            identifier = @"icon_message_declaration_16x16";
            break;
        case kCEESourceSymbolTypeVariableDeclaration:
        case kCEESourceSymbolTypeLabel:
        case kCEESourceSymbolTypeFunctionParameter:
        case kCEESourceSymbolTypeMessageParameter:
            identifier = @"icon_variable_declaration_16x16";
            break;
        case kCEESourceSymbolTypeCustomTypeDeclaration:
            identifier = @"icon_custom_type_declaration_16x16";
            break;
        case kCEESourceSymbolTypePropertyDeclaration:
            identifier = @"icon_property_declaration_16x16";
            break;
        case kCEESourceSymbolTypeTypeDefine:
            identifier = @"icon_typedef_16x16";
            break;
        case kCEESourceSymbolTypeEnumerator:
            identifier = @"icon_enumerator_16x16";
            break;
        case kCEESourceSymbolTypePrepDirectiveDefine:
        case kCEESourceSymbolTypePrepDirectiveInclude:
        case kCEESourceSymbolTypePrepDirectiveParameter:
        case kCEESourceSymbolTypePrepDirectiveCommon:
            identifier = @"icon_prep_directive_define_16x16";
            break;
        case kCEESourceSymbolTypeTemplateDeclaration:
            identifier = @"icon_template_declaration_16x16";
            break;
        case kCEESourceSymbolTypeNamespaceDefinition:
            identifier = @"icon_namespace_definition_16x16";
            break;
        case kCEESourceSymbolTypeClassDefinition:
            identifier = @"icon_class_definition_16x16";
            break;
        case kCEESourceSymbolTypeStructDefinition:
            identifier = @"icon_class_definition_16x16";
            break;
        case kCEESourceSymbolTypeEnumDefinition:
            identifier = @"icon_enum_definition_16x16";
            break;
        case kCEESourceSymbolTypeUnionDefinition:
            identifier = @"icon_union_definition_16x16";
            break;
        case kCEESourceSymbolTypeInterfaceDefinition:
            identifier = @"icon_interface_declaration_16x16";
            break;
        case kCEESourceSymbolTypeInterfaceDeclaration:
            identifier = @"icon_interface_declaration_16x16";
            break;
        case kCEESourceSymbolTypeImplementationDefinition:
            identifier = @"icon_implementation_definition_16x16";
            break;
        case kCEESourceSymbolTypeProtocolDeclaration:
            identifier = @"icon_protocol_declaration_16x16";
            break;
        case kCEESourceSymbolTypeExternBlock:
            identifier = @"icon_source_block_16x16";
            break;
        case kCEESourceSymbolTypeXMLTagName:
            identifier = @"icon_xml_tag_16x16";
            break;
        case kCEESourceSymbolTypeCSSSelector:
            identifier = @"icon_css_selector_16x16";
            break;
        case kCEESourceSymbolTypeImport:
            identifier = @"icon_import_16x16";
            break;
        case kCEESourceSymbolTypePackage:
            identifier = @"icon_package_16x16";
            break;
        case kCEESourceSymbolTypeOperatorOverloadDefinition:
            identifier = @"icon_function_definition_16x16";
            break;
        case kCEESourceSymbolTypeOperatorOverloadDeclaration:
            identifier = @"icon_function_declaration_16x16";
            break;
            
        case kCEESourceSymbolTypeUsingDeclaration:
            identifier = @"icon_using_declaration_16x16";
            break;
            
        default:
            break;
    }
    
    return [NSImage imageNamed:identifier];
}

@end


@implementation CEEUserInterfaceStyleConfiguration

+ (NSColor*)createColorFromString:(NSString*)string {
    NSColor* color = nil;
    NSString* patternString = @"(rgba|hsba)\\s*\\(\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\s*\\)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    if (matches.count) {
        for (NSTextCheckingResult *match in matches) {
            if ([[string substringWithRange:[match rangeAtIndex:1]] caseInsensitiveCompare:@"rgba"] == NSOrderedSame) {
                CGFloat r = [[string substringWithRange:[match rangeAtIndex:2]] floatValue] / 256.0;
                CGFloat g = [[string substringWithRange:[match rangeAtIndex:3]] floatValue] / 256.0;
                CGFloat b = [[string substringWithRange:[match rangeAtIndex:4]] floatValue] / 256.0;
                CGFloat a = [[string substringWithRange:[match rangeAtIndex:5]] floatValue] / 100.0;
                color = [NSColor colorWithSRGBRed:r green:g blue:b alpha:a];
                
            }
            else if ([[string substringWithRange:[match rangeAtIndex:1]] caseInsensitiveCompare:@"hsba"] == NSOrderedSame) {
                CGFloat h = [[string substringWithRange:[match rangeAtIndex:2]] floatValue] / 360.0;
                CGFloat s = [[string substringWithRange:[match rangeAtIndex:3]] floatValue] / 100.0;
                CGFloat b = [[string substringWithRange:[match rangeAtIndex:4]] floatValue] / 100.0;
                CGFloat a = [[string substringWithRange:[match rangeAtIndex:5]] floatValue] / 100.0;
                color = [NSColor colorWithHue:h saturation:s brightness:b alpha:a];
                color = [color colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
            
            }
        }
    }
    else {
        patternString = @"#[0-9a-fA-F]+";
        regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
        matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
        if (matches.count) {
            unsigned int hexValue = 0;
            NSString* hexString = [string substringFromIndex:1];
            NSScanner* scanner = [NSScanner scannerWithString:hexString];
            [scanner scanHexInt:&hexValue];
            CGFloat r = (CGFloat)(((unsigned char)(hexValue >> 16)) / 256.0);
            CGFloat g = (CGFloat)(((unsigned char)(hexValue >> 8)) / 256.0);
            CGFloat b = (CGFloat)(((unsigned char)(hexValue)) / 256.0);
            color = [NSColor colorWithSRGBRed:r green:g blue:b alpha:1.0];
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

- (CEEViewStyleState)getStyleFromSchemeSelector:(NSString*)selector {
    CEEViewStyleState state = kCEEViewStyleStateActived;
    NSString* descriptor = nil;
    NSString* pattern = @"([a-zA-Z0-9_]+)\\s*(\\[\\s*([a-zA-Z0-9]+)\\s*\\])?";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:pattern options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:selector options:0 range:NSMakeRange(0, [selector length])];
    if (matches.count) {
        for (NSTextCheckingResult *match in matches)
            if (match.numberOfRanges == 4)
                descriptor = [selector substringWithRange:[match rangeAtIndex:3]];
    }
    if ([descriptor caseInsensitiveCompare:@"actived"] == NSOrderedSame)
        state = kCEEViewStyleStateActived;
    if ([descriptor caseInsensitiveCompare:@"deactived"] == NSOrderedSame)
        state = kCEEViewStyleStateDeactived;
    if ([descriptor caseInsensitiveCompare:@"clicked"] == NSOrderedSame)
        state = kCEEViewStyleStateClicked;
    if ([descriptor caseInsensitiveCompare:@"disabled"] == NSOrderedSame)
        state = kCEEViewStyleStateDisabled;
    if ([descriptor caseInsensitiveCompare:@"highlighted"] == NSOrderedSame)
        state = kCEEViewStyleStateHeighLighted;
    
    return state;
}

- (NSString*)getIdentifierFromSchemeSelector:(NSString*)selector {
    
    NSString* identifier = nil;
    NSString* pattern = @"([a-zA-Z0-9_]+)\\s*(\\[\\s*([a-zA-Z0-9]+)\\s*\\])?";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:pattern options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:selector options:0 range:NSMakeRange(0, [selector length])];
    if (matches.count) {
        for (NSTextCheckingResult *match in matches)
            identifier = [selector substringWithRange:[match rangeAtIndex:1]];
    }
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
    CEEViewStyleState state = kCEEViewStyleStateDeactived;
    
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
            state = [self getStyleFromSchemeSelector:selector];
            if (isTail) {
                if (!*schemes)
                    *schemes = [[NSMutableArray alloc] init];
                
                scheme = [[CEEUserInterfaceStyleScheme alloc] init];
                scheme.styleState = state;
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
        
    NSMutableArray* schemes = nil;
    BOOL useClassIdentifier = NO;
    NSString* selector = [view getSchemeObjectIdentifier];
    if (!selector) {
        useClassIdentifier = YES;
        selector = [view getSchemeClassIdentifier];
    }
    [self enumerateSchemes:&schemes fromDescriptor:_descriptor withIdentifier:selector];
    
    if (!schemes && !useClassIdentifier) {
        selector = [view getSchemeClassIdentifier];
        [self enumerateSchemes:&schemes fromDescriptor:_descriptor withIdentifier:selector];
    }
    
    [view setSytleSchemes:schemes];
    return;
}

@end
