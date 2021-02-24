//
//  CEEAddReferenceViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProject.h"
#import "CEEStyleManager.h"
#import "CEEAddReferenceViewController.h"
#import "CEEAddReferenceView.h"
#import "CEEFilePathView.h"
#import "CEEButton.h"
#import "CEEImageTextTableCellView.h"
#import "CEETextTableCellView.h"

typedef NS_ENUM(NSInteger, AddReferenceScene) {
    kAddReferenceSceneSelection,
    kAddReferenceSceneConfirm,
};

@interface CEEAddReferenceViewController ()
@property AddReferenceScene scene;
@property (weak) CEEView* currentView;
@property (strong) IBOutlet CEEButton *button0;
@property (strong) IBOutlet CEEButton *button1;
@property (strong) CEEAddReferenceView* addReferenceView;
@property (strong) CEEFilePathView* filePathView;
@property (strong) NSMutableArray* addFilePaths;
@property (strong) NSArray* filePathsExpand;
@property (weak) CEEProject* project;
@end

@implementation CEEAddReferenceViewController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self.view setStyleState:kCEEViewStyleStateActived];
    
    AppDelegate* delegate = [NSApp delegate];
    _addReferenceView = (CEEAddReferenceView*)[delegate nibObjectWithClass:[CEEAddReferenceView class] fromNibNamed:@"AddReferenceView"];
    [_addReferenceView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _filePathView = (CEEFilePathView*)[delegate nibObjectWithClass:[CEEFilePathView class] fromNibNamed:@"FilePathView"];
    [_filePathView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    [_addReferenceView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_addReferenceView.filePathTable setDelegate:self];
    [_addReferenceView.filePathTable setDataSource:self];
    [_addReferenceView.filePathTable setNumberOfColumns:2];
    [_addReferenceView.filePathTable setTarget:self];
    [_addReferenceView.filePathTable registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    [_addReferenceView.filePathTable setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
    [_addReferenceView.filePathTable setAllowsMultipleSelection:YES];
    [_addReferenceView.openFileButton setTarget:self];
    [_addReferenceView.openFileButton setAction:@selector(openFilePanel:)];
    [_addReferenceView.removeFileButton setTarget:self];
    [_addReferenceView.removeFileButton setAction:@selector(removeFilePathFromSourceTable:)];
    
    [_filePathView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_filePathView.filePathTable setDelegate:self];
    [_filePathView.filePathTable setDataSource:self];
    [_filePathView.filePathTable setNumberOfColumns:2];
    [_filePathView.filePathTable setTarget:self];
    [_filePathView.filePathLabel setStringValue:@"The Following Files are Adding to Project"];
    _addFilePaths = [[NSMutableArray alloc] init];
}

- (void)viewWillAppear {
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    _scene = kAddReferenceSceneSelection;
    [self createScene:_scene];
    [self setViewStyleState:kCEEViewStyleStateActived];
}

- (void)createScene:(AddReferenceScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kAddReferenceSceneSelection) {
        [_addReferenceView.filePathTable reloadData];
        _currentView = (CEEView*)_addReferenceView;
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kAddReferenceSceneConfirm) {
        _filePathsExpand = ExpandFilePaths(_addFilePaths);
        [_filePathView.filePathTable reloadData];
        _currentView = (CEEView*)_filePathView;
        _button0.title = @"Previous";
        _button1.title = @"Complete";
    }
    
    [_currentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_currentView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_currentView setStyleState:kCEEViewStyleStateActived];
    
    [self.view addSubview:_currentView];
    NSDictionary *views = @{
                            @"currentView" : _currentView,
                            @"button" : _button0
                            };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[currentView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[currentView]-45-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
}

- (IBAction)button0Action:(id)sender {
    if (_scene == kAddReferenceSceneSelection) {
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseCancel];
    }
    else if (_scene == kAddReferenceSceneConfirm) {
        _scene = kAddReferenceSceneSelection;
        [self createScene:_scene];
    }
}

- (IBAction)button1Action:(id)sender {
    if (_scene == kAddReferenceSceneSelection) {
        _scene = kAddReferenceSceneConfirm;
        [self createScene:_scene];
    }
    else if (_scene == kAddReferenceSceneConfirm) {
        if (!_project)
            return;
        
        [_project addReferenceRoots:_addFilePaths];
        
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseOK];
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (tableView == _addReferenceView.filePathTable)
        return _addFilePaths.count;
    else if (tableView == _filePathView.filePathTable)
        return _filePathsExpand.count;
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = nil;
    if (tableView == _addReferenceView.filePathTable) {
        filePath = _addFilePaths[row];
        if (column == 0) {
            CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
            cellView.text.stringValue = [filePath lastPathComponent];
            [cellView.icon setImage:[styleManager filetypeIconFromFilePath:filePath]];
            return cellView;
        }
        else if (column == 1) {
            CEETextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDTextTableCellView"];
            cellView.text.stringValue = [_project shortFilePath:filePath];
            return cellView;
        }
    }
    else if (tableView == _filePathView.filePathTable) {
        filePath = _filePathsExpand[row];
        if (column == 0) {
            CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
            cellView.text.stringValue = [filePath lastPathComponent];
            [cellView.icon setImage:[styleManager filetypeIconFromFilePath:filePath]];
            return cellView;
        }
        else if (column == 1) {
            CEETextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDTextTableCellView"];
            cellView.text.stringValue = [_project shortFilePath:filePath];
            return cellView;
        }
    }
    return nil;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (tableView == _addReferenceView.filePathTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    else if (tableView == _filePathView.filePathTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    return nil;
}

-(IBAction)removeFilePathFromSourceTable:(id)sender {
    if (!_addFilePaths.count)
        return;
    
    NSMutableArray* removes = [[NSMutableArray alloc] init];
    NSUInteger i = [_addReferenceView.filePathTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [removes addObject:_addFilePaths[i]];
        i = [_addReferenceView.filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    
    for (NSString* filePath in removes)
        [_addFilePaths removeObject:filePath];
    
    [_addReferenceView.filePathTable reloadData];
}

- (IBAction)openFilePanel:(id)send {
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    NSURL* URL = [[NSURL alloc] initFileURLWithPath:NSHomeDirectory() isDirectory:YES];
    [openPanel setDirectoryURL:URL];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setAllowsMultipleSelection:YES];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanCreateDirectories:NO];
    [openPanel setDelegate:self];
    [openPanel beginSheetModalForWindow:self.view.window completionHandler: (^(NSInteger result){
        [NSApp stopModalWithCode:result];
        if (result == NSModalResponseOK) {
            for (NSURL* URL in [openPanel URLs])
                [self->_addFilePaths addObject:[URL path]];
        }
    })];
    
    [NSApp runModalForWindow:[self.view window]];
    [self->_addReferenceView.filePathTable reloadData];
}

- (NSDragOperation)tableView:(CEETableView *)tableView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation {
    if (tableView == _addReferenceView.filePathTable)
        return NSDragOperationGeneric;
    return NSDragOperationNone;
}

- (BOOL)tableView:(CEETableView *)tableView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation {
    
    if (tableView == _addReferenceView.filePathTable) {
        NSPasteboard *pasteboard = info.draggingPasteboard;
        NSArray *paths = [pasteboard propertyListForType:NSFilenamesPboardType];
        
        for (NSString* path in paths)
            [_addFilePaths addObject:path];
        
        [_addReferenceView.filePathTable reloadData];
        
        return YES;
    }
    
    return NO;
}

@end
