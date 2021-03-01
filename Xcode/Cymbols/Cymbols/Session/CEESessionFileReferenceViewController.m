//
//  CEESessionFileReferenceViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEESessionFileReferenceViewController.h"
#import "CEEImageTextTableCellView.h"
#import "CEETitlebarButton.h"
#import "CEETitlebarStateButton.h"
#import "CEECreateFileAtPathViewController.h"
#import "CEERemoveFileViewController.h"

@interface CEESessionFileReferenceViewController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) CEETableView *filePathTable;
@property (strong) CEETreeView *filePathTree;
@property (strong) NSString* filterCondition;
@property (strong) NSArray* filePaths;
@property (weak) IBOutlet CEETextField *filterInput;
@property (strong) NSWindowController* createFileAtPathWindowController;
@property (strong) NSWindowController* removeFileWindowController;
@property (strong) IBOutlet NSMenu *filePathTreeMenu;
@property (strong) IBOutlet NSMenu *filePathTableMenu;

@end

@implementation CEESessionFileReferenceViewController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [_titlebar setTitle:@"Untitled"];
    _filePathTable = [[CEETableView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_filePathTable setIdentifier:@"IDSessionFilePathTableView"];
    [_filePathTable setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_filePathTable setDataSource:self];
    [_filePathTable setDelegate:self];
    [_filePathTable setNumberOfColumns:1];
    [_filePathTable setTarget:self];
    [_filePathTable setDoubleAction:@selector(openFilesFromFilePathTable:)];
    [_filePathTable setAllowsMultipleSelection:YES];
    [_filePathTable setEnableDrawHeader:NO];
    [_filePathTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_filePathTable registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [_filePathTable setMenu:_filePathTableMenu];
    [_filePathTable setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_filePathTable reloadData];
    
    _filePathTree = [[CEETreeView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_filePathTree setNibNameOfCellView:@"TableCellViews"];
    [_filePathTree setIdentifier:@"IDSessionFilePathTreeView"];
    [_filePathTree setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_filePathTree setDataSource:self];
    [_filePathTree setDelegate:self];
    [_filePathTree setNumberOfColumns:1];
    [_filePathTree setTarget:self];
    [_filePathTree setDoubleAction:@selector(openFilesFromFilePathTree:)];
    [_filePathTree setAllowsMultipleSelection:YES];
    [_filePathTree setEnableDrawHeader:NO];
    [_filePathTree setMenu:_filePathTreeMenu];
    [_filePathTree setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_filePathTree registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    [_filePathTree setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_filePathTree reloadData];
    [_filePathTree setAutosaveExpandedItems:YES];
    [_filterInput setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(addReferenceResponse:) name:CEENotificationProjectAddReference object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(removeReferenceResponse:) name:CEENotificationProjectRemoveReference object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(createFileResponse:) name:CEENotificationProjectCreateFile object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(removeFileResponse:) name:CEENotificationProjectRemoveFile object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
    
    [self showfilePathTree];
}

- (void)menuWillOpen:(NSMenu *)menu {
    if (menu == _filePathTreeMenu) {
        
        if (!_session.project.database) {
            for (NSMenuItem* item in menu.itemArray)
                [item setEnabled:NO];
            return;
        }
        else {
            for (NSMenuItem* item in menu.itemArray)
                [item setEnabled:YES];
        }
        
        NSArray* items = [_filePathTree selectedItems];
        if (!items)
            return;
        
        NSMenuItem* openFileitem = [menu itemWithTag:2];
        NSMenuItem* openFileInNewWindowitem = [menu itemWithTag:3];
        NSMenuItem* createNewFileitem = [menu itemWithTag:4];
        NSMenuItem* createNewFolderItem = [menu itemWithTag:5];
        NSMenuItem* removeFileItem = [menu itemWithTag:6];
                
        BOOL containRoot = NO;
        BOOL containRootFile = NO;
        BOOL containFile = NO;
        BOOL containDirectory = NO;
       
        
        for (NSString* filePath in items) {
            BOOL isDirectory = NO;
            
            if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory]) {
                if (isDirectory)
                    containDirectory = YES;
                else
                    containFile = YES;
            }
            
            if ([self.session.project filePathIsReferenceRoot:filePath]) {
                if (!containRoot)
                    containRoot = YES;
                
                if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory])
                    if (!isDirectory && !containRootFile)
                        containRootFile = YES;
            }
        }
        
        if (containRoot) {
            //[removeFileItem setEnabled:NO];
            if (containRootFile) {
                [createNewFileitem setEnabled:NO];
                [createNewFolderItem setEnabled:NO];
            }
        }
        
        if (!containFile) {
            [openFileitem setEnabled:NO];
            [openFileInNewWindowitem setEnabled:NO];
        }
    }
    else if (menu == _filePathTableMenu) {
        
        if (!_session.project.database) {
            for (NSMenuItem* item in menu.itemArray)
                [item setEnabled:NO];
            return;
        }
        else {
            for (NSMenuItem* item in menu.itemArray)
                [item setEnabled:YES];
        }
        
        
        NSMenuItem* removeFileItem = [menu itemWithTag:3];
        [removeFileItem setEnabled:YES];
        
        BOOL containRootFile = NO;
        NSInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            if ([self.session.project filePathIsReferenceRoot:_filePaths[i]]) {
                if (!containRootFile)
                    containRootFile = YES;
                break;
            }
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
        
        if (containRootFile)
            [removeFileItem setEnabled:NO];
    }
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)openFilesFromFilePathTree:(id)sender {
    NSArray* items = [_filePathTree selectedItems];
    if (!items)
        return;
    
    for (id item in items) {
        NSString* filePath = (NSString*)item;
        BOOL isDirectory = NO;
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory]) {
            if (!isDirectory) {
                if (!_session.activedPort)
                    [_session setActivedPort:[_session createPort]];
                [_session.activedPort openSourceBufferWithFilePath:filePath];
            }
            else {
                if (![_filePathTree itemIsExpanded:item])
                    [_filePathTree expandItem:item];
                else
                    [_filePathTree collapseItem:item];
            }
        }
    }
}

