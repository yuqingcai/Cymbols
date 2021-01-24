//
//  CEESessionReferenceController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEETitleView.h"
#import "CEESessionReferenceController.h"
#import "CEEFileNameCellView.h"
#import "CEESourceBufferManagerViewController.h"

@interface CEESessionReferenceController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) IBOutlet CEESessionReferenceTableView *referenceTable;
@property (strong) NSArray* buffers;
@property (strong) NSWindowController* sourceBufferManagerWindowController;
@end

@implementation CEESessionReferenceController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_titlebar setTitle:@"Files Opened"];
    [_referenceTable setNibNameOfCellView:@"TableCellViews"];
    [_referenceTable setDataSource:self];
    [_referenceTable setDelegate:self];
    [_referenceTable setNumberOfColumns:1];
    [_referenceTable setTarget:self];
    [_referenceTable setAction:@selector(selectRow:)];
    [_referenceTable setAllowsMultipleSelection:NO];
    [_referenceTable setEnableDrawHeader:NO];
    [_referenceTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveSourceBufferResponse:) name:CEENotificationSessionPortSaveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activePortResponse:) name:CEENotificationSessionActivePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deletePortResponse:) name:CEENotificationSessionDeletePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(presentHistoryResponse:) name:CEENotificationSessionPortPresentHistory object:nil];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_session)
        return 0;
    
    _buffers = [[_session activedPort] openedSourceBuffers];
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
    NSString* string = nil;
    
    string = [buffer.filePath lastPathComponent];
        
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

- (void)highlightSelectionInReferenceTable {
    if (!_buffers)
        return;
    
    CEESessionPort* port = _session.activedPort;
    NSIndexSet *indexSet = nil;
    CEESourceBuffer* activedSourceBuffer = [port activedSourceBuffer];
    for (NSInteger i = 0; i < _buffers.count; i ++) {
        if (_buffers[i] == activedSourceBuffer) {
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

- (IBAction)cleanSymbols:(id)sender {
    if (!_referenceTable.selectedRowIndexes)
        return;
    
    NSInteger selectedRow = _referenceTable.selectedRow;
    if (selectedRow == -1)
        return ;
    
    CEESourceBuffer* buffer = _buffers[selectedRow];
    cee_database_symbols_delete_by_filepath(_session.project.database,
                                            [buffer.filePath UTF8String]);
}

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)saveSourceBufferResponse:(NSNotification*)notification {
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)openSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)presentHistoryResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)activePortResponse:(NSNotification*)notification {
    if (notification.object != _session)
        return;
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)deletePortResponse:(NSNotification*)notification {
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

- (IBAction)close:(id)sender {
    NSInteger clickedRow = [_referenceTable clickedRow];
    if (clickedRow == -1)
        return ;
    
    CEESourceBuffer* buffer = _buffers[clickedRow];
    [self closeBuffers:@[buffer]];
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (IBAction)closeOthers:(id)sender {
    NSInteger clickedRow = [_referenceTable clickedRow];
    if (clickedRow == -1)
        return ;
    
    CEESourceBuffer* selected = _buffers[clickedRow];
    NSMutableArray* buffers = [[NSMutableArray alloc] init];
    for (CEESourceBuffer* buffer in _buffers) {
        if (buffer != selected)
            [buffers addObject:buffer];
    }
    [self closeBuffers:buffers];
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (IBAction)closeOthersAbove:(id)sender {
    NSInteger clickedRow = [_referenceTable clickedRow];
    if (clickedRow == -1 || clickedRow == 0)
        return ;
    
    NSMutableArray* buffers = [[NSMutableArray alloc] init];
    for (NSInteger i = 0; i < clickedRow; i ++)
        [buffers addObject:_buffers[i]];
    
    [self closeBuffers:buffers];
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (IBAction)closeOthersBelow:(id)sender {
    NSInteger clickedRow = [_referenceTable clickedRow];
    if (clickedRow == -1 || clickedRow == _buffers.count - 1)
        return ;
    
    NSMutableArray* buffers = [[NSMutableArray alloc] init];
    for (NSInteger i = clickedRow + 1; i < _buffers.count; i ++)
        [buffers addObject:_buffers[i]];
    
    [self closeBuffers:buffers];
    [_referenceTable reloadData];
    [self highlightSelectionInReferenceTable];
}

- (void)closeBuffers:(NSArray*)buffers {
    __block BOOL shouldClose = YES;
    NSMutableArray* syncBuffers = nil;
    for (CEESourceBuffer* buffer in buffers) {
        if ([buffer stateSet:kCEESourceBufferStateShouldSyncWhenClose]) {
            if (!syncBuffers)
                syncBuffers = [[NSMutableArray alloc] init];
            [syncBuffers addObject:buffer];
        }
    }
    
    if (syncBuffers) {
        if (!_sourceBufferManagerWindowController)
            _sourceBufferManagerWindowController = [[NSStoryboard storyboardWithName:@"SourceBufferManager" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceBufferManagerWindowController"];
        CEESourceBufferManagerViewController* controller = (CEESourceBufferManagerViewController*)_sourceBufferManagerWindowController.contentViewController;
        [controller setModifiedSourceBuffers:syncBuffers];
        [self.view.window beginSheet:_sourceBufferManagerWindowController.window completionHandler:(^(NSInteger result) {
            if (result == NSModalResponseCancel)
                shouldClose = NO;
            else
                shouldClose = YES;
            [NSApp stopModalWithCode:result];
            [controller setModifiedSourceBuffers:nil];
        })];
        [NSApp runModalForWindow:self.view.window];
    }
    
    if (shouldClose)
        [[self.session activedPort] closeSourceBuffers:buffers];
}

- (void)menuWillOpen:(NSMenu *)menu {
    [_referenceTable highlightRowRect:[_referenceTable clickedRow] enable:YES];
}

- (void)menuDidClose:(NSMenu *)menu {
    [_referenceTable highlightRowRect:[_referenceTable clickedRow] enable:NO];
}
@end
