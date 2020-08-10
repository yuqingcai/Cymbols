//
//  CEESessionFilePathController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEESessionFilePathController.h"
#import "CEEFileNameCellView.h"

@interface CEESessionFilePathController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) IBOutlet CEETableView *sourceTable;
@property (strong) NSArray* filePathsInProject;
@property (strong) NSString* filePathCondition;
@property (weak) IBOutlet CEETextField *filterInput;
@end

@implementation CEESessionFilePathController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_titlebar setTitle:@"Untitled"];
    [_sourceTable setDataSource:self];
    [_sourceTable setDelegate:self];
    [_sourceTable setColumns:1];
    [_sourceTable reloadData];
    [_sourceTable setTarget:self];
    [_sourceTable setDoubleAction:@selector(openFiles:)];
    [_sourceTable setAllowsMultipleSelection:YES];
    [_filterInput setDelegate:self];
    [_sourceTable registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(addFilePathsResponse:) name:CEENotificationProjectAddFilePaths object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(removeFilePathsResponse:) name:CEENotificationProjectRemoveFilePaths object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_session)
        return 0;
    return _filePathsInProject.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FileName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    NSString* filePath = _filePathsInProject[row];
    NSString* string = [filePath lastPathComponent];
    CEEFileNameCellView *cellView = [_sourceTable makeViewWithIdentifier:@"IDFileNameCellView"];
    BOOL fileExisted = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
    if (!fileExisted) 
        string = [string stringByAppendingString:@" (delete)"];
    cellView.title.stringValue = string;
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    CymbolsDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}


- (BOOL)tableView:(CEETableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pasteboard {
    NSMutableArray *filePaths = [[NSMutableArray alloc] init];
    
    NSInteger i = [tableView.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [filePaths addObject:_filePathsInProject[i]];
        i = [tableView.selectedRowIndexes indexGreaterThanIndex:i];
    }
    [pasteboard setPropertyList:filePaths forType:NSFilenamesPboardType];
    return YES;
}

- (NSDragOperation)tableView:(CEETableView *)tableView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation {
    return NSDragOperationGeneric;
}

- (BOOL)tableView:(CEETableView *)tableView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation
{
    NSPasteboard* pasteboard = [info draggingPasteboard];
    if ([[pasteboard types] containsObject:NSFilenamesPboardType]) {
        NSArray* filePaths = [pasteboard propertyListForType:NSFilenamesPboardType];
        [_session.project addFilePaths:filePaths];
        return YES;
    }
    return NO;
}

- (void)tableView:(NSTableView *)tableView draggingSession:(NSDraggingSession *)session willBeginAtPoint:(NSPoint)screenPoint forRowIndexes:(NSIndexSet *)rowIndexes {
 session.animatesToStartingPositionsOnCancelOrFail = YES;
 }
 
- (void)tableView:(NSTableView *)tableView updateDraggingItemsForDrag:(id<NSDraggingInfo>)draggingInfo {
 }
 
- (void)tableView:(NSTableView *)tableView draggingSession:(NSDraggingSession *)session endedAtPoint:(NSPoint)screenPoint operation:(NSDragOperation)operation {
 }
 

- (IBAction)openFiles:(id)sender {
    if (!_sourceTable.selectedRowIndexes)
        return;
    
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    NSUInteger i = [_sourceTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [filePaths addObject:_filePathsInProject[i]];
        i = [_sourceTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    [_session.activedPort openSourceBuffersWithFilePaths:filePaths];
}

- (IBAction)openFilesInFinder:(id)sender {
    NSMutableArray* fileURLs = [[NSMutableArray alloc] init];
    NSUInteger i = [_sourceTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [fileURLs addObject:[NSURL fileURLWithPath:_filePathsInProject[i]]];
        i = [_sourceTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];
}

- (IBAction)openFilesInNewSession:(id)sender {
    if (!_sourceTable.selectedRowIndexes)
        return;
    
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    NSUInteger i = [_sourceTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        NSString* filePath = _filePathsInProject[i];
        [filePaths addObject:filePath];
        i = [_sourceTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    [_session.project createSessionWithFilePaths:filePaths];
}

- (void)textViewTextChanged:(CEETextView *)textView {
    if (textView == _filterInput) {
        _filePathCondition = [_filterInput.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        [self presentPaths];
    }
}

- (void)deserialize:(NSDictionary *)dict {
    _filePathsInProject = [_session filePathsFilter:_filePathCondition];
    [_sourceTable reloadData];
}

- (void)addFilePathsResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    [self presentPaths];
}

- (void)removeFilePathsResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    [self presentPaths];
}

- (void)sourceBufferStateChangedResponse:(NSNotification*)notification {    
    //[_sourceTable reloadData];
    //[self highlightSelectionInReferenceTable];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    
    [self presentPaths];
    [self presentTitle];
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    
    [self presentPaths];
    [self presentTitle];
}

- (void)presentTitle {
    if (_session.project.properties.name)
        [_titlebar setTitle:[_session.project.properties.name localizedCapitalizedString]];
    else
        [_titlebar setTitle:@"Untitled"];
}

- (void)presentPaths {
    _filePathsInProject = [_session filePathsFilter:_filePathCondition];
    [_sourceTable reloadData];
}

@end
