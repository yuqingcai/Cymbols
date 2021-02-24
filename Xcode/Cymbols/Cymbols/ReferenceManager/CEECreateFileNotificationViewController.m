//
//  CEECreateFileNotificationViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/30.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProject.h"
#import "CEEButton.h"
#import "CEEStyleManager.h"
#import "CEECreateFileNotificationViewController.h"

@interface CEECreateFileNotificationViewController ()
@property (weak) IBOutlet CEEButton *button0;
@property (weak) CEEProject* project;
@end

@implementation CEECreateFileNotificationViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setStyleState:kCEEViewStyleStateActived];
}

- (void)viewWillAppear {
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
}

- (IBAction)button0Action:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
