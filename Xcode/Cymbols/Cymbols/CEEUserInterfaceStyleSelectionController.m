//
//  CEEUserInterfaceStyleSelectionController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/13.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "CEEStyleManager.h"
#import "CEEUserInterfaceStyleSelectionController.h"
#import "CEEButton.h"

@interface CEEUserInterfaceStyleSelectionController ()
@property (strong) IBOutlet CEEButton *confirmedButton;
@property (strong) IBOutlet CEEButton *cancelButton;
@property (strong) IBOutlet CEETableView *styleTable;
@property (strong) NSString* selected;
@end

@implementation CEEUserInterfaceStyleSelectionController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_styleTable setDataSource:self];
    [_styleTable setDelegate:self];
    [_styleTable setColumns:1];
    [_styleTable setTarget:self];
    [_styleTable setAction:@selector(selectStyle:)];
    [_styleTable reloadData];
}

- (void)viewWillAppear {
    //if ([self.view conformsToProtocol:@protocol(CEEUIStatesable)])
    //    [(id<CEEUIStatesable>)self.view clearState:kCEEUIStateActived];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    _selected = [styleManager userInterfaceStyleName];
    [self highlightSelectionInStyleTable];
}

- (IBAction)confirm:(id)sender {
    [self dismissController:self];
}

- (IBAction)cancel:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [self highlightSelectionInStyleTable];
    [styleManager setUserInterfaceStyleName:_selected];
    [self dismissController:self];
}

- (void)highlightSelectionInStyleTable {
    if (!_selected)
        return;
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSArray* names = [styleManager userInterfaceStyleNames];
    for (NSInteger i = 0; i < names.count; i ++) {
        NSString* name = names[i];
        if ([name caseInsensitiveCompare:_selected] == NSOrderedSame) {
            //NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
            //[_styleTable selectRowIndexes:indexSet byExtendingSelection:NO];
            //[_styleTable scrollRowToVisible:i];
            break;
        }
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    return 0;
}

- (CEEView*)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    return nil;
}

- (CEEView*)tableView:(CEETableView*)tableView cellViewWithIdentifier:(NSString*)identifier {
    return nil;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"Style Name";
}

- (void)selectStyle:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSInteger i = [_styleTable selectedRow];
    NSArray* names = [styleManager userInterfaceStyleNames];
    NSString* name = [names objectAtIndex:i];
    [styleManager setUserInterfaceStyleName:name];
}

@end