- (void)openFilesFromFilePathTable:(id)sender {
    if (!_filePathTable.selectedRowIndexes)
        return;
    
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    NSUInteger i = [_filePathTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [filePaths addObject:_filePaths[i]];
        i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    
    if (!_session.activedPort)
        [_session setActivedPort:[_session createPort]];
    
    for (NSString* filePath in filePaths)
        [_session.activedPort openSourceBufferWithFilePath:filePath];
}

- (void)textViewTextChanged:(CEETextView *)textView {
    if (textView == _filterInput) {
        _filterCondition = _filterInput.stringValue;
        if (!_filterCondition || [_filterCondition isEqual:@""]) {
            _filePaths = [_session filePathsFilter:nil];
            [self showfilePathTree];
            [_filePathTree reloadData];
        }
        else {
            [self showfilePathTable];
            _filterCondition = [_filterCondition stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
            _filePaths = [_session filePathsFilter:_filterCondition];
            [_filePathTable reloadData];
        }
    }
}

- (IBAction)openFilesInFinder:(id)sender {
    NSMutableArray* URLs = [[NSMutableArray alloc] init];
    if (_filePathTable.superview) {
        NSUInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            [URLs addObject:[NSURL fileURLWithPath:_filePaths[i]]];
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
    }
    else if (_filePathTree.superview) {
        NSArray* items = [_filePathTree selectedItems];
        for (id item in items) {
            if ([[NSFileManager defaultManager] fileExistsAtPath:(NSString*)item]) {
                [URLs addObject:[NSURL fileURLWithPath:(NSString*)item]];
            }
        }
    }
    
    if (URLs.count)
        [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:URLs];
}

- (IBAction)openFiles:(id)sender {
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    if (_filePathTable.superview) {
        NSUInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            [filePaths addObject:_filePaths[i]];
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
    }
    else if (_filePathTree.superview) {
        NSArray* items = [_filePathTree selectedItems];
        for (id item in items) {
            BOOL isDirectory = NO;
            if ([[NSFileManager defaultManager] fileExistsAtPath:(NSString*)item isDirectory:&isDirectory]) {
                if (!isDirectory)
                    [filePaths addObject:(NSString*)item];
            }
        }
    }
    
    if (filePaths.count) {
        if (!_session.activedPort)
            [_session setActivedPort:[_session createPort]];
        for (NSString* filePath in filePaths)
            [_session.activedPort openSourceBufferWithFilePath:filePath];
    }
}

- (IBAction)openFilesInNewSession:(id)sender {
    NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    if (_filePathTable.superview) {
        NSUInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            [filePaths addObject:_filePaths[i]];
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
    }
    else if (_filePathTree.superview) {
        NSArray* items = [_filePathTree selectedItems];
        for (id item in items) {
            BOOL isDirectory = NO;
            if ([[NSFileManager defaultManager] fileExistsAtPath:(NSString*)item isDirectory:&isDirectory]) {
                if (!isDirectory)
                    [filePaths addObject:(NSString*)item];
            }
        }
    }
    
    if (filePaths.count)
        [_session.project createSessionWithFilePaths:filePaths];
}

- (IBAction)createNewFile:(id)sender {
    if (!_filePathTree.superview)
        return;
    
    __block NSModalResponse response = NSModalResponseCancel;
    NSArray* items = [_filePathTree selectedItems];
    if (!items)
        return;
    
    if (!_createFileAtPathWindowController)
        _createFileAtPathWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDCreateFileAtPathWindowController"];
    CEECreateFileAtPathViewController* viewControler = (CEECreateFileAtPathViewController*)_createFileAtPathWindowController.contentViewController;
    viewControler.createFileType = kCEECreateFileTypeFile;
    viewControler.relativePath = items[0];
    [self.view.window beginSheet:_createFileAtPathWindowController.window completionHandler:(^(NSInteger result) {
        response = result;
        [NSApp stopModalWithCode:result];
    })];
    [NSApp runModalForWindow:self.view.window];
    
    if (response != NSModalResponseOK)
        return;
}

- (IBAction)createNewFolder:(id)sender {
    if (!_filePathTree.superview)
        return;
    
    __block NSModalResponse response = NSModalResponseCancel;
    NSArray* items = [_filePathTree selectedItems];
    if (!items)
        return;
    
    if (!_createFileAtPathWindowController)
        _createFileAtPathWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDCreateFileAtPathWindowController"];
    CEECreateFileAtPathViewController* viewControler = (CEECreateFileAtPathViewController*)_createFileAtPathWindowController.contentViewController;
    viewControler.createFileType = kCEECreateFileTypeDirectory;
    viewControler.relativePath = items[0];
    [self.view.window beginSheet:_createFileAtPathWindowController.window completionHandler:(^(NSInteger result) {
        response = result;
        [NSApp stopModalWithCode:result];
    })];
    [NSApp runModalForWindow:self.view.window];
    
    if (response != NSModalResponseOK)
        return;
}

- (IBAction)deleteFiles:(id)sender {
    __block NSModalResponse response = NSModalResponseCancel;
    NSMutableArray* removingFilePaths = nil;
    
    if (_filePathTree.superview) {
        NSArray* items = [_filePathTree selectedItems];
        if (!items)
            return;
        removingFilePaths = [NSMutableArray arrayWithArray:items];
    }
    else if (_filePathTable.superview) {
        NSInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            if (!removingFilePaths)
                removingFilePaths = [[NSMutableArray alloc] init];
            [removingFilePaths addObject:_filePaths[i]];
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
    }
    
    if (!removingFilePaths)
        return;
    
    if (!_removeFileWindowController)
        _removeFileWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDRemoveFileWindowController"];
    CEERemoveFileViewController* viewControler = (CEERemoveFileViewController*)_removeFileWindowController.contentViewController;
    viewControler.removingFilePaths = removingFilePaths;
    [self.view.window beginSheet:_removeFileWindowController.window completionHandler:(^(NSInteger result) {
        response = result;
        [NSApp stopModalWithCode:result];
    })];
    [NSApp runModalForWindow:self.view.window];
    
    if (response != NSModalResponseOK)
        return;
}

- (IBAction)addReference:(id)sender {
    NSMutableArray* filePaths = nil;
    if (_filePathTree.superview) {
        filePaths = [NSMutableArray arrayWithArray:[_filePathTree selectedItems]];
    }
    else if (_filePathTable.superview) {
        filePaths = [[NSMutableArray alloc] init];
        NSInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            [filePaths addObject:_filePaths[i]];
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
    }
    [self.session.project addReferences:filePaths];
}

- (IBAction)removeReference:(id)sender {
    NSMutableArray* roots = [[NSMutableArray alloc] init];
    NSMutableArray* references = [[NSMutableArray alloc] init];
    if (_filePathTree.superview) {
        for (NSString* filePath in [_filePathTree selectedItems]) {
            if ([self.session.project filePathIsReferenceRoot:filePath])
                [roots addObject:filePath];
            else
                [references addObject:filePath];
        }
    }
    else if (_filePathTable.superview) {
        NSInteger i = [_filePathTable.selectedRowIndexes firstIndex];
        while (i != NSNotFound) {
            if ([self.session.project filePathIsReferenceRoot:_filePaths[i]])
                [roots addObject:_filePaths[i]];
            else
                [references addObject:_filePaths[i]];
            
            i = [_filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
        }
    }
    [self.session.project removeReferenceRoots:roots];
    [self.session.project removeReferences:references];
}

#pragma mark - table view delegate

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_session)
        return 0;
    return _filePaths.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FileName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = _filePaths[row];
    NSString* string = [filePath lastPathComponent];
    CEEImageTextTableCellView *cellView = [_filePathTable makeViewWithIdentifier:@"IDImageTextTableCellView"];
    BOOL fileExisted = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
    if (!fileExisted) {
        string = [string stringByAppendingString:@" (delete)"];
        cellView.text.stringValue = string;
        [cellView.icon setImage:[styleManager iconFromName:@"icon_file_not_existed_16x16"]];
    }
    else {
        cellView.text.stringValue = string;
        [cellView.icon setImage:[styleManager filetypeIconFromFileName:[filePath lastPathComponent]]];
    }
    
    return cellView;
}


#pragma mark - table view drag & drop

- (BOOL)tableView:(CEETableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pasteboard {
    NSMutableArray *filePaths = [[NSMutableArray alloc] init];
    NSInteger i = [tableView.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [filePaths addObject:_filePaths[i]];
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
    //NSPasteboard* pasteboard = [info draggingPasteboard];
    //if ([[pasteboard types] containsObject:NSFilenamesPboardType]) {
    //    NSArray* filePaths = [pasteboard propertyListForType:NSFilenamesPboardType];
    //    [_session.project addFilePathsReferenced:filePaths];
    //    return YES;
    //}
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
    NSMutableArray* filePaths = nil;
    NSArray* items = [_filePathTree selectedItems];
    for (id item in items) {
        BOOL isDirectory = NO;
        if ([[NSFileManager defaultManager] fileExistsAtPath:item isDirectory:&isDirectory]) {
            if (!isDirectory) {
                if (!filePaths)
                    filePaths = [[NSMutableArray alloc] init];
                [filePaths addObject:item];
            }
        }
    }
    [pasteboard setPropertyList:filePaths forType:NSFilenamesPboardType];
    return YES;
}

- (NSDragOperation)treeView:(CEETreeView *)treeView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation {
    return NSDragOperationGeneric;
}

- (BOOL)treeView:(CEETreeView *)treeView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation
{
    NSPasteboard* pasteboard = [info draggingPasteboard];
    if ([[pasteboard types] containsObject:NSFilenamesPboardType]) {
        NSArray* filePaths = [pasteboard propertyListForType:NSFilenamesPboardType];
        [_session.project addReferenceRoots:filePaths];
        return YES;
    }
    return NO;
}

#pragma mark - tree view data source

- (NSInteger)treeView:(CEETreeView *)treeView numberOfChildrenOfItem:(id)item {
    if (!item)
        return [_session.project getReferenceRoots].count;
    
    NSString* filePath = (NSString*)item;
    BOOL ret = NO;
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&ret] && ret) {
        return [[[NSFileManager defaultManager] contentsOfDirectoryAtURL:[NSURL fileURLWithPath:filePath] includingPropertiesForKeys:[NSArray arrayWithObject:NSURLNameKey] options:NSDirectoryEnumerationSkipsHiddenFiles error:nil] count];
    }
    return 0;
}

- (id)treeView:(CEETreeView *)treeView child:(NSInteger)index ofItem:(id)item {
    if (!item)
        return [_session.project getReferenceRoots][index];
    NSString* filePath = (NSString*)item;
    NSArray* urls = [[NSFileManager defaultManager] contentsOfDirectoryAtURL:[NSURL fileURLWithPath:filePath] includingPropertiesForKeys:[NSArray arrayWithObject:NSURLNameKey] options:NSDirectoryEnumerationSkipsHiddenFiles error:nil];
    NSURL* url = urls[index];
    return [url path];
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
    CEEImageTextTableCellView *cellView = [treeView makeViewWithIdentifier:@"IDImageTextTableCellView"];
    BOOL isDirectory = NO;
    BOOL isReferenced = NO;
    BOOL isExisted = NO;
    
    isExisted = [[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory];
    if (!isExisted) {
        string = [string stringByAppendingString:@" (delete)"];
        cellView.text.stringValue = string;
        [cellView.icon setImage:[styleManager iconFromName:@"icon_file_not_existed_16x16"]];
    }
    else {
        if (isDirectory) {
            cellView.text.stringValue = string;
            [cellView.icon setImage:[styleManager iconFromName:@"icon_directory_16x16"]];
        }
        else {
            isReferenced = [self.session.project filePathIsReferenced:filePath];
            if (!isReferenced)
                string = [string stringByAppendingString:@" (UnReferenced)"];
            cellView.text.stringValue = string;
            [cellView.icon setImage:[styleManager filetypeIconFromFileName:[filePath lastPathComponent]]];
        }
    }
    return cellView;
}

- (BOOL)treeView:(CEETreeView *)treeView shouldExpandItem:(nullable id)item {
    return YES;
}

- (BOOL)treeView:(CEETreeView *)treeView shouldCollapseItem:(nullable id)item {
    return YES;
}

- (id)treeView:(CEETreeView *)treeView persistentObjectForItem:(id)item {
    return item;
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\": ", self.view.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    serializing = [serializing stringByAppendingString:@"\"subviews\":["];
    serializing = [serializing stringByAppendingFormat:@"{ %@ }", [_filePathTree serialize]];
    serializing = [serializing stringByAppendingFormat:@"]"];
    serializing = [serializing stringByAppendingFormat:@"}"];
        
    return serializing;
}


- (void)deserialize:(NSDictionary *)dict {
    NSArray* subviews = dict[@"subviews"];
    NSDictionary* filePathTreeDescriptor = nil;
    
    for (NSDictionary* subview in subviews) {
        if ([subview objectForKey:_filePathTree.identifier]) {
            filePathTreeDescriptor = subview[_filePathTree.identifier];
            break;
        }
    }
    
    if (filePathTreeDescriptor[@"content"]) {
        [_filePathTree deserialize:filePathTreeDescriptor[@"content"]];
        NSInteger firstRowIndex = [filePathTreeDescriptor[@"firstRowIndex"] integerValue];
        [_filePathTree setFirstRowIndex:firstRowIndex];
    }
    else {
        _filePaths = [_session filePathsFilter:_filterCondition];
        if (_filePathTable.superview)
            [_filePathTable reloadData];
        else if (_filePathTree.superview)
            [_filePathTree reloadData];
    }
}

- (void)addReferenceResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    [self presentPaths];
}

- (void)removeReferenceResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    [self presentPaths];
}

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {
    //[_filePathTable reloadData];
    //[self highlightSelectionInReferenceTable];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    
    BOOL hidden = YES;
    if (_session.project.database)
        hidden = NO;
    
    
    [self presentPaths];
    [self presentTitle];
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    if ([notification object] != self.session.project)
        return;
    
    BOOL hidden = YES;
    if (_session.project.database)
        hidden = NO;
    
    [self presentPaths];
    [self presentTitle];
}

