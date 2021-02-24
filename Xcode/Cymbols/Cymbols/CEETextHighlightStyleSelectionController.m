//
//  CEETextHighlightStyleSelectionController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/13.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEETextHighlightStyleSelectionController.h"
#import "CEEImageTextTableCellView.h"
#import "CEEButton.h"

@interface CEETextHighlightStyleSelectionController ()
@property (strong) IBOutlet CEEButton *confirmedButton;
@property (strong) IBOutlet CEEButton *cancelButton;
@property (strong) IBOutlet CEETableView *styleTable;
@property (strong) NSString* selected;
@end

@implementation CEETextHighlightStyleSelectionController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [_styleTable setNibNameOfCellView:@"TableCellViews"];
    [_styleTable setDataSource:self];
    [_styleTable setDelegate:self];
    [_styleTable setNumberOfColumns:1];
    [_styleTable setTarget:self];
    [_styleTable setAction:@selector(selectStyle:)];
    [_styleTable reloadData];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    _selected = [styleManager textHighlightStyleName];
    [self highlightSelectionInStyleTable];
}

- (IBAction)confirm:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    AppDelegate* delegate = [NSApp delegate];
    [delegate setConfiguration:@"syntax_style" value:[styleManager textHighlightStyleName]];
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)cancel:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [self highlightSelectionInStyleTable];
    [styleManager setTextHighlightStyleName:_selected];
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (void)highlightSelectionInStyleTable {
    if (!_selected)
        return;
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSArray* names = [styleManager textHighlightStyleNames];
    for (NSInteger i = 0; i < names.count; i ++) {
        NSString* name = names[i];
        if ([name caseInsensitiveCompare:_selected] == NSOrderedSame) {
            NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
            [_styleTable selectRowIndexes:indexSet byExtendingSelection:NO];
            [_styleTable scrollRowToVisible:i];
            break;
        }
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSArray* names = [styleManager textHighlightStyleNames];
    return names.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"Style Name";
}

- (CEEView*)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSArray* names = [styleManager textHighlightStyleNames];
    CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
    NSString* fileName = names[row];
    cellView.text.stringValue = fileName;
    [cellView.icon setImage:[styleManager filetypeIconFromFileName:fileName]];
    return cellView;
}

- (void)selectStyle:(id)sender {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSInteger i = [_styleTable selectedRow];
    NSArray* names = [styleManager textHighlightStyleNames];
    NSString* name = [names objectAtIndex:i];
    [styleManager setTextHighlightStyleName:name];
}


@end
