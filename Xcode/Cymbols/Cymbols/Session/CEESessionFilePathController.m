//
//  CEESessionFilePathController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEESessionFilePathController.h"
#import "CEEFileNameCellView.h"
#import "CEETitlebarButton.h"
#import "CEETitlebarStateButton.h"

@interface CEESessionFilePathController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) CEETableView *sourceTable;
@property (strong) CEETreeView *sourceTree;
@property (strong) IBOutlet NSMenu *sourceTableMenu;
@property (strong) NSArray* filePathsInProject;
@property (strong) NSString* filterCondition;
@property (weak) IBOutlet CEETextField *filterInput;
@property (weak) IBOutlet CEETitlebarButton *addSourceButton;
@property (weak) IBOutlet CEETitlebarButton *removeSourceButton;
@property (weak) IBOutlet CEETitlebarStateButton *listButton;
@property (weak) IBOutlet CEETitlebarStateButton *treeButton;
@property (weak) IBOutlet CEETitlebarButton *reloadButton;

@property (weak) IBOutlet CEEView *contentView;
@end

@implementation CEESessionFilePathController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [_titlebar setTitle:@"Untitled"];
    _sourceTable = [[CEETableView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_sourceTable setNibNameOfCellView:@"TableCellViews"];
    [_sourceTable setIdentifier:@"IDSessionFilePathTableView"];
    [_sourceTable setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceTable setDataSource:self];
    [_sourceTable setDelegate:self];
    [_sourceTable setNumberOfColumns:1];
    [_sourceTable setTarget:self];
    [_sourceTable setDoubleAction:@selector(openFilesFromSourceTable:)];
    [_sourceTable setAllowsMultipleSelection:YES];
    [_sourceTable setEnableDrawHeader:NO];
    [_sourceTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_sourceTable registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [_sourceTable setMenu:_sourceTableMenu];
    [_sourceTable setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_sourceTable reloadData];
    
    _sourceTree = [[CEETreeView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_sourceTree setNibNameOfCellView:@"TableCellViews"];
    [_sourceTree setIdentifier:@"IDSessionFilePathTreeView"];
    [_sourceTree setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceTree setDataSource:self];
    [_sourceTree setDelegate:self];
    [_sourceTree setNumberOfColumns:1];
    [_sourceTree setTarget:self];
    [_sourceTree setDoubleAction:@selector(openFilesFromSourceTree:)];
    [_sourceTree setAllowsMultipleSelection:YES];
    [_sourceTree setEnableDrawHeader:NO];
    [_sourceTree setMenu:_sourceTableMenu];
    [_sourceTree setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_sourceTree registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [_sourceTree setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_sourceTree reloadData];
    
    [_filterInput setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(addFilePathsResponse:) name:CEENotificationProjectAddFilePaths object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(removeFilePathsResponse:) name:CEENotificationProjectRemoveFilePaths object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
}
- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

#pragma mark - table view delegate

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_session)
        return 0;
    return _filePathsInProject.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FileName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = _filePathsInProject[row];
    NSString* string = [filePath lastPathComponent];
    CEEFileNameCellView *cellView = [_sourceTable makeViewWithIdentifier:@"IDFileNameCellView"];
    BOOL fileExisted = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
    if (!fileExisted) 
        string = [string stringByAppendingString:@" (delete)"];
    cellView.title.stringValue = string;
    [cellView.icon setImage:[styleManager filetypeIconFromFileName:[filePath lastPathComponent]]];
    return cellView;
}

#pragma mark - table view drag & drop

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

#pragma mark - table view data source

- (void)tableView:(NSTableView *)tableView draggingSession:(NSDraggingSession *)session willBeginAtPoint:(NSPoint)screenPoint forRowIndexes:(NSIndexSet *)rowIndexes {
 session.animatesToStartingPositionsOnCancelOrFail = YES;
 }
 
- (void)tableView:(NSTableView *)tableView updateDraggingItemsForDrag:(id<NSDraggingInfo>)draggingInfo {
 }
 
- (void)tableView:(NSTableView *)tableView draggingSession:(NSDraggingSession *)session endedAtPoint:(NSPoint)screenPoint operation:(NSDragOperation)operation {
 }


#pragma mark - tree view drag & drop

- (BOOL)treeView:(CEETreeView *)treeView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pasteboard {
    //NSMutableArray *filePaths = [[NSMutableArray alloc] init];
    //NSInteger i = [treeView.selectedRowIndexes firstIndex];
    //BOOL ret = NO;
    //while (i != NSNotFound) {
    //    if ([[NSFileManager defaultManager] fileExistsAtPath:_filePathsInProject[i] isDirectory:&ret]) {
    //        if (!ret)
    //            [filePaths addObject:_filePathsInProject[i]];
    //    }
    //    i = [treeView.selectedRowIndexes indexGreaterThanIndex:i];
    //}
    //[pasteboard setPropertyList:filePaths forType:NSFilenamesPboardType];
    //return YES;
    return NO;
}

- (NSDragOperation)treeView:(CEETreeView *)treeView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation {
    return NSDragOperationGeneric;
}

- (BOOL)treeView:(CEETreeView *)treeView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation
{
    //NSPasteboard* pasteboard = [info draggingPasteboard];
    //if ([[pasteboard types] containsObject:NSFilenamesPboardType]) {
    //    NSArray* filePaths = [pasteboard propertyListForType:NSFilenamesPboardType];
    //    [_session.project addFilePaths:filePaths];
    //    return YES;
    //}
    //return NO;
    NSPasteboard* pasteboard = [info draggingPasteboard];
    NSArray* filePaths = [pasteboard propertyListForType:NSFilenamesPboardType];
    for (NSString* filePath in filePaths)
        NSLog(@"%@", filePath);
    return YES;
}

#pragma mark - tree view data source

- (NSInteger)treeView:(CEETreeView *)treeView numberOfChildrenOfItem:(id)item {
    if (!item)
        return _session.project.properties.filePathsUserSelected.count;
    
    NSString* filePath = (NSString*)item;
    BOOL ret = NO;
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&ret] && ret)
        return [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:filePath error:NULL] count];
    
    return 0;
}

