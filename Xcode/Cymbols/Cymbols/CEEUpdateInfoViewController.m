//
//  CEEUpdateInfoViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/2/13.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEUpdateInfoViewController.h"
#import "CEETextLabel.h"
#import "CEETextTitle.h"

@interface CEEUpdateInfoViewController ()
@property (weak) IBOutlet CEETextTitle *updateTitle;
@property (weak) IBOutlet CEETextLabel *updateLabel;

@end

@implementation CEEUpdateInfoViewController

- (void)viewDidAppear {
    [super viewDidAppear];
    
    if (_infoString)
        _updateLabel.stringValue = _infoString;
}

- (IBAction)viewDetails:(id)sender {
    NSURL* URL = [NSURL URLWithString:@"https://cymbols.io/news.html"];
    [[NSWorkspace sharedWorkspace] openURL:URL];
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)closeWindow:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