- (void)createFileResponse:(NSNotification*)notification {
    [self presentPaths];
}

- (void)removeFileResponse:(NSNotification*)notification {
    [self presentPaths];
}

- (void)presentTitle {
    if (_session.project.name)
        [_titlebar setTitle:[_session.project.name localizedCapitalizedString]];
    else
        [_titlebar setTitle:@"Untitled"];
}

- (void)presentPaths {
    _filePaths = [_session filePathsFilter:_filterCondition];
    [_filePathTable reloadData];
    [_filePathTree reloadData];
}

- (void)removeView:(CEEView*)view {
    if (view != _filePathTree && view != _filePathTable)
        return;
    
    // remove filePathTree from view hierarchies
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

- (void)showfilePathTable {
    if (_filePathTable.superview)
        return;
    
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"filterInput" : _filterInput,
        @"filePathTable" : _filePathTable,
    };
    
    // remove filePathTree from view hierarchies
    if (_filePathTree.superview)
        [self removeView:_filePathTree];
    
    // add filePathTable to view hierarchies
    [self.view addSubview:_filePathTable];
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[filePathTable]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[filePathTable]-0-[filterInput]-3-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [_filePathTable reloadData];
}

- (void)showfilePathTree {
    if (_filePathTree.superview)
        return;
    
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"filterInput" : _filterInput,
        @"filePathTree" : _filePathTree,
    };
    
    // remove filePathTree from view hierarchies
    if (_filePathTable.superview)
        [self removeView:_filePathTable];
    
    // add filePathTree to view hierarchies
    [self.view addSubview:_filePathTree];
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[filePathTree]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[filePathTree]-0-[filterInput]-3-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [_filePathTree reloadData];
}

@end
