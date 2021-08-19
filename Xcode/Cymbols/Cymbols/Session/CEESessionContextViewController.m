//
//  CEESessionContextViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/1.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEESessionContextViewController.h"
#import "CEEEditViewController.h"
#import "CEESplitView.h"
#import "CEETitlebarButton.h"
#import "CEEImageTextTableCellView.h"
#import "CEEImageView.h"
#import "cee_symbol.h"

@interface CEESessionContextViewController ()
@property (weak) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETitlebarButton* toggleButton;
@property (strong) CEESplitView *splitView;
@property (strong) CEETableView *symbolTable;
@property (strong) NSView *detailView;
@property (strong) CEETitleView* detailTitlebar;
@property (strong) CEEEditViewController *monitor;
@property BOOL adjustSplitView;
@property (strong) CEESourceBuffer* contextSourceBuffer;
@end

@implementation CEESessionContextViewController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_titlebar setTitleLeading:5.0];
    [_titlebar setTitleTailing:_toggleButton.frame.size.width];
    [_titlebar setTitle:@"Context"];
    [_titlebar setIcon:[styleManager iconFromName:@"icon_relation_16x16"]];
    _adjustSplitView = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionCreateSourceContextResponse:) name:CEENotificationSessionCreateSourceContext object:nil];
}