- (id)treeView:(CEETreeView *)treeView child:(NSInteger)index ofItem:(id)item {
    if (!item)
        return _session.project.properties.filePathsUserSelected[index];
    
    NSString* filePath = (NSString*)item;
    NSString* subFilePath = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:filePath error:NULL][index];
    
    return [filePath stringByAppendingPathComponent:subFilePath];
}

- (BOOL)treeView:(CEETreeView *)treeView isItemExpandable:(id)item {
    NSString* filePath = (NSString*)item;
    BOOL ret = NO;
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&ret] && ret)
        return YES;
    return NO;
}

- (NSString *)treeView:(CEETreeView *)treeView titleForColumn:(NSInteger)column {
    return @"?";
}

- (CEEView*)treeView:(CEETreeView *)treeView viewForColumn:(NSInteger)column item:(id)item {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = (NSString*)item;
    NSString* string = [filePath lastPathComponent];
    CEEFileNameCellView *cellView = [treeView makeViewWithIdentifier:@"IDFileNameCellView"];
    BOOL fileExisted = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
    if (!fileExisted)
        string = [string stringByAppendingString:@" (delete)"];
    
    cellView.title.stringValue = string;
    BOOL isDirectory = NO;
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory]) {
        if (!isDirectory)
            [cellView.icon setImage:[styleManager filetypeIconFromFileName:[filePath lastPathComponent]]];
        else
            [cellView.icon setImage:[styleManager iconFromName:@"icon_directory_16x16"]];
    }
    return cellView;
}

- (BOOL)treeView:(CEETreeView *)treeView shouldExpandItem:(nullable id)item {
    return YES;
}

- (BOOL)treeView:(CEETreeView *)treeView shouldCollapseItem:(nullable id)item {
    return YES;
}

