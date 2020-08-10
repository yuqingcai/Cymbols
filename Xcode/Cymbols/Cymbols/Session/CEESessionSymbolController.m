//
//  CEESessionSymbolController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionSymbolController.h"
#import "CEETitleView.h"
#import "CEEImageView.h"

@interface CEESessionSymbolController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETableView *symbolTable;
@property (strong) CEESourceBuffer* buffer;
@end

@implementation CEESessionSymbolController

@synthesize session = _session;

- (void)viewDidLoad {
    //[super viewDidLoad];
    //[_titlebar setTitle:@"Symbols"];
    //[_symbolTable setDataSource:self];
    //[_symbolTable setDelegate:self];
    //[_symbolTable setTarget:self];
    //[_symbolTable setAction:@selector(selectRow:)];
    
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortOpenSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortPresentSourceBufferResponse:) name:CEENotificationSessionPortPresentSourceBuffer object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionActivePortResponse:) name:CEENotificationSessionActivePort object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferSymbolSelectedResponse:) name:CEENotificationSourceBufferSymbolSelected object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    return 0;
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    return nil;
}

- (CEEView*)tableView:(CEETableView*)tableView cellViewWithIdentifier:(NSString*)identifier {
    return nil;
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"";
}

- (IBAction)selectRow:sender {
    if (!_symbolTable.selectedRowIndexes)
        return;
    
    CEESymbolWrapper* wrapper = [_buffer symbolWrapperByIndex:_symbolTable.selectedRow];
    [_buffer highlightSymbol:wrapper->symbol_ref];
}

- (void)deserialize:(NSDictionary *)dict {
    _buffer = [[_session.activedPort currentReference] buffer];
    //[_symbolTable reloadData];
}

- (void)sourceBufferStateChangedResponse:(NSNotification*)notification {
    if (notification.object != _buffer)
        return;
    [self presentSymbols];
}

- (void)sessionPortOpenSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    [self presentSymbols];
}

- (void)sessionPortPresentSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    [self presentSymbols];
}

- (void)sessionActivePortResponse:(NSNotification*)notification {
    if ([notification object] != self.session)
        return;
    [self presentSymbols];
}

- (void)sourceBufferSymbolSelectedResponse:(NSNotification*)notification {
    NSIndexSet *indexSet = nil;
    NSInteger index = [_buffer selectedSymbolIndex];
    indexSet = [NSIndexSet indexSetWithIndex:index];
    //[_symbolTable selectRowIndexes:indexSet byExtendingSelection:NO];
    //[_symbolTable scrollRowToVisible:[indexSet firstIndex]];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    [self presentSymbols];    
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    [self presentSymbols];    
}

- (void)presentSymbols {
    _buffer = [[_session.activedPort currentReference] buffer];
    //[_symbolTable reloadData];
}

@end