- (void)dealloc {
    AppDelegate* delegate = [NSApp delegate];
    if (_contextSourceBuffer)
        [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
    _contextSourceBuffer = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewWillAppear {
    [super viewWillAppear];
    
    if (!_splitView)
        [self createSplitView];
    
    [_symbolTable reloadData];
    [self selectSymbolAtIndex:0];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    if (!_adjustSplitView) {
        [self setHoldingPrioritiesInSplitView:_splitView];
        [self setDividersPositionInSplitView:_splitView];
        [_splitView adjustSubviews];
        _adjustSplitView = YES;
    }
}

- (void)createSplitView {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSArray *constraintsH = nil;
    NSArray *constraintsV = nil;
    
    _splitView = [[CEESplitView alloc] init];
    [_splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_splitView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_splitView setStyleState:kCEEViewStyleStateActived];
    [_splitView setDividerStyle:NSSplitViewDividerStyleThin];
    [_splitView setVertical:NO];
    
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"splitView" : _splitView,
    };
    
    [self.view addSubview:_splitView];
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[splitView]-0-|" options:0 metrics:nil views:views];
    constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[splitView(>=300)]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    _symbolTable = [[CEETableView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_symbolTable setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_symbolTable setNibNameOfCellView:@"TableCellViews"];
    [_symbolTable setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_symbolTable setDelegate:self];
    [_symbolTable setDataSource:self];
    [_symbolTable setNumberOfColumns:2];
    [_symbolTable setEnableDrawHeader:YES];
    [_symbolTable setTarget:self];
    [_symbolTable setAction:@selector(selectRow:)];
    [_symbolTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    
    _detailView = [[CEEView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 100.0, 100.0)];
    [_detailView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    _detailTitlebar = [[CEETitleView alloc] init];
    [_detailTitlebar setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_detailTitlebar setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    _monitor =  [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_monitor.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_monitor setEditable:NO];
    [_monitor setIntelligence:NO];
    [_monitor setWrap:YES];
    
    views = @{
        @"symbolTable" : _symbolTable,
        @"detailTitlebar" : _detailTitlebar,
        @"monitorView" : _monitor.view,
    };
    
    [_detailView addSubview:_detailTitlebar];
    [_detailView addSubview:_monitor.view];
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[detailTitlebar]-0-|" options:0 metrics:nil views:views];
    [_detailView addConstraints:constraintsH];
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[monitorView]-0-|" options:0 metrics:nil views:views];
    [_detailView addConstraints:constraintsH];
    constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[detailTitlebar(==25.0)]-0-[monitorView(>=200.0)]-0-|" options:0 metrics:nil views:views];
    [_detailView addConstraints:constraintsV];
    
    [_splitView addSubview:_symbolTable];
    [_splitView addSubview:_detailView];
    
    constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:[symbolTable(>=100.0)]" options:0 metrics:nil views:views];
    [_symbolTable addConstraints:constraintsV];
    
}

- (void)setDividersPositionInSplitView:(NSSplitView*)splitView {
    CGFloat position;
    CGFloat distance;
    
    CGFloat splitViewWidth = splitView.frame.size.width;
    CGFloat splitViewHeight = splitView.frame.size.height;
    
    if (splitView.vertical)
        distance = (splitViewWidth - (splitView.dividerThickness * (splitView.subviews.count - 1))) / splitView.subviews.count;
    else
        distance = (splitViewHeight - (splitView.dividerThickness * (splitView.subviews.count - 1))) / splitView.subviews.count;
    
    for (NSInteger i = 0; i < splitView.subviews.count - 1; i ++) {
        position = distance + ((distance + splitView.dividerThickness) * i);
        [splitView setPosition:position ofDividerAtIndex:i];
    }
}

- (void)setHoldingPrioritiesInSplitView:(NSSplitView*)splitView {
    for (NSInteger i = 0; i < splitView.subviews.count; i ++)
        [splitView setHoldingPriority:250 - i forSubviewAtIndex:i];
}

- (void)presentContextBufferWithSymbol:(CEESourceSymbol*)symbol {
    if (!symbol)
        return;
    
    NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
    AppDelegate* delegate = [NSApp delegate];
        
    if (_contextSourceBuffer)
        [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
    _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
    if (!_contextSourceBuffer)
        return;
    
    [_monitor setBuffer:_contextSourceBuffer];
    if (symbol->ranges)
        [_monitor highlight:symbol->ranges];
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [_titlebar setTitle:[NSString stringWithFormat:@"Context of \"%s\"", symbol->name, nil]];
    if (_session.project)
        [_detailTitlebar setTitle:[_session.project shortFilePath:filePath]];
    else
        [_detailTitlebar setTitle:filePath];
    [_detailTitlebar setIcon:[styleManager iconFromFilePath:filePath]];
}

- (void)sessionCreateSourceContextResponse:(NSNotification*)notification {
    CEESession* session = notification.object;
    if (session != _session ||
        !_session.sourceContext ||
        !_session.sourceContext->symbols)
        return;
            
    [_symbolTable reloadData];
    [self selectSymbolAtIndex:0];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_session.sourceContext ||
        !_session.sourceContext->symbols)
        return 0;
    return cee_list_length(_session.sourceContext->symbols);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (column == 0)
        return @"File";
    else if (column == 1)
        return @"Path";
    return @"";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEEList* symbols = _session.sourceContext->symbols;
    
    if (column == 0) {
        CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
        CEESourceSymbol* symbol = cee_list_nth_data(symbols, (cee_int)row);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        cellView.text.stringValue = [NSString stringWithFormat:@"%@", [filePath lastPathComponent]];
        [cellView.icon setImage:[styleManager iconFromSymbol:symbol]];
        return cellView;
    }
    else if (column == 1) {
        CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
        CEESourceSymbol* symbol = cee_list_nth_data(symbols, (cee_int)row);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        if (_session.project)
            cellView.text.stringValue = [_session.project shortFilePath:filePath];
        else
            cellView.text.stringValue = filePath;
        [cellView.icon setImage:[styleManager iconFromFilePath:filePath]];
        return cellView;
    }
    return nil;
}

- (IBAction)selectRow:(id)sender {
    if (!_session.sourceContext ||
        !_session.sourceContext->symbols ||
        !_symbolTable.selectedRowIndexes || _symbolTable.selectedRow == -1)
        return;
    
    CEEList* symbols = _session.sourceContext->symbols;
    CEESourceSymbol* symbol = cee_list_nth_data(symbols, (cee_int)_symbolTable.selectedRow);
    [self presentContextBufferWithSymbol:symbol];
}

- (void)selectSymbolAtIndex:(NSInteger)index {
    CEESourceSymbol* symbol = NULL;
    
    if (_session.sourceContext && _session.sourceContext->symbols) {
        NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:index];
        [_symbolTable selectRowIndexes:indexSet byExtendingSelection:NO];
        [_symbolTable scrollRowToVisible:[indexSet firstIndex]];
        symbol = cee_list_nth_data(_session.sourceContext->symbols, (cee_int)_symbolTable.selectedRow);
    }
    
    [self presentContextBufferWithSymbol:symbol];
    
}

@end
