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
@property (weak) IBOutlet CEEView *contentView;

@end

@implementation CEESessionFilePathController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [_titlebar setTitle:@"Untitled"];
    _sourceTable = [[CEETableView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_sourceTable setIdentifier:@"IDSessionFilePathTableView"];
    [_sourceTable setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceTable setDataSource:self];
    [_sourceTable setDelegate:self];
    [_sourceTable setColumns:1];
    [_sourceTable reloadData];
    [_sourceTable setTarget:self];
    [_sourceTable setDoubleAction:@selector(openFiles:)];
    [_sourceTable setAllowsMultipleSelection:YES];
    [_sourceTable setEnableDrawHeader:NO];
    [_sourceTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_sourceTable registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [_sourceTable setMenu:_sourceTableMenu];
    [_sourceTable setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _sourceTree = [[CEETreeView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_sourceTree setIdentifier:@"IDSessionFilePathTreeView"];
    [_sourceTree setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceTree setColumns:1];
    [_sourceTree setAllowsMultipleSelection:NO];
    [_sourceTree setEnableDrawHeader:NO];
    [_sourceTree setColumnAutoresizingStyle:kCEETableViewNoColumnAutoresizing];
    [_sourceTree registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [_sourceTree setMenu:_sourceTableMenu];
    [_sourceTree setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
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

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
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

- (void)deserialize:(NSDictionary *)dict {
    _filePathsInProject = [_session filePathsFilter:_filterCondition];
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
    [_sourceTable reloadData];
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
}

@end
