//
//  CEESessionSymbolController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEESessionSymbolController.h"
#import "CEESymbolCellView.h"


@interface CEESessionSymbolController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETableView *symbolTable;
@property (strong) CEESourceBuffer* buffer;
@end

@implementation CEESessionSymbolController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_titlebar setTitle:@"Symbols"];
    [_symbolTable setDataSource:self];
    [_symbolTable setDelegate:self];
    [_symbolTable setTarget:self];
    [_symbolTable setAction:@selector(selectRow:)];
    [_symbolTable setEnableDrawHeader:NO];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortOpenSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortActiveSourceBufferResponse:) name:CEENotificationSessionPortActiveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionActivePortResponse:) name:CEENotificationSessionActivePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_buffer || !_buffer.symbol_wrappers)
        return 0;
    return cee_list_length(_buffer.symbol_wrappers);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"SymbolName";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEESymbolCellView* cellView = [tableView makeViewWithIdentifier:@"IDSymbolCellView"];
    CEESymbolWrapper* wrapper = cee_list_nth_data(_buffer.symbol_wrappers, (cee_uint)row);
    CEESourceSymbol* symbol = wrapper->symbol_ref;
    NSString* string = [NSString stringWithUTF8String:symbol->descriptor];
    
    switch (symbol->type) {
        case kCEESourceSymbolTypeFunctionDefinition:
            string = [string stringByAppendingString:@"()"];
            break;
        case kCEESourceSymbolTypeFunctionDeclaration:
            string = [string stringByAppendingString:@"()"];
            break;
        case kCEESourceSymbolTypeTemplateDeclaration:
            string = [string stringByAppendingString:@"<>"];
            break;
        default:
            break;
    }
    cellView.title.stringValue = string;
    [cellView.icon setImage:[styleManager symbolIconFromSymbolType:symbol->type]];
    [cellView.leading setConstant:wrapper->level*12];
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (IBAction)selectRow:sender {
    if (!_symbolTable.selectedRowIndexes || !_buffer.symbol_wrappers)
        return;
    
    CEESymbolWrapper* wrapper = cee_list_nth_data(_buffer.symbol_wrappers, 
                                                  (cee_uint)_symbolTable.selectedRow);
    
    [_session.activedPort setActivedSourceSymbol:wrapper->symbol_ref];
}

- (void)deserialize:(NSDictionary *)dict {
    _buffer = [[_session.activedPort currentBufferReference] buffer];
    [_symbolTable reloadData];
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

- (void)sessionPortActiveSourceBufferResponse:(NSNotification*)notification {
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
    //NSIndexSet *indexSet = nil;
    //NSInteger index = [_buffer selectedSymbolIndex];
    //indexSet = [NSIndexSet indexSetWithIndex:index];
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
    _buffer = [[_session.activedPort currentBufferReference] buffer];
    [_symbolTable reloadData];
}

@end
