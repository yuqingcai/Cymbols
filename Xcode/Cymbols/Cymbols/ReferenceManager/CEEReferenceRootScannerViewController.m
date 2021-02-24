//
//  CEEReferenceRootScannerViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProject.h"
#import "CEEStyleManager.h"
#import "CEEReferenceRootScannerViewController.h"
#import "CEEFilePathScannerView.h"
#import "CEEFilePathSelectionView.h"
#import "CEEFilePathView.h"
#import "CEEButton.h"
#import "CEEImageTextTableCellView.h"
#import "CEETextTableCellView.h"

typedef NS_ENUM(NSInteger, ScannerScene) {
    kScannerSceneScanning,
    kScannerSceneSelection,
    kScannerSceneConfirm,
};

@interface CEEReferenceRootScannerViewController ()
@property ScannerScene scene;
@property (weak) CEEView* currentView;
@property (weak) IBOutlet CEEButton *button0;
@property (weak) IBOutlet CEEButton *button1;
@property (strong) CEEFilePathScannerView* filePathScanningView;
@property (strong) CEEFilePathSelectionView* filePathSelectionView;
@property (strong) CEEFilePathView* filePathView;
@property (strong) NSMutableArray* unreferencedFilePaths;
@property (strong) NSMutableArray* selectedFilePaths;
@property (weak) CEEProject* project;
@property BOOL isScanning;
@property BOOL cancelScanning;
@end

@implementation CEEReferenceRootScannerViewController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self.view setStyleState:kCEEViewStyleStateActived];
    
    AppDelegate* delegate = [NSApp delegate];
    
    _filePathScanningView = (CEEFilePathScannerView*)[delegate nibObjectWithClass:[CEEFilePathScannerView class] fromNibNamed:@"FilePathScannerView"];
    [_filePathScanningView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _filePathSelectionView = (CEEFilePathSelectionView*)[delegate nibObjectWithClass:[CEEFilePathSelectionView class] fromNibNamed:@"FilePathSelectionView"];
    [_filePathSelectionView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _filePathView = (CEEFilePathView*)[delegate nibObjectWithClass:[CEEFilePathView class] fromNibNamed:@"FilePathView"];
    [_filePathView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    [_filePathSelectionView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_filePathSelectionView.filePathTable setDelegate:self];
    [_filePathSelectionView.filePathTable setDataSource:self];
    [_filePathSelectionView.filePathTable setNumberOfColumns:2];
    [_filePathSelectionView.filePathTable setTarget:self];
    [_filePathSelectionView.filePathTable setAllowsMultipleSelection:YES];
    [_filePathSelectionView.selectAllButton setTarget:self];
    [_filePathSelectionView.selectAllButton setAction:@selector(selectAll:)];
    [_filePathScanningView.progressBar setMinValue:0.0];
    [_filePathScanningView.progressBar setMaxValue:1.0];
    [_filePathScanningView.progressBar setDoubleValue:0.0];
        
    [_filePathView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_filePathView.filePathTable setDelegate:self];
    [_filePathView.filePathTable setDataSource:self];
    [_filePathView.filePathTable setNumberOfColumns:2];
    [_filePathView.filePathTable setTarget:self];
}

- (void)viewWillAppear {
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    _scene = kScannerSceneScanning;
    [self createScene:_scene];
    [self setViewStyleState:kCEEViewStyleStateActived];
    _unreferencedFilePaths = nil;
    _selectedFilePaths = nil;
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [self scanReferenceFilePaths];
}

- (void)UISetupBeforScan {
    [_filePathScanningView.progressBar setMinValue:0.0];
    [_filePathScanningView.progressBar setMaxValue:1.0];
    [_filePathScanningView.progressBar setDoubleValue:0.0];
    [_button0 setEnabled:YES];
    [_button1 setEnabled:NO];
}

- (void)UISetupAfterSscan {
    [_button0 setEnabled:YES];
    [_button1 setEnabled:YES];
}

- (void)scanReferenceFilePaths {
    @autoreleasepool {
        _cancelScanning = NO;
        
        NSArray* referenceRoots = [_project getReferenceRoots];
        __block NSArray* filePaths = ExpandFilePaths(referenceRoots);
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            self->_isScanning = YES;
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self UISetupBeforScan];
            });
            
            for (int i = 0; i < filePaths.count; i ++) {
                NSString* filePath = filePaths[i];
                
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [self->_filePathScanningView.scanningLabel setStringValue:[filePath lastPathComponent]];
                    [self->_filePathScanningView.progressBar setDoubleValue:(double)(i+1)/filePaths.count];
                    if (![self->_project filePathIsReferenced:filePath]) {
                        if (!self->_unreferencedFilePaths)
                            self->_unreferencedFilePaths = [[NSMutableArray alloc] init];
                        [self->_unreferencedFilePaths addObject:filePath];
                    }
                });
                
                if (self->_cancelScanning)
                    break;
            }
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self UISetupAfterSscan];
                self->_isScanning = NO;
                if (self->_cancelScanning) {
                    if (self.view.window.sheetParent)
                        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
                    if ([self.view.window isModalPanel])
                        [NSApp stopModalWithCode:NSModalResponseCancel];
                }
                else {
                    if (self->_unreferencedFilePaths && self->_unreferencedFilePaths.count) {
                        self->_scene = kScannerSceneSelection;
                        self->_selectedFilePaths = [[NSMutableArray alloc] initWithArray:self->_unreferencedFilePaths];
                        self->_filePathSelectionView.selectAllButton.state = NSControlStateValueOn;
                        [self createScene:self->_scene];
                    }
                    else {
                        if (self.view.window.sheetParent)
                            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
                        if ([self.view.window isModalPanel])
                            [NSApp stopModalWithCode:NSModalResponseCancel];
                    }
                }
            });
        });
    }
}

