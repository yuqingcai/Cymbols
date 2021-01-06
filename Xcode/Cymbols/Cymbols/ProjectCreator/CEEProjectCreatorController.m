//
//  CEEProjectCreatorController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/2.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEEProjectCreatorController.h"
#import "CEEProjectSettingView.h"
#import "CEEProjectPropertyView.h"
#import "CEEButton.h"
#import "CEEProject.h"
#import "CEEFileNameCellView.h"
#import "CEEFilePathCellView.h"

typedef NS_ENUM(NSInteger, CEEProjectCreateScene) {
    kCEEProjectSetting,
    kCEEProjectProperty
};


@interface CEEProjectCreatorController ()
@property CEEProjectCreateScene scene;
@property (strong) CEEView* currentView;
@property (strong) IBOutlet CEEButton *button0;
@property (strong) IBOutlet CEEButton *button1;
@property (strong) CEEProjectSettingView* settingView;
@property (strong) CEEProjectPropertyView* propertyView;
@property (strong) CEEProjectSetting* setting;
@end

@implementation CEEProjectCreatorController

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    AppDelegate* delegate = [NSApp delegate];
    
    _settingView = (CEEProjectSettingView*)[delegate nibObjectWithClass:[CEEProjectSettingView class] fromNibNamed:@"ProjectSettingView"];
    [_settingView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _propertyView = (CEEProjectPropertyView*)[delegate nibObjectWithClass:[CEEProjectPropertyView class] fromNibNamed:@"ProjectPropertyView"];
    [_propertyView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    
    [_settingView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_settingView.filePathTable registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    [_settingView.filePathTable setDraggingSourceOperationMask:NSDragOperationEvery forLocal:NO];
    [_settingView.filePathTable setDataSource:self];
    [_settingView.filePathTable setDelegate:self];
    [_settingView.filePathTable setNumberOfColumns:2];
    [_settingView.filePathTable setAllowsMultipleSelection:YES];
    [_settingView.addFilePathButton setTarget:self];
    [_settingView.addFilePathButton setAction:@selector(addUserSelectedFilePaths:)];
    [_settingView.removeFilePathsButton setTarget:self];
    [_settingView.removeFilePathsButton setAction:@selector(removeUserSelectedFilePaths:)];
    
    
    [_propertyView.filePathTable setNibNameOfCellView:@"TableCellViews"];
    [_propertyView.filePathTable setDataSource:self];
    [_propertyView.filePathTable setDelegate:self];
    [_propertyView.filePathTable setNumberOfColumns:2];
    [_propertyView.filePathTable setAllowsMultipleSelection:NO];
}

- (void)viewWillAppear {
    _setting = [self.session.project createEmptyProjectSetting];
    _scene = kCEEProjectSetting;
    [self createScene:_scene];
    [self setViewStyleState:kCEEViewStyleStateActived];
}

- (void)createScene:(CEEProjectCreateScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kCEEProjectSetting) {
        _settingView.nameInput.stringValue = _setting.name;
        [_settingView.filePathTable reloadData];
        _currentView = (CEEView*)_settingView;
        _button0.title = @"Cancel";
        _button1.title = @"Next";
    }
    else if (scene == kCEEProjectProperty) {
        _setting.filePathsExpanded = ExpandFilePaths(_setting.filePathsUserSelected);
        [_propertyView.filePathTable reloadData];
        _currentView = (CEEView*)_propertyView;
        _setting.name = _settingView.nameInput.stringValue;
        _propertyView.projectNameLabel.stringValue = [NSString stringWithFormat:@"Project Name: %@", _setting.name];
        _propertyView.savePathLabel.stringValue = [NSString stringWithFormat:@"Project Path: %@", _setting.path];
        _propertyView.sourceFileLabel.stringValue = [NSString stringWithFormat:@"%lu Files will be added to Project", (unsigned long)_setting.filePathsExpanded.count];
        _button0.title = @"Previous";
        _button1.title = @"Create";
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
    if (_scene == kCEEProjectSetting) {
        [self dismissController:self];
    }
    else if (_scene == kCEEProjectProperty) {
        _scene = kCEEProjectSetting;
        [self createScene:_scene];
    }
}

- (IBAction)button1Action:(id)sender {
    if (_scene == kCEEProjectSetting) {
        [self selectSavePath:self];
    }
    else if (_scene == kCEEProjectProperty) {
        [self createProject];
        [self dismissController:self];
    }
}

- (void)createProject {
    if (_setting.bookmark)
        [self.session.project startAccessSecurityScopedResourcesWithBookmarks:@[_setting.bookmark]];
    
    CEEProjectController* controller = nil;
    if (!self.session.project.database) {
        [self.session.project setProperties:_setting];
    }
    else {
        controller = [NSDocumentController sharedDocumentController];
        [controller createProjectFromSetting:_setting];
    }
    [self.session.project saveProjectSettingAsDefault:_setting];
    
    if (_setting.bookmark)
        [self.session.project stopAccessSecurityScopedResourcesWithBookmarks:@[_setting.bookmark]];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (tableView == _settingView.filePathTable) {
        if (_setting.filePathsUserSelected)
            return _setting.filePathsUserSelected.count;
    }
    else if (tableView == _propertyView.filePathTable) {
        if (_setting.filePathsExpanded)
            return _setting.filePathsExpanded.count;
        
    }
    return 0;
}

- (NSView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    if (tableView == _settingView.filePathTable) {
        NSString* filePath = _setting.filePathsUserSelected[row];
        if (column == 0) {
            CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
            cellView.title.stringValue = [filePath lastPathComponent];
            [cellView.icon setImage:[styleManager filetypeIconFromFilePath:filePath]];
            return cellView;
        }
        else if (column == 1) {
            CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
            cellView.title.stringValue = filePath;
            return cellView;
        }
    }
    else if (tableView == _propertyView.filePathTable) {
        NSString* filePath = _setting.filePathsExpanded[row];
        if (column == 0) {
            CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFileNameCellView"];
            cellView.title.stringValue = [filePath lastPathComponent];
            [cellView.icon setImage:[styleManager filetypeIconFromFilePath:filePath]];
            return cellView;
        }
        else if (column == 1) {
            CEEFilePathCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
            cellView.title.stringValue = filePath;
            return cellView;
        }
    }
    return nil;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (tableView == _settingView.filePathTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    else if (tableView == _propertyView.filePathTable) {
        if (column == 0)
            return @"Name";
        else if (column == 1)
            return @"Path";
        else
            return @"";
    }
    return nil;
}

- (IBAction)selectSavePath:(id)sender {
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    NSString* filePath = NSHomeDirectory();
    if (_setting.path)
        filePath = _setting.path;
    NSURL* URL = [[NSURL alloc] initFileURLWithPath:filePath isDirectory:YES];
    //[openPanel setAccessoryView:[[CEEView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)] ];
    [openPanel setDirectoryURL:URL];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setAllowsMultipleSelection:NO];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanCreateDirectories:YES];
    [openPanel setDelegate:self];
    [openPanel setPrompt:@"Next"];
    [openPanel beginSheetModalForWindow:self.view.window completionHandler: (^(NSInteger result){
        [NSApp stopModalWithCode:result];
        if (result == NSModalResponseOK) {
            self->_setting.path = [[openPanel URL] path];
            self->_setting.bookmark = nil;
            self->_scene = kCEEProjectProperty;
            [self createScene:self->_scene];
        }
    })];
    
    [NSApp runModalForWindow:[self.view window]];
}

- (IBAction)addUserSelectedFilePaths:(id)sender {
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
            CEEProjectSetting* setting = self->_setting;
            NSMutableArray* filePaths = [[NSMutableArray alloc] initWithArray:setting.filePathsUserSelected];
            for (NSURL* URL in [openPanel URLs])
                [filePaths addObject:[URL path]];
            setting.filePathsUserSelected = filePaths;
        }
    })];
    
    [NSApp runModalForWindow:[self.view window]];
    [_settingView.filePathTable reloadData];
}

