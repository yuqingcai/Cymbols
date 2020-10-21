//
//  CEEProjectCleanViewController.m
//  Cymbols
//
//  Created by qing on 2020/9/28.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEProjectCleanViewController.h"
#import "CEEButton.h"

@interface CEEProjectCleanViewController ()
@property (strong) CEEProject* project;
@end

@implementation CEEProjectCleanViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)viewDidAppear {
    [super viewDidAppear];
}

- (IBAction)confirm:(id)sender {
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    _project = [projectController currentDocument];
    
    if (_project)
        cee_database_symbols_delete(_project.database);
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)cancle:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseCancel];
}

@end
