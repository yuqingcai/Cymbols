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

- (BOOL)project:(CEEProject*)project securitySaveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath {
    if (!project || !buffer || !filePath)
        return FALSE;
    
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    NSArray* bookmarks = nil;
    BOOL ret = NO;
    if (access([filePath UTF8String], W_OK) != 0) {
        bookmarks = [project getSecurityBookmarksWithFilePaths:@[filePath]];
        if (bookmarks) {
            [project startAccessSecurityScopedResourcesWithBookmarks:bookmarks];
            ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
            [project stopAccessSecurityScopedResourcesWithBookmarks:bookmarks];
        }
    }
    else {
        ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:filePath];
    }
    return ret;
}

- (CEESourceBuffer*)project:(CEEProject*)project securityOpenSourceBufferWithFilePath:(NSString*)filePath {
    if (!project || !filePath)
        return nil;
    
    CEESourceBuffer* buffer = nil;
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    if (access([filePath UTF8String], R_OK) != 0) {
        NSArray* bookmarks = [project getSecurityBookmarksWithFilePaths:@[filePath]];
        if (bookmarks) {
            [project startAccessSecurityScopedResourcesWithBookmarks:bookmarks];
            buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath andOption:kCEESourceBufferOpenOptionIndependent];
            [project stopAccessSecurityScopedResourcesWithBookmarks:bookmarks];
        }
    }
    else {
        buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath andOption:kCEESourceBufferOpenOptionIndependent];
    }
    
    return buffer;
}

- (void)project:(CEEProject*)project securityCloseSourceBuffer:(CEESourceBuffer*)buffer {
    if (!project || !buffer)
        return;
    
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    [sourceBufferManager closeSourceBuffer:buffer];
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
    
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    
    _cancel = NO;
    
    __block NSDateFormatter *dateFormatter = [self createISO8601DateFormatter];
    
    [self saveModifiedBuffers];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        CEEList* infos = cee_database_filepath_entry_infos_get(self->_project.database);
        
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
        while (p) {
            
            @autoreleasepool {
                
                CEEProjectFilePathEntryInfo* info = p->data;
                NSString* filePath = [NSString stringWithUTF8String:info->file_path];
                CEEList* list = NULL;
                BOOL shouldParsed = YES;
                
                if (self->_sync && info->last_parsed_time) {
                    NSDate* lastModifiedDate = [[[NSFileManager defaultManager] attributesOfItemAtPath:filePath error:nil] fileModificationDate];
                    NSDate* lastParsedDate = [dateFormatter dateFromString:[NSString stringWithUTF8String:info->last_parsed_time]];
                    if ([lastModifiedDate compare:lastParsedDate] == NSOrderedSame ||
                        [lastModifiedDate compare:lastParsedDate] == NSOrderedAscending)
                        shouldParsed = NO;
                }
                
                if (shouldParsed) {
                    __block CEESourceBuffer* buffer = nil;
                    // open source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        buffer = [self project:self->_project securityOpenSourceBufferWithFilePath:filePath];
                    });
                    
                    if (self->_sync && info->symbol_count)
                        cee_database_symbols_delete_by_filepath(self->_project.database,
                                                                [buffer.filePath UTF8String]);
                    
                    cee_source_buffer_parse(buffer, 0);
                    
                    cee_source_fregment_symbol_tree_dump_to_list(buffer.prep_directive_symbol_tree, &list);
                    cee_source_fregment_symbol_tree_dump_to_list(buffer.statement_symbol_tree, &list);
                    
                    if (list) {
                        //NSLog(@"%@ parsed", [buffer.filePath lastPathComponent]);
                        // update last parsed time to buffer
                        cee_char* time_str = cee_time_to_iso8601(cee_time_current());
                        if (time_str) {
                            cee_database_filepath_last_parsed_time_set(self->_project.database,
                                                                       [buffer.filePath UTF8String],
                                                                       time_str);
                            cee_free(time_str);
                        }
                        cee_database_symbols_write(self->_project.database, list);
                        cee_database_filepath_symbols_count_set(self->_project.database,
                                                                [filePath UTF8String],
                                                                cee_list_length(list));
                        cee_list_free(list);
                    }
                    
                    // close source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        [self project:self->_project securityCloseSourceBuffer:buffer];
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
        
        cee_list_free_full(infos, cee_project_file_path_entry_info_free);
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self->_label1 setStringValue:@"Complete!"];
            [self->_button setTitle:@"OK"];
            [self->_button setHighlighted:YES];
        });
    });
}

/*
- (void)viewDidAppear {
    [super viewDidAppear];
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    CEEList* infos = cee_database_filepath_entry_infos_get(self->_project.database);
    CEEList* p = infos;
    while (p) {

        int bst_0 = cee_bst_count_get();
        int list_0 = cee_list_count_get();
        int symbol_0 = cee_source_symbol_count_get();
        int fregment_0 = cee_source_fregment_count_get();
        int token_0 = cee_token_count_get();
        
        CEEProjectFilePathEntryInfo* info = p->data;
        NSString* filePath = [NSString stringWithUTF8String:info->file_path];
        @autoreleasepool {
            //for (int i = 0; i < 2000; i ++) {
                NSLog(@"%@", filePath);
                CEESourceBuffer* buffer = [self project:self->_project securityOpenSourceBufferWithFilePath:filePath];
                cee_source_buffer_parse(buffer, 0);
                [self project:self->_project securityCloseSourceBuffer:buffer];
            //}
        } // autoreleasepool
        
        p = p->next;

        int bst_1 = cee_bst_count_get();
        int list_1 = cee_list_count_get();
        int symbol_1 = cee_source_symbol_count_get();
        int fregment_1 = cee_source_fregment_count_get();
        int token_1 = cee_token_count_get();

        if (bst_0 != bst_1 ||
            list_0 != list_1 ||
            symbol_0 != symbol_1 ||
            fregment_0 != fregment_1 ||
            token_0 != token_1) {
            fprintf(stdout, "oops!!\n");
        }
    }

    cee_list_free_full(infos, cee_project_file_path_entry_info_free);

    [_label1 setStringValue:@"Complete!"];
    [_button setTitle:@"OK"];
    [_button setHighlighted:YES];
}
*/
- (NSDateFormatter*)createISO8601DateFormatter {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    NSLocale *enUSPOSIXLocale = [NSLocale localeWithLocaleIdentifier:@"en_US_POSIX"];
    [formatter setLocale:enUSPOSIXLocale];
    [formatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss'Z'"];
    [formatter setCalendar:[NSCalendar calendarWithIdentifier:NSCalendarIdentifierGregorian]];
    return formatter;
}

- (void)saveModifiedBuffers {
    NSMutableArray* buffers = nil;
    for (CEESession* session in self->_project.sessions) {
        for (CEESessionPort* port in session.ports) {
            for (CEESourceBuffer* buffer in port.openedSourceBuffers) {
                if ([buffer stateSet:kCEESourceBufferStateModified] &&
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
        [self project:_project securitySaveSourceBuffer:buffer atFilePath:buffer.filePath];
}

- (IBAction)cancel:(id)sender {
    _cancel = YES;
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
