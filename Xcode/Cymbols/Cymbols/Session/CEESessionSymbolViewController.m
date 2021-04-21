//
//  CEESessionSymbolViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEESessionSymbolViewController.h"
#import "CEEImageTextTableCellView.h"
#import "CEEStyleManager.h"

@interface CEESessionSymbolViewController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETableView *symbolTable;
@property (weak) IBOutlet CEETextField *filterInput;
@property (strong) CEESessionPort* port;
@property (strong) CEESourceBuffer* buffer;
@property (strong) NSString* filter;
@property CEEList* symbolWrappers;
@property (strong) NSTimer* updateSymbolsTimer;
@property BOOL shouldUpdate;
@end

@implementation CEESessionSymbolViewController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_titlebar setTitle:@"Symbols"];
    [_symbolTable setNibNameOfCellView:@"TableCellViews"];
    [_symbolTable setDataSource:self];
    [_symbolTable setDelegate:self];
    [_symbolTable setTarget:self];
    [_symbolTable setAction:@selector(selectRow:)];
    [_symbolTable setEnableDrawHeader:NO];
    [_symbolTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_filterInput setDelegate:self];
    
    _shouldUpdate = NO;
    _updateSymbolsTimer = [NSTimer timerWithTimeInterval:0.5 target:self selector:@selector(updateSymbols:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_updateSymbolsTimer forMode:NSRunLoopCommonModes];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activeSourceBufferResponse:) name:CEENotificationSessionPortActiveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activePortResponse:) name:CEENotificationSessionActivePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deletePortResponse:) name:CEENotificationSessionDeletePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(presentHistoryResponse:) name:CEENotificationSessionPortPresentHistory object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferReloadResponse:) name:CEENotificationSourceBufferReload object:nil];
}

- (void)updateSymbols:(NSTimer *)timer {
    if (_shouldUpdate)
        [self presentSymbols];
    
    _shouldUpdate = NO;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    if (_symbolWrappers)
        cee_list_free_full(_symbolWrappers, cee_source_symbol_wrapper_free);
    _symbolWrappers = NULL;
    
    if (_updateSymbolsTimer) {
        [_updateSymbolsTimer invalidate];
        _updateSymbolsTimer = nil;
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_symbolWrappers)
        return 0;
    return cee_list_length(_symbolWrappers);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"SymbolName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
    CEESourceSymbolWrapper* wrapper = cee_list_nth_data(_symbolWrappers, (cee_uint)row);
    CEESourceSymbol* symbol = wrapper->symbol_ref;
    NSString* string = [NSString stringWithUTF8String:symbol->name];
    
    cellView.text.stringValue = string;
    [cellView.icon setImage:[styleManager iconFromSymbol:symbol]];
    return cellView;
}

- (CGFloat)tableView:(CEETableView *)tableView indentForRow:(NSInteger)row {
    CEESourceSymbolWrapper* wrapper = cee_list_nth_data(_symbolWrappers, (cee_uint)row);
    return wrapper->level - 1;
}

- (IBAction)selectRow:sender {
    if (!_symbolTable.selectedRowIndexes || !_symbolWrappers)
        return;
    
    CEESourceSymbolWrapper* wrapper = cee_list_nth_data(_symbolWrappers,
                                                        (cee_uint)_symbolTable.selectedRow);
    [_session jumpToSymbol:wrapper->symbol_ref inPort:_session.activedPort];
}

- (BOOL)deserialize:(NSDictionary *)dict {
    _port = [_session activedPort];
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];
    return YES;
}

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {
    if (notification.object != _buffer)
        return;
    if ([_buffer stateSet:kCEESourceBufferStateShouldSyncToFile])
        _shouldUpdate = YES;
}

- (void)openSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port != _port)
        return;
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];
}

- (void)activeSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port != _port)
        return;
    
    if (_buffer == [_port activedSourceBuffer])
        return;
    
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];
}

- (void)activePortResponse:(NSNotification*)notification {
    if ([notification object] != self.session)
        return;
    
    _port = [_session activedPort];
    
    if (_buffer == [_port activedSourceBuffer])
        return;
    
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];
}

- (void)deletePortResponse:(NSNotification*)notification {
    if ([notification object] != self.session)
        return;
    
    _port = [_session activedPort];
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    
    _port = [_session activedPort];
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];    
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    
    _port = [_session activedPort];
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];    
}

- (void)presentHistoryResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port != _port)
        return;
    
    _buffer = [_port activedSourceBuffer];
    [self presentSymbols];
}

- (void)sourceBufferReloadResponse:(NSNotification*)notification {
    if (notification.object != _buffer)
        return;
    
    [self presentSymbols];
}

- (void)presentSymbols {
    if (_symbolWrappers)
        cee_list_free_full(_symbolWrappers, cee_source_symbol_wrapper_free);
    _symbolWrappers = NULL;
    
    if (_buffer)
        _symbolWrappers = cee_source_symbol_wrappers_copy_with_condition(_buffer.symbols,
                                                                         [_filter UTF8String]);
    [_symbolTable reloadData];
}

- (void)textViewTextChanged:(CEETextView *)textView {
    if (textView == _filterInput) {
        _filter = [_filterInput.stringValue stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        [self presentSymbols];
    }
}

@end
