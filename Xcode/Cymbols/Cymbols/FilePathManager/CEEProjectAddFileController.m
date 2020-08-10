//
//  CEEProjectAddFileController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEEStyleManager.h"
#import "CEEProjectAddFileController.h"
#import "CEEAddFileSelectionView.h"
#import "CEEFilePathConfirmView.h"
#import "CEEButton.h"
#import "CEEFileNameCellView.h"
#import "CEEFilePathCellView.h"

typedef NS_ENUM(NSInteger, CEEProjectAddFileScene) {
    kCEEFileSelection,
    kCEEFileConfirm,
};


@interface CEEProjectAddFileController ()
@property CEEProjectAddFileScene scene;
@property (weak) CEEView* currentView;
@property (strong) IBOutlet CEEButton *button0;
@property (strong) IBOutlet CEEButton *button1;
@property (strong) CEEAddFileSelectionView* addFileSelectionView;
@property (strong) CEEFilePathConfirmView* filePathConfirmView;
@property (strong) NSMutableArray* addFilePaths;
@property (strong) NSArray* filePathsExpand;
@end

@implementation CEEProjectAddFileController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self setViewStyle:kCEEViewStyleActived];
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    _addFileSelectionView = (CEEAddFileSelectionView*)[delegate nibObjectWithClass:[CEEAddFileSelectionView class] fromNibNamed:@"AddFileSelectionView"];
    [_addFileSelectionView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _filePathConfirmView = (CEEFilePathConfirmView*)[delegate nibObjectWithClass:[CEEFilePathConfirmView class] fromNibNamed:@"FilePathConfirmView"];
    [_filePathConfirmView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    [_addFileSelectionView.sourceTable setDelegate:self];
    [_addFileSelectionView.sourceTable setDataSource:self];
    [_addFileSelectionView.sourceTable setColumns:2];
    [_addFileSelectionView.sourceTable setTarget:self];
    [_addFileSelectionView.sourceTable registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    [_addFileSelectionView.sourceTable setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
    [_addFileSelectionView.sourceTable setAllowsMultipleSelection:YES];
    
    [_addFileSelectionView.addFilePathButton setTarget:self];
    [_addFileSelectionView.addFilePathButton setAction:@selector(addFilePathToSourceTable:)];
    [_addFileSelectionView.openFilePanelButton setTarget:self];
    [_addFileSelectionView.openFilePanelButton setAction:@selector(openFilePanel:)];
    [_addFileSelectionView.removeFilePathsButton setTarget:self];
    [_addFileSelectionView.removeFilePathsButton setAction:@selector(removeFilePathsFromSourceTable:)];
    
    [_filePathConfirmView.sourceTable setDelegate:self];
    [_filePathConfirmView.sourceTable setDataSource:self];
    [_filePathConfirmView.sourceTable setColumns:2];
    [_filePathConfirmView.sourceTable setTarget:self];
    [_filePathConfirmView.label setStringValue:@"The following Files is adding to Project"];
    _addFilePaths = [[NSMutableArray alloc] init];
}

- (void)viewWillAppear {
    _scene = kCEEFileSelection;
    [self createScene:_scene];
    [self setViewStyle:kCEEViewStyleActived];
}

- (void)createScene:(CEEProjectAddFileScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kCEEFileSelection) {
        [_addFileSelectionView.sourceTable reloadData];
        _currentView = _addFileSelectionView;
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kCEEFileConfirm) {
        _filePathsExpand = ExpandFilePaths(_addFilePaths);
        [_filePathConfirmView.sourceTable reloadData];
        _currentView = _filePathConfirmView;
        _button0.title = @"Previous";
        _button1.title = @"Complete";
    }
    
    [_currentView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_currentView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_currentView setStyle:kCEEViewStyleActived];
    
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
    if (_scene == kCEEFileSelection) {
        [self dismissController:self];
    }
    else if (_scene == kCEEFileConfirm) {
        _scene = kCEEFileSelection;
        [self createScene:_scene];
    }
}

- (IBAction)button1Action:(id)sender {
    if (_scene == kCEEFileSelection) {
        _scene = kCEEFileConfirm;
        [self createScene:_scene];
    }
    else if (_scene == kCEEFileConfirm) {
        [self.session.project addFilePaths:_filePathsExpand];
        [self dismissController:self];
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (tableView == _addFileSelectionView.sourceTable)
        return _addFilePaths.count;
    else if (tableView == _filePathConfirmView.sourceTable)
        return _filePathsExpand.count;
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    if (tableView == _addFileSelectionView.sourceTable) {
        if (column == 0) {
            CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
            cellView.title.stringValue = [_addFilePaths[row] lastPathComponent];
            return cellView;
        }
        else if (column == 1) {
            CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
            cellView.title.stringValue = _addFilePaths[row];
            return cellView;
        }
    }
    else if (tableView == _filePathConfirmView.sourceTable) {
        if (column == 0) {
            CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
            cellView.title.stringValue = [_filePathsExpand[row] lastPathComponent];
            return cellView;
        }
        else if (column == 1) {
            CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
            cellView.title.stringValue = _filePathsExpand[row];
            return cellView;
        }
    }
    return nil;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (tableView == _addFileSelectionView.sourceTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    else if (tableView == _filePathConfirmView.sourceTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    return nil;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    CymbolsDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (IBAction)addFilePathToSourceTable:(id)sender {
    if ([_addFileSelectionView.filePathInput.stringValue isEqualToString:@""])
        return;
    
    NSString* filePath = _addFileSelectionView.filePathInput.stringValue;
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        [_addFilePaths addObject:filePath];
        [_addFileSelectionView.sourceTable reloadData];
    }
}

-(IBAction)removeFilePathsFromSourceTable:(id)sender {
    if (!_addFilePaths.count)
        return;
    
    NSMutableArray* removes = [[NSMutableArray alloc] init];
    NSUInteger i = [_addFileSelectionView.sourceTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [removes addObject:_addFilePaths[i]];
        i = [_addFileSelectionView.sourceTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    
    for (NSString* filePath in removes)
        [_addFilePaths removeObject:filePath];
    
    [_addFileSelectionView.sourceTable reloadData];
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
    [self->_addFileSelectionView.sourceTable reloadData];
}

- (NSDragOperation)tableView:(CEETableView *)tableView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation {
    if (tableView == _addFileSelectionView.sourceTable)
        return NSDragOperationGeneric;
    return NSDragOperationNone;
}

- (BOOL)tableView:(CEETableView *)tableView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation {
    
    if (tableView == _addFileSelectionView.sourceTable) {
        NSPasteboard *pasteboard = info.draggingPasteboard;
        NSArray *paths = [pasteboard propertyListForType:NSFilenamesPboardType];
        
        for (NSString* path in paths)
            [_addFilePaths addObject:path];
        
        [_addFileSelectionView.sourceTable reloadData];
        
        return YES;
    }
    
    return NO;
}

@end