-(IBAction)removeUserSelectedFilePaths:(id)sender {
    NSMutableArray* filePaths = [[NSMutableArray alloc] initWithArray:_setting.filePathsUserSelected];
    NSMutableArray* removes = [[NSMutableArray alloc] init];
    NSUInteger i = [_settingView.filePathTable.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [removes addObject:filePaths[i]];
        i = [_settingView.filePathTable.selectedRowIndexes indexGreaterThanIndex:i];
    }
    
    for (NSString* filePath in removes)
        [filePaths removeObject:filePath];
    
    _setting.filePathsUserSelected = filePaths;
    [_settingView.filePathTable reloadData];
}

- (NSDragOperation)tableView:(CEETableView *)tableView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation {
    if (tableView == _settingView.filePathTable)
        return NSDragOperationGeneric;
    return NSDragOperationNone;
}

- (BOOL)tableView:(CEETableView *)tableView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation {
    
    if (tableView == _settingView.filePathTable) {
        NSMutableArray* filePaths = [[NSMutableArray alloc] initWithArray:_setting.filePathsUserSelected];
        NSArray* filePathsInPastBoard = [info.draggingPasteboard propertyListForType:NSFilenamesPboardType];
        
        for (NSString* filePath in filePathsInPastBoard)
            [filePaths addObject:filePath];
        
        _setting.filePathsUserSelected = filePaths;
        [_settingView.filePathTable reloadData];
        
        return YES;
    }
    return NO;
}

@end
