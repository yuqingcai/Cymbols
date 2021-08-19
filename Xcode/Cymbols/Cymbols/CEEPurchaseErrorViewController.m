//
//  CEEPurchaseErrorViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/16.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEPurchaseErrorViewController.h"
#import "CEETextTitle.h"

@interface CEEPurchaseErrorViewController ()
@property (weak) IBOutlet CEETextTitle *info;

@end

@implementation CEEPurchaseErrorViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_info setAlignment:kCEETextLayoutAlignmentCenter];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_info setStringValue:@"Cymbols Purchase Error\n\nThe purchase may be canceled or your device has lost connect from internet,\nApplication will be terminated."];
}

- (IBAction)confirm:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
