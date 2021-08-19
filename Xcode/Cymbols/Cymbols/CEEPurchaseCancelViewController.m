//
//  CEEPurchaseCancelViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/16.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEPurchaseCancelViewController.h"
#import "CEETextTitle.h"

@interface CEEPurchaseCancelViewController ()
@property (weak) IBOutlet CEETextTitle *info;

@end

@implementation CEEPurchaseCancelViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_info setAlignment:kCEETextLayoutAlignmentCenter];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_info setStringValue:@"Cymbols Purchase Canceled\n\nApplication will be terminated."];
}

- (IBAction)confirm:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
