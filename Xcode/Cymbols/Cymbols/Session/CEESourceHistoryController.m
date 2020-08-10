//
//  CEESourceHistoryController.m
//  Cymbols
//
//  Created by qing on 2020/3/20.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEESourceHistoryController.h"
#import "CEEFileNameCellView.h"
#import "CEEProject.h"

@interface CEESourceHistoryController ()
@property (weak) IBOutlet CEETableView *historyTable;
@property (weak) NSArray* references;
@end

@implementation CEESourceHistoryController

@synthesize port = _port;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_historyTable setDataSource:self];
    [_historyTable setDelegate:self];
    [_historyTable setTarget:self];
    [_historyTable setColumns:1];
    [_historyTable setAction:@selector(selectRow:)];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_historyTable reloadData];
    NSRect rect = self.view.window.frame;
    CGFloat height = _port.references.count * _historyTable.rowHeight;
    if (height > 400.0)
        height = 400.0;
    rect.size.height = height;
    rect.origin.y -= height;
    [self.view.window setFrame:rect display:YES];
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
    
    _references = _port.references;
    if (!_references)
        return 0;
    return _references.count;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FileName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEBufferReference* reference = _references[row];
    CEESourceBuffer* buffer = reference.buffer;
    CEEFileNameCellView* cellView = [_historyTable makeViewWithIdentifier:@"IDFileNameCellView"];
    NSString* string = [buffer.filePath lastPathComponent];
    if (column == 0) {
        if ([buffer stateSet:kCEESourceBufferStateModified])
            string = [string stringByAppendingString:@" *"];
        if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
            string = [string stringByAppendingString:@" (delete)"];
    }
    cellView.title.stringValue = string;
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    CymbolsDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}


- (void)highlightSelectionInHistoryTable {    
    NSIndexSet *indexSet = nil;
    CEEBufferReference* reference = [_port currentReference];
    NSArray* references = [_port references];
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
    [_port presentHistory:_port.references[_historyTable.selectedRow]];
}

@end