- (IBAction)openFilesFromSourceTable:(id)sender {
    if (!_sourceTable.selectedRowIndexes)
        return;
    
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    NSUInteger i = [_sourceTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [filePaths addObject:_filePathsInProject[i]];
        i = [_sourceTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    
    if (!_session.activedPort)
        [_session setActivedPort:[_session createPort]];
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

- (IBAction)openFilesFromSourceTree:(id)sender {
    NSArray* items = [_sourceTree selectedItems];
    if (!items)
        return;
    
    for (id item in items) {
        NSString* filePath = (NSString*)item;
        BOOL isDirectory = NO;
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory]) {
            if (!isDirectory) {
                if (!_session.activedPort)
                    [_session setActivedPort:[_session createPort]];
                [_session.activedPort openSourceBuffersWithFilePaths:@[filePath]];
            }
            else {
                if (![_sourceTree itemIsExpanded:item])
                    [_sourceTree expandItem:item];
                else
                    [_sourceTree collapseItem:item];
            }
        }
    }
}

- (void)deserialize:(NSDictionary *)dict {
    _filePathsInProject = [_session filePathsFilter:_filterCondition];
    if (_sourceTable.superview)
        [_sourceTable reloadData];
    else if (_sourceTree.superview)
        [_sourceTree reloadData];
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

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {    
    //[_sourceTable reloadData];
    //[self highlightSelectionInReferenceTable];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    
    BOOL hidden = YES;
    if (_session.project.database)
        hidden = NO;
    
    [_addSourceButton setHidden:hidden];
    [_removeSourceButton setHidden:hidden];
    [_listButton setHidden:hidden];
    [_treeButton setHidden:hidden];
    [_listButton setIsExclusive:YES];
    [_treeButton setIsExclusive:YES];
    [_reloadButton setHidden:hidden];
    
    [self presentPaths];
    [self presentTitle];
    [self setTreeViewMode];
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    
    BOOL hidden = YES;
    if (_session.project.database)
        hidden = NO;
    
    [_addSourceButton setHidden:hidden];
    [_removeSourceButton setHidden:hidden];
    [_listButton setHidden:hidden];
    [_treeButton setHidden:hidden];
    [_listButton setIsExclusive:YES];
    [_treeButton setIsExclusive:YES];
    [_reloadButton setHidden:hidden];
    
    [self presentPaths];
    [self presentTitle];
    [self setTreeViewMode];
}

- (void)presentTitle {
    if (_session.project.properties.name)
        [_titlebar setTitle:[_session.project.properties.name localizedCapitalizedString]];
    else
        [_titlebar setTitle:@"Untitled"];
}

- (void)presentPaths {
    _filePathsInProject = [_session filePathsFilter:_filterCondition];
    if (_sourceTable.superview)
        [_sourceTable reloadData];
    else if (_sourceTree.superview)
        [_sourceTree reloadData];
}

- (void)textViewTextChanged:(CEETextView *)textView {
    if (textView == _filterInput) {
        _filterCondition = [_filterInput.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        [self presentPaths];
    }
}

- (IBAction)toggleTreeView:(id)sender {
    [_listButton setState:NSControlStateValueOff];
    [self showSourceTree];
}

- (IBAction)toggleListView:(id)sender {
    [_treeButton setState:NSControlStateValueOff];
    [self showSourceTable];
}

- (void)setTreeViewMode {
    [_treeButton setState:NSControlStateValueOn];
    [self toggleTreeView:nil];
}

- (void)removeView:(CEEView*)view {
    if (view != _sourceTree && view != _sourceTable)
        return;
       
    // remove sourceTree from view hierarchies
    if (view.superview) {
        NSMutableArray* constraints = nil;
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in view.superview.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == view ||
                constraint.secondItem == view) {
                [view.superview removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _titlebar.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == view ||
                constraint.secondItem == view) {
                [_titlebar removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _filterInput.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == view ||
                constraint.secondItem == view) {
                [_filterInput removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in view.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == view.superview ||
                constraint.secondItem == view.superview ||
                constraint.firstItem == _titlebar ||
                constraint.secondItem == _titlebar ||
                constraint.firstItem == _filterInput ||
                constraint.secondItem == _filterInput ) {
                [view removeConstraint:constraint];
            }
        }
        [view removeFromSuperview];
    }
}

- (void)showSourceTable {
    if (_sourceTable.superview)
        return;
    
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"filterInput" : _filterInput,
        @"sourceTable" : _sourceTable,
    };
    
    // remove sourceTree from view hierarchies
    [self removeView:_sourceTree];
    
    // add sourceTable to view hierarchies
    [self.view addSubview:_sourceTable];
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[sourceTable]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[sourceTable]-0-[filterInput]-3-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [_sourceTable reloadData];
}

- (void)showSourceTree {
    if (_sourceTree.superview)
        return;
    
    NSDictionary *views = @{
      @"titlebar" : _titlebar,
      @"filterInput" : _filterInput,
      @"sourceTree" : _sourceTree,
    };
    
    // remove sourceTree from view hierarchies
    [self removeView:_sourceTable];
    
    // add sourceTree to view hierarchies
    [self.view addSubview:_sourceTree];
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[sourceTree]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[sourceTree]-0-[filterInput]-3-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [_sourceTree reloadData];
}

- (IBAction)reload:(id)sender {
    if (_sourceTree.superview)
        [_sourceTree reloadData];
    
    if (_sourceTable.superview)
        [_sourceTable reloadData];
}

@end
