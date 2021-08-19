//
//  CEESourceHistoryViewController.m
//  Cymbols
//
//  Created by qing on 2020/3/20.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEESourceHistoryViewController.h"
#import "CEEImageTextTableCellView.h"
#import "CEETextTableCellView.h"
#import "CEEProject.h"
#import "CEEStyleManager.h"

@interface CEESourceHistoryViewController ()
@property (weak) IBOutlet CEETableView *historyTable;
@end

@implementation CEESourceHistoryViewController

@synthesize port = _port;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_historyTable setNibNameOfCellView:@"TableCellViews"];
    [_historyTable setDataSource:self];
    [_historyTable setDelegate:self];
    [_historyTable setTarget:self];
    [_historyTable setNumberOfColumns:1];
    [_historyTable setAction:@selector(selectRow:)];
    [_historyTable setEnableDrawHeader:NO];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_historyTable reloadData];
    [self highlightSelectionInHistoryTable];
}

- (void)setPort:(CEESessionPort *)port {
    _port = port;
    [_historyTable reloadData];
    [self highlightSelectionInHistoryTable];
}

- (CEESessionPort*)port {
    return _port;
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_port)
        return 0;
    
    if (!_port.sourceBufferReferences)
        return 0;
    return _port.sourceBufferReferences.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FileName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEESourceBufferReferenceContext* reference = _port.sourceBufferReferences[row];
    
    CEEImageTextTableCellView *cellView = [_historyTable makeViewWithIdentifier:@"IDImageTextTableCellView"];
    NSString* string = [reference.filePath lastPathComponent];
    BOOL validated = YES;
    BOOL isDirectory = NO;
    if (![[NSFileManager defaultManager] fileExistsAtPath:reference.filePath isDirectory:&isDirectory] || isDirectory)
        validated = NO;
        
    if (validated) {
        cellView.text.stringValue = [string stringByAppendingFormat:@" - %ld (bytes)", reference.presentBufferOffset];
        [cellView.icon setImage:[styleManager iconFromFileName:[reference.filePath lastPathComponent]]];
    }
    else {
        cellView.text.stringValue = [string stringByAppendingString:@" (deleted)"];
        [cellView.icon setImage:[styleManager iconFromName:@"icon_file_not_existed_16x16"]];
    }
    return cellView;
}

- (void)highlightSelectionInHistoryTable {    
    NSIndexSet* indexSet = nil;
    CEESourceBufferReferenceContext* reference = [_port currentSourceBufferReference];
    NSArray* references = [_port sourceBufferReferences];
    for (NSInteger i = 0; i < references.count; i ++) {
        if (reference == references[i]) {
            indexSet = [NSIndexSet indexSetWithIndex:i];
            break;
        }
    }
    [_historyTable selectRowIndexes:indexSet byExtendingSelection:NO];
    [_historyTable scrollRowToVisible:[indexSet firstIndex]];
}

- (IBAction)selectRow:sender {
    if (!_historyTable.selectedRowIndexes)
        return;
    [_port presentSourceBufferReference:_port.sourceBufferReferences[_historyTable.selectedRow]];
}

- (CEETableView*)tableView {
    return _historyTable;
}

@end
