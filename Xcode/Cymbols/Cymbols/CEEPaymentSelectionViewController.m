//
//  CEEPaymentSelectionViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/10.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEPaymentSelectionViewController.h"
#import "CEEPaymentSelectionTableCellView.h"
#import "CEEStorePayment.h"
#import "CEEButton.h"

@interface CEEPaymentSelectionViewController ()
@property (weak) IBOutlet CEETableView *paymentSelectionTable;
@property (weak) IBOutlet CEEButton *purchaseButton;
@property (weak) IBOutlet CEEButton *restoreButton;
@property (weak) IBOutlet CEEButton *cancelButton;
@property (weak) IBOutlet NSProgressIndicator *indicator;
@end

@implementation CEEPaymentSelectionViewController

- (void)viewDidLoad {
    [super viewDidLoad];
        
    [_paymentSelectionTable setNibNameOfCellView:@"TableCellViews"];
    [_paymentSelectionTable setDelegate:self];
    [_paymentSelectionTable setDataSource:self];
    [_paymentSelectionTable setNumberOfColumns:1];
    [_paymentSelectionTable setTarget:self];
    //[_paymentSelectionTable setAction:@selector(selectRow:)];
    [_paymentSelectionTable setAllowsMultipleSelection:YES];
    [_paymentSelectionTable setEnableDrawHeader:NO];
    [_paymentSelectionTable setSelectable:YES];
    [_paymentSelectionTable setAllowsMultipleSelection:NO];
    
    [_purchaseButton setEnabled:NO];
    [_restoreButton setEnabled:NO];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(purchaseError:) name:CEENotificationPurchaseError object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(purchaseSuccessful:) name:CEENotificationPurchaseSuccessful object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(purchaseNoProductRestoredResponse:) name:CEENotificationPurchaseNoProductRestored object:nil];
    
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_paymentSelectionTable reloadData];
    
    if (_paymentSelectionTable.numberOfRows) {
        NSIndexSet* indexSet = [NSIndexSet indexSetWithIndex:0];
        [_paymentSelectionTable selectRowIndexes:indexSet byExtendingSelection:NO];
    }
    
    if (_paymentSelectionTable.selectedRowIndexes) {
        [_purchaseButton setEnabled:YES];
        [_restoreButton setEnabled:YES];
    }
    
    [_indicator setHidden:YES];
}

- (IBAction)cancel:(id)sender {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = @"Purchase Message";
    alert.informativeText = @"Are you want to cancel subscription purchase?";
    alert.alertStyle = NSAlertStyleWarning;
    [alert addButtonWithTitle:@"OK"];
    [alert addButtonWithTitle:@"Cancel"];
    
    [alert beginSheetModalForWindow:self.view.window completionHandler:^(NSInteger returnCode) {
        if (returnCode == NSAlertFirstButtonReturn) {
            if (self.view.window.sheetParent)
                [self.view.window.sheetParent endSheet:self.view.window returnCode:kCEEPurchaseStateCancel];
            if ([self.view.window isModalPanel])
                [NSApp stopModalWithCode:kCEEPurchaseStateCancel];
        }
    }];
}

- (IBAction)restore:(id)sender {
    [_purchaseButton setEnabled:NO];
    [_cancelButton setEnabled:NO];
    [_restoreButton setEnabled:NO];
    [_indicator setHidden:NO];
    [_indicator startAnimation:self];
    [[CEEStorePayment sharedInstance] restore];
}

- (IBAction)purchase:(id)sender {
    [_purchaseButton setEnabled:NO];
    [_cancelButton setEnabled:NO];
    [_restoreButton setEnabled:NO];
    [_indicator setHidden:NO];
    [_indicator startAnimation:self];
    NSIndexSet* indexSet = [_paymentSelectionTable selectedRowIndexes];
    [[CEEStorePayment sharedInstance] purchaseProductByIndex:[indexSet firstIndex]];
}

- (void)purchaseError:(NSNotification*)notification {
    [_indicator stopAnimation:self];
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:kCEEPurchaseStateError];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:kCEEPurchaseStateError];
}

- (void)purchaseSuccessful:(NSNotification*)notification {
    [_indicator stopAnimation:self];
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:kCEEPurchaseStateSuccessful];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:kCEEPurchaseStateSuccessful];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    NSArray* productDescriptors = [[CEEStorePayment sharedInstance] productDescriptors];
    if (!productDescriptors)
        return 0;
    return productDescriptors.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FilePath";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    NSArray* productDescriptors = [[CEEStorePayment sharedInstance] productDescriptors];
    CEEProductDescriptor* descriptor = productDescriptors[row];
    CEEPaymentSelectionTableCellView *cellView = nil;
    cellView = [_paymentSelectionTable makeViewWithIdentifier:@"IDPaymentSelectionTableCellView"];
    cellView.title.stringValue = descriptor.title;
    cellView.text.stringValue = descriptor.details;
    return cellView;
}

- (void)purchaseNoProductRestoredResponse:(NSNotification*)notification {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = @"Restore Message";
    alert.informativeText = @"Can't find any purchased product to restore.";
    alert.alertStyle = NSAlertStyleWarning;
    [alert addButtonWithTitle:@"OK"];
    [alert beginSheetModalForWindow:self.view.window completionHandler:^(NSInteger returnCode) {
        [self->_purchaseButton setEnabled:YES];
        [self->_cancelButton setEnabled:YES];
        [self->_restoreButton setEnabled:YES];
        [self->_indicator setHidden:YES];
        [self->_indicator stopAnimation:self];
    }];
}

@end
