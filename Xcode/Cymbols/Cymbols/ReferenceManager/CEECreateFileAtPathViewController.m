//
//  CEECreateFileAtPathViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProject.h"
#import "CEEButton.h"
#import "CEECheckBox.h"
#import "CEEStyleManager.h"
#import "CEECreateFileAtPathViewController.h"
#import "CEECreateFileNotificationViewController.h"
#import "CEEFileSettingView.h"

typedef NS_ENUM(NSInteger, CreateFileAtPathScene) {
    kCreateFileAtPathSceneSetting,
};

@interface CEECreateFileAtPathViewController ()
@property CreateFileAtPathScene scene;
@property (weak) CEEView* currentView;
@property (strong) CEEFileSettingView* fileSettingView;
@property (weak) IBOutlet CEEButton *button0;
@property (weak) IBOutlet CEEButton *button1;
@property (strong) NSWindowController* createFileNotificationWindowController;
@property (weak) CEEProject* project;
@end

@implementation CEECreateFileAtPathViewController

- (void)viewDidLoad {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [super viewDidLoad];
    [self.view setStyleState:kCEEViewStyleStateActived];
    
    AppDelegate* delegate = [NSApp delegate];
    _fileSettingView = (CEEFileSettingView*)[delegate nibObjectWithClass:[CEEFileSettingView class] fromNibNamed:@"FileSettingView"];
    [_fileSettingView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    if (!_createFileNotificationWindowController)
        _createFileNotificationWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDCreateFileNotificationWindowController"];
}

- (void)viewWillAppear {
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    _scene = kCreateFileAtPathSceneSetting;
    [self createScene:_scene];
    [self setViewStyleState:kCEEViewStyleStateActived];
}

- (void)createScene:(CreateFileAtPathScene)scene {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (_currentView)
        [_currentView removeFromSuperview];
    
    if (scene == kCreateFileAtPathSceneSetting) {
        _currentView = (CEEView*)_fileSettingView;
        
        if (_createFileType == kCEECreateFileTypeFile) {
            [_fileSettingView.titleIcon setImage:[styleManager iconFromName:@"icon_file_16x16"]];
            _fileSettingView.titleLabel.stringValue = @"Create New File";
            _fileSettingView.nameInput.stringValue = @"NewFile";
            _fileSettingView.referenceCheckbox.hidden = NO;
            _fileSettingView.referenceCheckbox.enabled = YES;
            _fileSettingView.referenceCheckbox.state = NSControlStateValueOn;
            _fileSettingView.referenceCheckbox.title = @"Add Reference to Project";
        }
        else if (_createFileType == kCEECreateFileTypeDirectory) {
            [_fileSettingView.titleIcon setImage:[styleManager iconFromName:@"icon_directory_16x16"]];
            _fileSettingView.titleLabel.stringValue = @"Create New Folder";
            _fileSettingView.nameInput.stringValue = @"NewFolder";
            _fileSettingView.referenceCheckbox.hidden = YES;
            _fileSettingView.referenceCheckbox.enabled = NO;
            _fileSettingView.referenceCheckbox.state = NSControlStateValueOff;
            _fileSettingView.referenceCheckbox.title = @"Add Reference to Project";
        }
        _button0.title = @"Cancel";
        _button1.title = @"OK";
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
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (BOOL)fileNameIsValidated:(NSString*)fileName {
    return YES;
}

- (IBAction)button1Action:(id)sender {
    BOOL ret = NO;
    BOOL isDirectory = NO;
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* fileName = _fileSettingView.nameInput.stringValue;
    if (![self fileNameIsValidated:fileName]) {
        CEECreateFileNotificationViewController* viewController = (CEECreateFileNotificationViewController*)_createFileNotificationWindowController.contentViewController;
        [viewController.notificationIcon setImage:[styleManager iconFromName:@"icon_alert_32x32"]];
        viewController.notificationTitle.stringValue = @"Invalid FileNme";
        viewController.notificationLabel.stringValue = @"Invalid FileNme";
        [self.view.window beginSheet:_createFileNotificationWindowController.window completionHandler:(^(NSInteger result) {
            [NSApp stopModalWithCode:result];
        })];
        [NSApp runModalForWindow:self.view.window];
        return;
    }
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:_relativePath isDirectory:&isDirectory]) {
        CEECreateFileNotificationViewController* viewController = (CEECreateFileNotificationViewController*)_createFileNotificationWindowController.contentViewController;
        [viewController.notificationIcon setImage:[styleManager iconFromName:@"icon_alert_32x32"]];
        viewController.notificationTitle.stringValue = @"Invalid Path";
        viewController.notificationLabel.stringValue = @"Invalid Path";
        [self.view.window beginSheet:_createFileNotificationWindowController.window completionHandler:(^(NSInteger result) {
            [NSApp stopModalWithCode:result];
        })];
        [NSApp runModalForWindow:self.view.window];
        return;
    }
    
    NSString* parentPath = nil;
    if (isDirectory)
        parentPath = _relativePath;
    else
        parentPath = [_relativePath stringByDeletingLastPathComponent];
    
    NSString* filePath = [parentPath stringByAppendingPathComponent:fileName];
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        CEECreateFileNotificationViewController* viewController = (CEECreateFileNotificationViewController*)_createFileNotificationWindowController.contentViewController;
        [viewController.notificationIcon setImage:[styleManager iconFromName:@"icon_alert_32x32"]];
        viewController.notificationTitle.stringValue = @"Duplicated fileNme";
        viewController.notificationLabel.stringValue = @"Duplicated fileNme";
        [self.view.window beginSheet:_createFileNotificationWindowController.window completionHandler:(^(NSInteger result) {
            [NSApp stopModalWithCode:result];
        })];
        [NSApp runModalForWindow:self.view.window];
        return;
    }
    
    if (_createFileType == kCEECreateFileTypeFile) {
        ret = [_project createFile:filePath];
        if (ret && _fileSettingView.referenceCheckbox.isEnabled && _fileSettingView.referenceCheckbox.state == NSControlStateValueOn)
            [_project addReferences:@[filePath]];
    }
    else if (_createFileType == kCEECreateFileTypeDirectory) {
        ret = [_project createDirectory:filePath];
    }
    
    if (!ret) {
#ifdef DEBUG
        NSLog(@"ERROR: Create File / Directory Failed.");
#endif
        return;
    }
    else {
#ifdef DEBUG
        NSLog(@"INFO: Create File / Directory At Path:%@.", filePath);
#endif
    }

    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
