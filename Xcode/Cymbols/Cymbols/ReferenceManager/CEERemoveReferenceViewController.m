//
//  CEERemoveReferenceViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEERemoveReferenceViewController.h"
#import "CEERemoveReferenceView.h"
#import "CEEFilePathView.h"
#import "CEEButton.h"
#import "CEEProject.h"
#import "CEEImageTextTableCellView.h"
#import "CEETextTableCellView.h"

typedef NS_ENUM(NSInteger, RemoveReferenceScene) {
    kRemoveReferenceSceneSelection,
    kRemoveReferenceSceneConfirm,
};

@interface CEERemoveReferenceViewController ()
@property RemoveReferenceScene scene;
@property (strong) CEEView* currentView;
@property (strong) IBOutlet CEEButton *button0;
@property (strong) IBOutlet CEEButton *button1;
@property (strong) CEERemoveReferenceView* removeReferenceView;
@property (strong) CEEFilePathView* filePathView;
@property (strong) NSArray* filePathsWithCondition;
@property (strong) NSArray* filePathsRemoving;
@property (strong) NSString* condition;
@property (weak) CEEProject* project;
@end

@implementation CEERemoveReferenceViewController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self.view setStyleState:kCEEViewStyleStateActived];
    
    AppDelegate* delegate = [NSApp delegate];
    _removeReferenceView = (CEERemoveReferenceView*)[delegate nibObjectWithClass:[CEERemoveReferenceView class] fromNibNamed:@"RemoveReferenceView"];
    [_removeReferenceView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _filePathView = (CEEFilePathView*)[delegate nibObjectWithClass:[CEEFilePathView class] fromNibNamed:@"FilePathView"];
    [_filePathView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _condition = nil;
    _filePathsRemoving = nil;
    _filePathsWithCondition = nil;
    
    [_removeReferenceView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_removeReferenceView.filePathTable setDelegate:self];
    [_removeReferenceView.filePathTable setDataSource:self];
    [_removeReferenceView.filePathTable setNumberOfColumns:2];
    [_removeReferenceView.filePathTable setTarget:self];
    [_removeReferenceView.filePathTable registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    [_removeReferenceView.filePathTable setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
    [_removeReferenceView.filePathTable setAllowsMultipleSelection:YES];
    [_removeReferenceView.filePathInput setDelegate:self];
    
    [_filePathView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_filePathView.filePathTable setDelegate:self];
    [_filePathView.filePathTable setDataSource:self];
    [_filePathView.filePathTable setNumberOfColumns:2];
    [_filePathView.filePathTable setTarget:self];
    [_filePathView.filePathLabel setStringValue:@"The Following Files are Removing from Project"];
}

- (void)viewWillAppear {
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    _scene = kRemoveReferenceSceneSelection;
    [self createScene:_scene];
    [self setViewStyleState:kCEEViewStyleStateActived];
}

- (void)createScene:(RemoveReferenceScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kRemoveReferenceSceneSelection) {
        if (!_project)
            return;
        _filePathsWithCondition = [_project getReferenceFilePathsWithCondition:_condition];
        _currentView = (CEEView*)_removeReferenceView;
        [_removeReferenceView.filePathTable reloadData];
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kRemoveReferenceSceneConfirm) {
        _currentView = (CEEView*)_filePathView;
        _filePathsRemoving = [self filePathsRemovingFromSelection];
        [_filePathView.filePathTable reloadData];
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

- (NSArray*)filePathsRemovingFromSelection {
    if (!_removeReferenceView.filePathTable.selectedRowIndexes)
        return nil;
    
    NSMutableArray* removings = [[NSMutableArray alloc] init];
    NSUInteger i = [_removeReferenceView.filePathTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        NSString* filePath = _filePathsWithCondition[i];
        [removings addObject:filePath];
        i = [_removeReferenceView.filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    return removings;
}

- (IBAction)button0Action:(id)sender {
    if (_scene == kRemoveReferenceSceneSelection) {
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseCancel];
    }
    else if (_scene == kRemoveReferenceSceneConfirm) {
        _scene = kRemoveReferenceSceneSelection;
        [self createScene:_scene];
    }
}

- (IBAction)button1Action:(id)sender {
    if (_scene == kRemoveReferenceSceneSelection) {
        _scene = kRemoveReferenceSceneConfirm;
        [self createScene:_scene];
    }
    else if (_scene == kRemoveReferenceSceneConfirm) {
        if (!_project)
            return;
        
        [_project removeReferences:_filePathsRemoving];
        
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseOK];
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (tableView == _removeReferenceView.filePathTable)
        return _filePathsWithCondition.count;
    else if (tableView == _filePathView.filePathTable)
        return _filePathsRemoving.count;
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = nil;
    if (tableView == _removeReferenceView.filePathTable) {
        filePath = _filePathsWithCondition[row];
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
        filePath = _filePathsRemoving[row];
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
    if (tableView == _removeReferenceView.filePathTable) {
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

- (void)textViewTextChanged:(CEETextView*)textView {
    if (textView == _removeReferenceView.filePathInput) {
        _condition = [_removeReferenceView.filePathInput.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        
        if (!_project)
            return;
        
        _filePathsWithCondition = [_project getReferenceFilePathsWithCondition:_condition];
        [_removeReferenceView.filePathTable reloadData];
    }
}

@end
