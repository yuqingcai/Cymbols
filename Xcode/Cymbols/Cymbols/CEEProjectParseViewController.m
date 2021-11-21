//
//  CEEProjectParseViewController.m
//  Cymbols
//
//  Created by qing on 2020/8/25.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProjectParseViewController.h"
#import "CEETextLabel.h"
#import "CEETextTitle.h"
#import "cee_backend.h"
#import "CEEButton.h"
#import "CEEImageView.h"

@interface CEEProjectParseViewController ()
@property (strong) CEEProject* project;
@property (weak) IBOutlet NSProgressIndicator *progressBar;
@property (weak) IBOutlet CEETextTitle *label0;
@property (weak) IBOutlet CEETextLabel *label1;
@property (weak) IBOutlet CEEButton *button;
@property (weak) IBOutlet CEEImageView *icon;

@property BOOL cancel;

@end

@implementation CEEProjectParseViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    _sync = NO;
}

- (void)viewWillAppear {
    [super viewWillAppear];
    if (self->_sync)
        [self->_label0 setStringValue:@"Syncing Project..."];
    else
        [self->_label0 setStringValue:@"Building Project..."];
    [_label1 setStringValue:@""];
    [_button setTitle:@"Stop"];
    [_button setHighlighted:NO];
    [_progressBar setMinValue:0.0];
    [_progressBar setMaxValue:1.0];
    [_progressBar setDoubleValue:0.0];
    
    if (_sync)
        [_icon setImage:[NSImage imageNamed:@"icon_database_sync_16x16"]];
    else
        [_icon setImage:[NSImage imageNamed:@"icon_database_build_16x16"]];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    __block AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    
    _cancel = NO;
    
    __block NSDateFormatter *dateFormatter = [self createISO8601DateFormatter];
    
    [self saveModifiedBuffers];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        CEEList* infos = cee_database_file_reference_infos_get(self->_project.database);
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self->_progressBar setMinValue:0.0];
            [self->_progressBar setMaxValue:cee_list_length(infos)];
            [self->_progressBar setDoubleValue:0.0];
        });
        
        // when _sync is NOT TRUE, that means we are rebuilding the whole project
        if (!self->_sync) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self->_label0 setStringValue:@"Cleaning Database..."];
            });
            
            cee_database_symbols_delete(self->_project.database);
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                if (self->_sync)
                    [self->_label0 setStringValue:@"Syncing Project..."];
                else
                    [self->_label0 setStringValue:@"Building Project..."];
            });
        }
        
        int i = 0;
        CEEList* p = infos;
        CEEList* unreferences = NULL;
        
        CEESourceSymbolType filter_types[] = {
            kCEESourceSymbolTypePrepDirectiveInclude,
            kCEESourceSymbolTypePrepDirectiveIncludePath,
            kCEESourceSymbolTypeImport,
            kCEESourceSymbolTypeUsingDeclaration,
            kCEESourceSymbolTypePackage,
            kCEESourceSymbolTypeXMLTagStart,
            kCEESourceSymbolTypeXMLTagEnd,
            kCEESourceSymbolTypeXMLTagName,
            kCEESourceSymbolTypeXMLTagAttribute,
            kCEESourceSymbolTypeCSSSelector,
            0,
        };
        
        while (p) {
            
            @autoreleasepool {
                
                CEEFileReferenceInfo* info = p->data;
                NSString* filePath = [NSString stringWithUTF8String:info->file_path];
                CEEList* list = NULL;
                BOOL shouldParsed = YES;
                BOOL fileExisted = YES;
                
                if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                    unreferences = cee_list_prepend(unreferences, cee_strdup([filePath UTF8String]));
                    fileExisted = NO;
                }
                
                if (fileExisted && self->_sync && info->last_parsed_time) {
                    NSDate* lastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:filePath error:nil] fileModificationDate];
                    NSDate* lastParsedDate = [dateFormatter dateFromString:[NSString stringWithUTF8String:info->last_parsed_time]];
                    if ([lastModifiedDate compare:lastParsedDate] == NSOrderedSame ||
                        [lastModifiedDate compare:lastParsedDate] == NSOrderedAscending)
                        shouldParsed = NO;
                }
                
                if (!fileExisted)
                    shouldParsed = NO;
                
                if (shouldParsed) {
                    __block CEESourceBuffer* buffer = nil;
                    // open source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        buffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
                    });
                    
                    if (self->_sync && info->symbol_count)
                        cee_database_symbols_delete_by_file_path(self->_project.database,
                                                                 [buffer.filePath UTF8String]);
                    [buffer parse];
                    
                    cee_source_fragment_symbol_tree_dump_to_list(buffer.prep_directive_symbol_tree, &list);
                    cee_source_fragment_symbol_tree_dump_to_list(buffer.statement_symbol_tree, &list);
                    list = cee_source_fragment_symbol_list_type_filter(list, filter_types);
                    
                    if (list) {
                       //NSLog(@"%@ parsed", [buffer.filePath lastPathComponent]);
                        // update last parsed time to buffer
                        cee_char* time_str = cee_time_to_iso8601(cee_time_current());
                        if (time_str) {
                            cee_database_file_reference_last_parsed_time_set(self->_project.database,
                                                                             [buffer.filePath UTF8String],
                                                                             time_str);
                            cee_free(time_str);
                        }
                        cee_database_symbols_write(self->_project.database, list);
                        cee_database_file_reference_symbols_count_set(self->_project.database,
                                                                      [filePath UTF8String],
                                                                      cee_list_length(list));
                        cee_list_free(list);
                    }
                    
                    // close source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        [delegate.sourceBufferManager closeSourceBuffer:buffer];
                    });
                }
                
                p = p->next;
                i ++;
                
                dispatch_sync(dispatch_get_main_queue(), ^{
                    @autoreleasepool {
                        NSString* operateString = @"";
                        if (self->_sync)
                            operateString = @"Syncing";
                        else
                            operateString = @"Parsing";
                        
                        NSString* labelString = [NSString stringWithFormat:@"%@ \"%@\"", operateString, [filePath lastPathComponent]];
                        [self->_label1 setStringValue:labelString];
                        [self->_progressBar setDoubleValue:(double)i];
                    } // autoreleasepool
                });
                
            } // autoreleasepool
                        
            if (self->_cancel)
                break;
        }
        cee_list_free_full(infos, cee_file_reference_info_free);
        
        if (unreferences) {
            cee_database_file_references_remove(self->_project.database, unreferences);
            
            p = unreferences;
            while (p) {
                cee_database_symbols_delete_by_file_path(self->_project.database, p->data);
                p = p->next;
            }
            
            cee_list_free_full(unreferences, cee_free);
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self->_label1 setStringValue:@"Complete!"];
            [self->_button setTitle:@"OK"];
            [self->_button setHighlighted:YES];
        });
    });
}

