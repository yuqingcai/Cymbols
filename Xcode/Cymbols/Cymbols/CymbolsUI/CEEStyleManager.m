//
//  CEEUIStyleManager.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEEJSONReader.h"
#import "NSView+UIStyle.h"
#import "cee_datetime.h"

NSNotificationName CEENotificationUserInterfaceStyleUpdate = @"CEENotificationUserInterfaceStyleUpdate";
NSNotificationName CEENotificationTextHighlightStyleUpdate = @"CEENotificationTextHighlightStyleUpdate";
NSNotificationName CEENotificationUserInterfaceStyleUpdateComplete = @"CEENotificationUserInterfaceStyleUpdateComplete";
NSNotificationName CEENotificationTextHighlightStyleUpdateComplete = @"CEENotificationTextHighlightStyleUpdateComplete";

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
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationConfigurationChanged:) name:CEENotificationApplicationConfigurationChanged object:nil];
    
    return self;
}

- (void)applicationConfigurationChanged:(NSNotification*)notification {
    AppDelegate* delegate = [NSApp delegate];
    NSDictionary* configuration = [delegate configuration];
    NSString* name = nil;
    
    name = configuration[CEEApplicationConfigurationNameUIStyle];
    if ([name compare:_userInterfaceStyleName options:NSCaseInsensitiveSearch] != NSOrderedSame)
        [[CEEStyleManager defaultStyleManager] setUserInterfaceStyleName:name];
    
    name = configuration[CEEApplicationConfigurationNameTextHighlightStyle];
    if ([name compare:_textHighlightStyleName options:NSCaseInsensitiveSearch] != NSOrderedSame)
        [[CEEStyleManager defaultStyleManager] setTextHighlightStyleName:name];
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

- (NSArray*)filePathsWithExtension:(NSString*)extension inDirectory:(NSString*)directory {
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
    return [self filePathsWithExtension:@"ui" inDirectory:_directory];
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
    return [self filePathsWithExtension:@"syntax" inDirectory:_directory];
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

- (void)setUserInterfaceStyleName:(NSString*)name {
    _userInterfaceStyleName = name;
    NSString* filepath = [[self directory] stringByAppendingPathComponent:[_userInterfaceStyleName stringByAppendingPathExtension:@"ui"]];
    NSDictionary* descriptor = [CEEJSONReader objectFromFile:filepath];
    [_userInterfaceConfiguration setDescriptor:descriptor];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationUserInterfaceStyleUpdate object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationUserInterfaceStyleUpdateComplete object:self];
}

- (NSString*)userInterfaceStyleName {
    return _userInterfaceStyleName;
}

- (void)setTextHighlightStyleName:(NSString *)name {
    _textHighlightStyleName = name;
    NSString* filepath = [[self directory] stringByAppendingPathComponent:[_textHighlightStyleName stringByAppendingPathExtension:@"syntax"]];
    _textHighlightDescriptor = [CEEJSONReader stringFromFile:filepath];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationTextHighlightStyleUpdate object:self];
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationTextHighlightStyleUpdateComplete object:self];
}

- (NSString*)textHighlightStyleName {
    return _textHighlightStyleName;
}

- (NSImage*)iconFromFileName:(NSString*)fileName {
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

- (NSImage*)iconFromFilePath:(NSString*)filePath {
    BOOL isDirectory = NO;
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory])
        return [NSImage imageNamed:@"icon_file_not_existed_16x16"];
    
    if (isDirectory) 
        return [NSImage imageNamed:@"icon_directory_16x16"];
    
    return [self iconFromFileName:[filePath lastPathComponent]];
}

- (NSImage*)iconFromSymbol:(CEESourceSymbol*)symbol {
    NSString* identifier = nil;
    switch (symbol->type) {
            
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

- (NSImage*)iconFromName:(NSString*)name {
    return [NSImage imageNamed:name];
}

@end
