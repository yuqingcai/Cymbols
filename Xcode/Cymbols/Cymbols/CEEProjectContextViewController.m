//
//  CEEProjectContextViewController.m
//  Cymbols
//
//  Created by qing on 2020/8/26.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEProjectContextViewController.h"
#import "CEEImageTextTableCellView.h"
#import "CEEEditViewController.h"
#import "CEETextLabel.h"
#import "CEETitleView.h"
#import "CEEImageView.h"

@interface CEEProjectContextViewController ()
@property (strong) CEEProject* project;
@property (weak) IBOutlet CEETableView *symbolTable;
@property (weak) IBOutlet CEETitleView *titlebar;
@property (strong) CEEEditViewController *editViewController;
@property (weak) IBOutlet CEEView *sourceContentView;
@property (strong) CEESourceBuffer* contextSourceBuffer;

@end

@implementation CEEProjectContextViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [_symbolTable setNibNameOfCellView:@"TableCellViews"];
    [_symbolTable setDataSource:self];
    [_symbolTable setDelegate:self];
    [_symbolTable setTarget:self];
    [_symbolTable setAction:@selector(selectRow:)];
    [_symbolTable setDoubleAction:@selector(jumpToSymbol:)];
    [_symbolTable setEnableDrawHeader:YES];
    
    [_titlebar setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    _editViewController =  [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_editViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceContentView addSubview:_editViewController.view];
    
    NSDictionary *views = @{
                            @"titlebar" : _titlebar,
                            @"editView" : _editViewController.view,
                            };
    
    NSDictionary* metrics = @{
                @"titleHeight" : @(25),
                };
    
    NSArray *constraintsH = nil;
    NSArray *constraintsV = nil;
    
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[editView]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[titlebar]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    
    constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar(==titleHeight)]-0-[editView]-0-|" options:0 metrics:metrics views:views];
    [self.view addConstraints:constraintsV];
    
    [_editViewController setEditable:NO];
    [_editViewController setIntelligence:NO];
    [_editViewController setWrap:YES];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [_editViewController setBuffer:nil];
    [_titlebar setTitle:@""];
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
    _project = [delegate currentProject];
    _symbolIndex = -1;
    [_symbolTable reloadData];
    
    if (activedPort &&
        activedPort.source_context &&
        cee_list_length(activedPort.source_context->symbols))
        [self selectSymbolAtIndex:0];
}
- (void)dealloc {
    AppDelegate* delegate = [NSApp delegate];
    if (_contextSourceBuffer)
        [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
    _contextSourceBuffer = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
    if (!activedPort || !activedPort.source_context || !activedPort.source_context->symbols)
        return 0;
    return cee_list_length(activedPort.source_context->symbols);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
    if (!activedPort || !activedPort.source_context || !activedPort.source_context->symbols)
        return @"Location info";
        
    CEESourceSymbol* symbol = cee_list_nth_data(activedPort.source_context->symbols, 0);
    NSString* name = [NSString stringWithUTF8String:symbol->name];
    cee_ulong length = cee_list_length(activedPort.source_context->symbols);
    return [NSString stringWithFormat:@"\"%@\" found at %lu locations", name, length];
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
    CEESourceSymbol* symbol = cee_list_nth_data(activedPort.source_context->symbols, (cee_int)row);
    NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
    cellView.text.stringValue = [NSString stringWithFormat:@"%ld %@ - line %d", row, [filePath lastPathComponent], symbol->line_no + 1];
    [cellView.icon setImage:[styleManager symbolIconFromSymbolType:symbol->type]];
    return cellView;
}

- (CGFloat)tableView:(nonnull CEETableView *)tableView indentForRow:(NSInteger)row {
    return 0.0;
}

- (IBAction)selectRow:sender {
    @autoreleasepool {
        AppDelegate* delegate = [NSApp delegate];
        CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
        
        if (!_symbolTable.selectedRowIndexes || _symbolTable.selectedRow == -1)
            return;
        
        CEESourceSymbol* symbol = cee_list_nth_data(activedPort.source_context->symbols,
                                                    (cee_int)_symbolTable.selectedRow);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        if (_contextSourceBuffer)
            [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
        _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
        [_editViewController setBuffer:_contextSourceBuffer];
        [_editViewController highlightRanges:symbol->ranges];
        [_titlebar setTitle:filePath];
        return;
    }
}

- (void)selectSymbolAtIndex:(NSInteger)index {
    @autoreleasepool {
        NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:index];
        [_symbolTable selectRowIndexes:indexSet byExtendingSelection:NO];
        [_symbolTable scrollRowToVisible:[indexSet firstIndex]];
                
        AppDelegate* delegate = [NSApp delegate];
        CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
        
        CEESourceSymbol* symbol = cee_list_nth_data(activedPort.source_context->symbols,
                                                    (cee_int)_symbolTable.selectedRow);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        if (_contextSourceBuffer)
            [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
        _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
        [_editViewController setBuffer:_contextSourceBuffer];
        [_editViewController highlightRanges:symbol->ranges];
        [_titlebar setTitle:filePath];
    }
}

- (IBAction)jumpToSymbol:(id)sender {
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
    if (!activedPort || !activedPort.source_context || !activedPort.source_context->symbols)
        return;
    _symbolIndex = _symbolTable.selectedRow;
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)cancel:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

@end