- (NSDateFormatter*)createISO8601DateFormatter {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    NSLocale *enUSPOSIXLocale = [NSLocale localeWithLocaleIdentifier:@"en_US_POSIX"];
    [formatter setLocale:enUSPOSIXLocale];
    [formatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss'Z'"];
    [formatter setCalendar:[NSCalendar calendarWithIdentifier:NSCalendarIdentifierGregorian]];
    return formatter;
}

- (void)saveModifiedBuffers {
    AppDelegate* delegate = [NSApp delegate];
    NSMutableArray* buffers = nil;
    for (CEESession* session in self->_project.sessions) {
        for (CEESessionPort* port in session.ports) {
            for (CEESourceBuffer* buffer in port.openedSourceBuffers) {
                if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile] &&
                    ![buffer stateSet:kCEESourceBufferStateFileTemporary] &&
                    ![buffer stateSet:kCEESourceBufferStateFileDeleted]) {
                    
                    if (!buffers)
                        buffers = [[NSMutableArray alloc] init];
                    
                    if (![buffers containsObject:buffer])
                        [buffers addObject:buffer];
                }
            }
        }
    }
    
    for (CEESourceBuffer* buffer in buffers)
        [delegate.sourceBufferManager saveSourceBuffer:buffer atFilePath:buffer.filePath];
}

- (IBAction)cancel:(id)sender {
    _cancel = YES;
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
