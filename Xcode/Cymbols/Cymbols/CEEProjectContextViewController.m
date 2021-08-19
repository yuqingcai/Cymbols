//
//  CEEProjectContextViewController.m
//  Cymbols
//
//  Created by qing on 2020/8/26.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEStyleManager.h"
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
    CEESession* session = [[delegate currentProject] currentSession];
    _project = [delegate currentProject];
    _selectedSymbolRef = NULL;
    [_symbolTable reloadData];
    
    if (session &&
        session.sourceContext &&
        cee_list_length(session.sourceContext->symbols))
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
    CEESession* session = [[delegate currentProject] currentSession];
    if (!session || !session.sourceContext || !session.sourceContext->symbols)
        return 0;
    return cee_list_length(session.sourceContext->symbols);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    AppDelegate* delegate = [NSApp delegate];
    CEESession* session = [[delegate currentProject] currentSession];
    if (!session || !session.sourceContext || !session.sourceContext->symbols)
        return @"Location info";
        
    CEESourceSymbol* symbol = cee_list_nth_data(session.sourceContext->symbols, 0);
    NSString* name = [NSString stringWithUTF8String:symbol->name];
    cee_ulong length = cee_list_length(session.sourceContext->symbols);
    return [NSString stringWithFormat:@"\"%@\" found at %lu locations", name, length];
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    AppDelegate* delegate = [NSApp delegate];
    CEESession* session = [[delegate currentProject] currentSession];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
    CEESourceSymbol* symbol = cee_list_nth_data(session.sourceContext->symbols, (cee_int)row);
    NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
    cellView.text.stringValue = [NSString stringWithFormat:@"%ld %@ - line %d", row, [filePath lastPathComponent], symbol->line_no + 1];
    [cellView.icon setImage:[styleManager iconFromSymbol:symbol]];
    return cellView;
}

- (CGFloat)tableView:(nonnull CEETableView *)tableView indentForRow:(NSInteger)row {
    return 0.0;
}

- (IBAction)selectRow:sender {
    @autoreleasepool {
        AppDelegate* delegate = [NSApp delegate];
        CEESession* session = [[delegate currentProject] currentSession];
        
        if (!_symbolTable.selectedRowIndexes || _symbolTable.selectedRow == -1)
            return;
        
        CEESourceSymbol* symbol = cee_list_nth_data(session.sourceContext->symbols,
                                                    (cee_int)_symbolTable.selectedRow);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        if (_contextSourceBuffer)
            [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
        _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
        [_editViewController setBuffer:_contextSourceBuffer];
        [_editViewController highlight:symbol->ranges];
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
        CEESession* session = [[delegate currentProject] currentSession];
        
        CEESourceSymbol* symbol = cee_list_nth_data(session.sourceContext->symbols,
                                                    (cee_int)_symbolTable.selectedRow);
        NSString* filePath = [NSString stringWithUTF8String:symbol->file_path];
        if (_contextSourceBuffer)
            [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
        _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
        [_editViewController setBuffer:_contextSourceBuffer];
        [_editViewController highlight:symbol->ranges];
        [_titlebar setTitle:filePath];
    }
}

- (IBAction)jumpToSymbol:(id)sender {
    AppDelegate* delegate = [NSApp delegate];
    CEESession* session = [[delegate currentProject] currentSession];
    if (!session || !session.sourceContext || !session.sourceContext->symbols)
        return;
    _selectedSymbolRef = cee_list_nth_data(session.sourceContext->symbols,
                                        (cee_int)_symbolTable.selectedRow);
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)cancel:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

@end