- (void)createScene:(ScannerScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kScannerSceneScanning) {
        _currentView = (CEEView*)_filePathScanningView;
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kScannerSceneSelection) {
        [_filePathSelectionView.filePathTable reloadData];
        _filePathSelectionView.filePathLabel.stringValue = @"The Following Files are Unreferenced";
        _currentView = (CEEView*)_filePathSelectionView;
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kScannerSceneConfirm) {
        [_filePathView.filePathTable reloadData];
        _filePathView.filePathLabel.stringValue = @"The Following Files are Adding to Project";
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
    if (_scene == kScannerSceneScanning) {
        if (_isScanning && !_cancelScanning)
            _cancelScanning = YES;
    }
    else if (_scene == kScannerSceneSelection) {
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
        if ([self.view.window isModalPanel])
                [NSApp stopModalWithCode:NSModalResponseCancel];
    }
    else if (_scene == kScannerSceneConfirm) {
        _scene = kScannerSceneSelection;
        [self createScene:_scene];
    }
}

- (IBAction)button1Action:(id)sender {
    if (_scene == kScannerSceneSelection) {
        _scene = kScannerSceneConfirm;
        [self createScene:_scene];
    }
    else if (_scene == kScannerSceneConfirm) {
        if (!_project)
            return;
        
        [_project addReferences:_selectedFilePaths];
        
        if (self.view.window.sheetParent)
            [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
        if ([self.view.window isModalPanel])
            [NSApp stopModalWithCode:NSModalResponseOK];
    }
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (tableView == _filePathSelectionView.filePathTable) {
        if (column == 0)
            return @"FileName";
        else if (column == 1)
            return @"FilePath";
        else
            return @"";
    }
    else if (tableView == _filePathView.filePathTable) {
        if (column == 0)
            return @"FileName";
        else if (column == 1)
            return @"FilePath";
        else
            return @"";
    }
    return @"";
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (tableView == _filePathSelectionView.filePathTable) {
        if (_unreferencedFilePaths)
            return _unreferencedFilePaths.count;
    }
    else if (tableView == _filePathView.filePathTable) {
        if (_selectedFilePaths)
            return _selectedFilePaths.count;
    }
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (tableView == _filePathSelectionView.filePathTable) {
        NSString* filePath = _unreferencedFilePaths[row];
        if (column == 0) {
            CEECheckboxTableCellView *cellView = nil;
            cellView = [_filePathSelectionView.filePathTable makeViewWithIdentifier:@"IDCheckboxTableCellView"];
            if (cellView) {
                if (!cellView.delegate)
                    [cellView setDelegate:self];
                cellView.checkbox.title = [filePath lastPathComponent];
                cellView.selectedIdentifier = filePath;
                if ([self filePathIsSelected:filePath])
                    [cellView.checkbox setState:NSControlStateValueOn];
                else
                    [cellView.checkbox setState:NSControlStateValueOff];
            }
            return cellView;
        }
        else if (column == 1) {
            CEETextTableCellView *cellView = [_filePathSelectionView.filePathTable makeViewWithIdentifier:@"IDTextTableCellView"];
            if (cellView)
                cellView.text.stringValue = [_project shortFilePath:filePath];
            return cellView;
        }
    }
    else if (tableView == _filePathView.filePathTable) {
        NSString* filePath = _selectedFilePaths[row];
        if (column == 0) {
            CEEImageTextTableCellView* cellView = nil;
            cellView = [_filePathView.filePathTable makeViewWithIdentifier:@"IDImageTextTableCellView"];
            if (cellView) {
                cellView.text.stringValue = [filePath lastPathComponent];
                [cellView.icon setImage:[styleManager filetypeIconFromFilePath:filePath]];
            }
            return cellView;
        }
        else if (column == 1) {
            CEETextTableCellView* cellView = [_filePathView.filePathTable makeViewWithIdentifier:@"IDTextTableCellView"];
            if (cellView)
                cellView.text.stringValue = [_project shortFilePath:filePath];
            return cellView;
        }
    }
    return nil;
}

- (BOOL)filePathIsSelected:(NSString*)filePath {
    for (NSInteger i = 0; i < _selectedFilePaths.count; i ++) {
        if ([_selectedFilePaths[i] compare:filePath options:NSLiteralSearch] == NSOrderedSame)
            return YES;
    }
    return NO;
}

- (void)selectAll:(id)sender {
    if (_filePathSelectionView.selectAllButton.state == NSControlStateValueOn) {
        _selectedFilePaths = [[NSMutableArray alloc] init];
        for (NSString* filePath in _unreferencedFilePaths)
            [_selectedFilePaths addObject:filePath];
    }
    else {
        _selectedFilePaths = nil;
    }
    
    [_filePathSelectionView.filePathTable reloadData];
}

#pragma mark - protocol CEEFilePathConfirmCellDelegate
- (void)select:(NSString*)filePath {
    if (![self filePathIsSelected:filePath]) {
        if (!_selectedFilePaths)
            _selectedFilePaths = [[NSMutableArray alloc] init];
        [_selectedFilePaths addObject:filePath];
    }
}

- (void)deselect:(NSString*)filePath {
    if ([self filePathIsSelected:filePath])
        [_selectedFilePaths removeObject:filePath];
}

@end
