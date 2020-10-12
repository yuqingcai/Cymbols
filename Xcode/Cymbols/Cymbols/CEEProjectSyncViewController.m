//
//  CEEProjectSyncViewController.m
//  Cymbols
//
//  Created by qing on 2020/9/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEProjectSyncViewController.h"
#import "CEEFileNameCellView.h"
#import "CEEFilePathCellView.h"
#import "CEETextLabel.h"
#import "CEEButton.h"

@interface CEEProjectSyncViewController ()
@property (strong) CEEProject* project;
@property (weak) IBOutlet CEETableView *bufferTable;
@property (strong) NSMutableArray* bufferToBeSynced;
@property (weak) IBOutlet NSProgressIndicator *progressBar;
@property (weak) IBOutlet CEETextLabel *label;
@property (weak) IBOutlet CEEButton *button;
@property BOOL cancel;

@end

@implementation CEEProjectSyncViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_bufferTable setColumns:2];
    [_bufferTable setDataSource:self];
    [_bufferTable setDelegate:self];
    [_bufferTable setEnableDrawHeader:YES];
}

- (BOOL)project:(CEEProject*)project SecuritySaveSourceBuffer:(CEESourceBuffer*)buffer atFilePath:(NSString*)filePath {
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

- (void)viewDidAppear {
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    _project = [projectController currentDocument];
    _cancel = NO;
    _bufferToBeSynced = nil;
    
    for (CEESession* session in _project.sessions) {
        for (CEESessionPort* port in session.ports) {
            for (CEESourceBuffer* buffer in port.openedSourceBuffers) {
                if ([buffer stateSet:kCEESourceBufferStateModified] && 
                    ![buffer stateSet:kCEESourceBufferStateFileTemporary] && 
                    ![buffer stateSet:kCEESourceBufferStateFileDeleted]) {
                    if (!_bufferToBeSynced)
                        _bufferToBeSynced = [[NSMutableArray alloc] init];
                    
                    if (![_bufferToBeSynced containsObject:buffer])
                        [_bufferToBeSynced addObject:buffer];
                }
            }
        }
    }
        
    if (!_bufferToBeSynced) {
        [_label setStringValue:@"No Sources should be Synced."];
        [_button setTitle:@"Complete"];
        [_progressBar setMinValue:0.0];
        [_progressBar setMaxValue:1.0];
        [_progressBar setDoubleValue:1.0];
        return;
    }

    [_progressBar setMinValue:0.0];
    [_progressBar setMaxValue:_bufferToBeSynced.count];
    [_progressBar setDoubleValue:0.0];
    [_bufferTable reloadData];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        for (int i = 0; i < self->_bufferToBeSynced.count; i ++) {
            __block CEESourceBuffer* buffer = self->_bufferToBeSynced[i];
            [self->_project syncSourceSymbols:buffer];
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self project:self->_project SecuritySaveSourceBuffer:buffer atFilePath:buffer.filePath];                
                [self->_label setStringValue:[buffer.filePath lastPathComponent]];
                [self->_progressBar setDoubleValue:(double)i+1];
            });
        
            if (self->_cancel)
                break;
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            self->_bufferToBeSynced = nil;
            [self->_button setTitle:@"Complete"];
        });
    });
}

- (IBAction)cancle:(id)sender {
    _cancel = YES;
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_bufferToBeSynced)
        return 0;
    return _bufferToBeSynced.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (column == 0) 
        return @"Name";
    else if (column == 1) 
        return @"Path";
    return @"";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEESourceBuffer* buffer = _bufferToBeSynced[row];
    NSString* filePath = buffer.filePath;
    NSString* fileName = [filePath lastPathComponent];
    
    if (column == 0) {
        CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
        cellView.title.stringValue = fileName;
        if ([buffer stateSet:kCEESourceBufferStateModified])
            cellView.title.stringValue = [fileName stringByAppendingString:@" *"];
        
        [cellView.icon setImage:[styleManager filetypeIconFromFileName:fileName]];
        return cellView;
    }
    else if (column == 1) {
        CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
        cellView.title.stringValue = filePath;
        return cellView;
    }
    
    return nil;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}
@end
