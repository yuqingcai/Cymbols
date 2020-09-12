//
//  CEESessionReferenceController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEESessionReferenceController.h"
#import "CEEFileNameCellView.h"

@interface CEESessionReferenceController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) IBOutlet CEETableView *referenceTable;
@property (strong) NSArray* buffers;
@end

@implementation CEESessionReferenceController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_titlebar setTitle:@"Opened Files"];
    [_referenceTable setDataSource:self];
    [_referenceTable setDelegate:self];
    [_referenceTable setColumns:1];
    [_referenceTable setTarget:self];
    [_referenceTable setAction:@selector(selectRow:)];
    [_referenceTable setAllowsMultipleSelection:NO];
    [_referenceTable setEnableDrawHeader:NO];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortOpenSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionActivePortResponse:) name:CEENotificationSessionActivePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionDeletePortResponse:) name:CEENotificationSessionDeletePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_session)
        return 0;
    
    _buffers = [_session registeredSourceBuffers];
    if (_buffers)
        return _buffers.count;
    return 0;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FileName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEESourceBuffer* buffer = _buffers[row];
    CEEFileNameCellView* cellView = [_referenceTable makeViewWithIdentifier:@"IDFileNameCellView"];
    NSString* string = [buffer.filePath lastPathComponent];
    if (column == 0) {
        if ([buffer stateSet:kCEESourceBufferStateModified])
            string = [string stringByAppendingString:@" *"];
        if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
            string = [string stringByAppendingString:@" (delete)"];
    }
    cellView.title.stringValue = string;
    [cellView.icon setImage:[styleManager filetypeIconFromFileName:[buffer.filePath lastPathComponent]]];
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (void)highlightSelectionInReferenceTable {
    CEESessionPort* port = _session.activedPort;
    NSIndexSet *indexSet = nil;
    CEEBufferReference* reference = [port currentBufferReference];
    if (!_buffers)
        return;
    
    for (NSInteger i = 0; i < _buffers.count; i ++) {
        if (reference.buffer == _buffers[i]) {
            indexSet = [NSIndexSet indexSetWithIndex:i];
            break;
        }
    }
    [_referenceTable selectRowIndexes:indexSet byExtendingSelection:NO];
    [_referenceTable scrollRowToVisible:[indexSet firstIndex]];
}

- (void)deserialize:(NSDictionary *)dict {
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (IBAction)selectRow:sender {
    if (!_referenceTable.selectedRowIndexes)
        return;
    NSInteger selectedRow = _referenceTable.selectedRow;
    if (selectedRow == -1)
        return ;
    
    CEESourceBuffer* buffer = _buffers[selectedRow];
    [_session.activedPort setActivedSourceBuffer:buffer];
}

- (void)sourceBufferStateChangedResponse:(NSNotification*)notification {
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)sessionPortOpenSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)sessionActivePortResponse:(NSNotification*)notification {
    if (notification.object != _session)
        return;
    [self highlightSelectionInReferenceTable];
}

- (void)sessionDeletePortResponse:(NSNotification*)notification {
    if (notification.object != _session)
        return;
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

@end
