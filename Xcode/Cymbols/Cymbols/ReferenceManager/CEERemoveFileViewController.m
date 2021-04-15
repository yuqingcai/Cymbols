//
//  CEERemoveFileViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProject.h"
#import "CEEStyleManager.h"
#import "CEERemoveFileViewController.h"
#import "CEEFilePathView.h"
#import "CEEButton.h"
#import "CEEImageTextTableCellView.h"
#import "CEETextTableCellView.h"

typedef NS_ENUM(NSInteger, DeleteFileScene) {
    kDeleteFileSceneList,
    kDeleteFileSceneConfirm,
};

@interface CEERemoveFileViewController ()
@property DeleteFileScene scene;
@property (weak) CEEView* currentView;
@property (strong) IBOutlet CEEButton *button0;
@property (strong) IBOutlet CEEButton *button1;
@property (strong) CEEFilePathView* filePathView;
@property (strong) NSArray* expandedFilePaths;
@property (weak) CEEProject* project;
@end

@implementation CEERemoveFileViewController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self.view setStyleState:kCEEViewStyleStateActived];
    
    AppDelegate* delegate = [NSApp delegate];
    
    _filePathView = (CEEFilePathView*)[delegate nibObjectWithClass:[CEEFilePathView class] fromNibNamed:@"FilePathView"];
    [_filePathView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    [_filePathView.icon setImage:[styleManager iconFromName:@"icon_caution_16x16"]];
    [_filePathView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_filePathView.filePathTable setDelegate:self];
    [_filePathView.filePathTable setDataSource:self];
    [_filePathView.filePathTable setNumberOfColumns:2];
    [_filePathView.filePathTable setTarget:self];
}

- (void)viewWillAppear {
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    _scene = kDeleteFileSceneList;
    [self createScene:_scene];
    [self setViewStyleState:kCEEViewStyleStateActived];
}

- (void)createScene:(DeleteFileScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kDeleteFileSceneList) {
        [_filePathView.filePathTable reloadData];
        _currentView = (CEEView*)_filePathView;
        _button0.title = @"Cancel";
        _button1.title = @"Next";
        _filePathView.filePathLabel.stringValue = @"The Following Files are moving to Trash";
    }
    else if (scene == kDeleteFileSceneConfirm) {
        _expandedFilePaths = ExpandFilePaths(_removingFilePaths);
        [_filePathView.filePathTable reloadData];
        _currentView = (CEEView*)_filePathView;
        _button0.title = @"Previous";
        _button1.title = @"Complete";
        _filePathView.filePathLabel.stringValue = @"The Following Files are moving to Trash";
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
    if (_scene == kDeleteFileSceneList) {
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseCancel];
    }
    else if (_scene == kDeleteFileSceneConfirm) {
        _scene = kDeleteFileSceneList;
        [self createScene:_scene];
    }
}

- (IBAction)button1Action:(id)sender {
    if (_scene == kDeleteFileSceneList) {
        _scene = kDeleteFileSceneConfirm;
        [self createScene:_scene];
    }
    else if (_scene == kDeleteFileSceneConfirm) {
        if (!_project)
            return;
        
        [self trashFiles];
        
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseOK];
    }
}

- (void)trashFiles {
    [_project removeFiles:_removingFilePaths];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (_scene == kDeleteFileSceneList && tableView == _filePathView.filePathTable)
        return _removingFilePaths.count;
    else if (_scene == kDeleteFileSceneConfirm && tableView == _filePathView.filePathTable)
        return _expandedFilePaths.count;
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = nil;
    if (_scene == kDeleteFileSceneList && tableView == _filePathView.filePathTable) {
        filePath = _removingFilePaths[row];
        if (column == 0) {
            CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
            cellView.text.stringValue = [filePath lastPathComponent];
            [cellView.icon setImage:[styleManager iconFromFilePath:filePath]];
            return cellView;
        }
        else if (column == 1) {
            CEETextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDTextTableCellView"];
            cellView.text.stringValue = [_project shortFilePath:filePath];
            return cellView;
        }
    }
    else if (_scene == kDeleteFileSceneConfirm && tableView == _filePathView.filePathTable) {
        filePath = _expandedFilePaths[row];
        if (column == 0) {
            CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
            cellView.text.stringValue = [filePath lastPathComponent];
            [cellView.icon setImage:[styleManager iconFromFilePath:filePath]];
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
    if (_scene == kDeleteFileSceneList && tableView == _filePathView.filePathTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    else if (_scene == kDeleteFileSceneConfirm && tableView == _filePathView.filePathTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    return nil;
}

@end
