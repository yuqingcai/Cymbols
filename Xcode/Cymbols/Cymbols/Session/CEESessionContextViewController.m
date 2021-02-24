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
@property CEEList* symbols;
@property BOOL adjustSplitView;
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
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortCreateContextResponse:) name:CEENotificationSessionPortCreateContext object:nil];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    if (_symbols)
        cee_list_free(_symbols);
    _symbols = NULL;
}

- (void)viewWillAppear {
    [super viewWillAppear];

    if (!_splitView)
        [self createSplitView];

    [self createContextSymbols];

    if (!_symbols)
        return;

    [_symbolTable reloadData];

    CEESourceSymbol* symbol = cee_list_nth_data(_symbols, 0);
    [self presentContextBufferWithSymbol:symbol];
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
    constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[splitView]-0-|" options:0 metrics:nil views:views];
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
        @"detailTitlebar" : _detailTitlebar,
        @"monitorView" : _monitor.view,
    };
    
    [_detailView addSubview:_detailTitlebar];
    [_detailView addSubview:_monitor.view];
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[detailTitlebar]-0-|" options:0 metrics:nil views:views];
    [_detailView addConstraints:constraintsH];
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[monitorView]-0-|" options:0 metrics:nil views:views];
    [_detailView addConstraints:constraintsH];
    constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[detailTitlebar(==25.0)]-0-[monitorView]-0-|" options:0 metrics:nil views:views];
    [_detailView addConstraints:constraintsV];
    
    [_splitView addSubview:_symbolTable];
    [_splitView addSubview:_detailView];
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
    NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    CEESourceBuffer* buffer = [sourceBufferManager openSourceBufferWithFilePath:filePath andOption:kCEESourceBufferOpenOptionIndependent];
    if (!buffer)
        return;
    
    cee_source_buffer_parse(buffer, kCEESourceBufferParserOptionCreateSymbolWrapper);
    [_monitor setBuffer:buffer];
    CEEList* ranges = cee_ranges_from_string(symbol->locations);
    if (ranges) {
        [_monitor highlightRanges:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
    [sourceBufferManager closeSourceBuffer:buffer];

    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [_titlebar setTitle:[NSString stringWithFormat:@"Context of \"%s\"", symbol->name, nil]];
    if (_session.project)
        [_detailTitlebar setTitle:[_session.project shortFilePath:filePath]];
    else
        [_detailTitlebar setTitle:filePath];
    [_detailTitlebar setIcon:[styleManager filetypeIconFromFilePath:filePath]];
}

- (void)sessionPortCreateContextResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;

    [self createContextSymbols];

    if (!_symbols)
        return;
    
    [_symbolTable reloadData];
        
    CEESourceSymbol* symbol = cee_list_nth_data(_symbols, 0);
    [self presentContextBufferWithSymbol:symbol];
    
}

- (void)createContextSymbols {
    if (_symbols)
        cee_list_free(_symbols);
    _symbols = NULL;
    
    if (!_session.activedPort.context)
        return;
    
    CEEList* p = NULL;
    
    p = _session.activedPort.context;
    while (p) {
        if (cee_source_symbol_is_definition(p->data))
            _symbols = cee_list_prepend(_symbols, p->data);
        p = p->next;
    }
    
    if (!_symbols) {
        p = _session.activedPort.context;
        while (p) {
            _symbols = cee_list_prepend(_symbols, p->data);
            p = p->next;
        }
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_symbols)
        return 0;
    return cee_list_length(_symbols);
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
    
    if (column == 0) {
        CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
        CEESourceSymbol* symbol = cee_list_nth_data(_symbols, (cee_int)row);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        cellView.text.stringValue = [NSString stringWithFormat:@"%@", [filePath lastPathComponent]];
        [cellView.icon setImage:[styleManager symbolIconFromSymbolType:symbol->type]];
        return cellView;
    }
    else if (column == 1) {
        CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
        CEESourceSymbol* symbol = cee_list_nth_data(_symbols, (cee_int)row);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        if (_session.project)
            cellView.text.stringValue = [_session.project shortFilePath:filePath];
        else
            cellView.text.stringValue = filePath;
        [cellView.icon setImage:[styleManager filetypeIconFromFilePath:filePath]];
        return cellView;
    }
    return nil;
}

- (IBAction)selectRow:(id)sender {
    if (!_symbolTable.selectedRowIndexes || _symbolTable.selectedRow == -1)
        return;
    
    CEESourceSymbol* symbol = cee_list_nth_data(_symbols, (cee_int)_symbolTable.selectedRow);
    [self presentContextBufferWithSymbol:symbol];
}

@end
