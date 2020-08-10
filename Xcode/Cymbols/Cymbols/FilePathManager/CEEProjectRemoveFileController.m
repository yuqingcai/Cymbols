//
//  CEEProjectRemoveFileController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEEStyleManager.h"
#import "CEEProjectRemoveFileController.h"
#import "CEERemoveFileSelectionView.h"
#import "CEEFilePathConfirmView.h"
#import "CEEButton.h"
#import "CEEProject.h"
#import "CEEFileNameCellView.h"
#import "CEEFilePathCellView.h""

typedef NS_ENUM(NSInteger, CEEProjectRemoveFileScene) {
    kCEEFileSelection,
    kCEEFileConfirm,
};

@interface CEEProjectRemoveFileController ()
@property CEEProjectRemoveFileScene scene;
@property (strong) CEEView* currentView;
@property (strong) IBOutlet CEEButton *button0;
@property (strong) IBOutlet CEEButton *button1;
@property (strong) CEERemoveFileSelectionView* removeFileSelectionView;
@property (strong) CEEFilePathConfirmView* filePathConfirmView;
@property (strong) NSArray* filePathsWithCondition;
@property (strong) NSArray* filePathsRemoving;
@property (strong) NSString* condition;
@end

@implementation CEEProjectRemoveFileController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self setViewStyle:kCEEViewStyleActived];
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    _removeFileSelectionView = (CEERemoveFileSelectionView*)[delegate nibObjectWithClass:[CEERemoveFileSelectionView class] fromNibNamed:@"RemoveFileSelectionView"];
    [_removeFileSelectionView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    _filePathConfirmView = (CEEFilePathConfirmView*)[delegate nibObjectWithClass:[CEEFilePathConfirmView class] fromNibNamed:@"FilePathConfirmView"];
    [_filePathConfirmView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    _condition = nil;
    _filePathsRemoving = nil;
    _filePathsWithCondition = nil;
    [_removeFileSelectionView.sourceTable setDelegate:self];
    [_removeFileSelectionView.sourceTable setDataSource:self];
    [_removeFileSelectionView.sourceTable setColumns:2];
    [_removeFileSelectionView.sourceTable setTarget:self];
    [_removeFileSelectionView.sourceTable registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    [_removeFileSelectionView.sourceTable setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
    [_removeFileSelectionView.sourceTable setAllowsMultipleSelection:YES];
    [_removeFileSelectionView.filePathInput setDelegate:self];
    
    [_filePathConfirmView.sourceTable setDelegate:self];
    [_filePathConfirmView.sourceTable setDataSource:self];
    [_filePathConfirmView.sourceTable setColumns:2];
    [_filePathConfirmView.sourceTable setTarget:self];
    [_filePathConfirmView.label setStringValue:@"The following Files is removing from Project"];
}

- (void)viewWillAppear {
    _scene = kCEEFileSelection;
    [self createScene:_scene];
}

- (void)createScene:(CEEProjectRemoveFileScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kCEEFileSelection) {
        _filePathsWithCondition = [_session.project getFilePathsWithCondition:_condition];
        _currentView = (CEEView*)_removeFileSelectionView;
        [_removeFileSelectionView.sourceTable reloadData];
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kCEEFileConfirm) {
        _currentView = (CEEView*)_filePathConfirmView;
        _filePathsRemoving = [self filePathsRemovingFromSelection];
        [_filePathConfirmView.sourceTable reloadData];
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

- (NSArray*)filePathsRemovingFromSelection {
    if (!_removeFileSelectionView.sourceTable.selectedRowIndexes)
        return nil;
    
    NSMutableArray* removings = [[NSMutableArray alloc] init];
    NSUInteger i = [_removeFileSelectionView.sourceTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        NSString* filePath = _filePathsWithCondition[i];
        [removings addObject:filePath];
        i = [_removeFileSelectionView.sourceTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    return removings;
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
        [self.session.project removeFilePaths:_filePathsRemoving];
        [self dismissController:self];
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (tableView == _removeFileSelectionView.sourceTable)
        return _filePathsWithCondition.count;
    else if (tableView == _filePathConfirmView.sourceTable)
        return _filePathsRemoving.count;
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    if (tableView == _removeFileSelectionView.sourceTable) {
        if (column == 0) {
            CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
            cellView.title.stringValue = [_filePathsWithCondition[row] lastPathComponent];
            return cellView;
        }
        else if (column == 1) {
            CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
            cellView.title.stringValue = _filePathsWithCondition[row];
            return cellView;
        }
    }
    else if (tableView == _filePathConfirmView.sourceTable) {
        if (column == 0) {
            CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
            cellView.title.stringValue = [_filePathsRemoving[row] lastPathComponent];
            return cellView;
        }
        else if (column == 1) {
            CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
            cellView.title.stringValue = _filePathsRemoving[row];
            return cellView;
        }
    }
    return nil;
}


- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (tableView == _removeFileSelectionView.sourceTable) {
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

- (void)textViewTextChanged:(CEETextView*)textView {
    if (textView == _removeFileSelectionView.filePathInput) {
        _condition = [_removeFileSelectionView.filePathInput.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        _filePathsWithCondition = [_session.project getFilePathsWithCondition:_condition];
        [_removeFileSelectionView.sourceTable reloadData];
    }
}

@end
