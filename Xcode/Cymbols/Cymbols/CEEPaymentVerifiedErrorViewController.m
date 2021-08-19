//
//  CEEPaymentVerifiedErrorViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/18.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEPaymentVerifiedErrorViewController.h"
#import "CEETextTitle.h"

@interface CEEPaymentVerifiedErrorViewController ()
@property (weak) IBOutlet CEETextTitle *info;
@end

@implementation CEEPaymentVerifiedErrorViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    [_info setAlignment:kCEETextLayoutAlignmentCenter];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_info setStringValue:@"Cymbols Payment Verified Error\n\nYour device may be lost connect from internet,\nApplication will be terminated."];
}

- (IBAction)confirm:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
