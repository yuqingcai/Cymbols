//
//  CEEProjectContextViewController.m
//  Cymbols
//
//  Created by qing on 2020/8/26.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEProjectContextViewController.h"
#import "CEESymbolCellView.h"
#import "CEEEditViewController.h"
#import "CEETextLabel.h"
#import "CEETitleView.h"
#import "CEEImageView.h"

@interface CEEProjectContextViewController ()
@property (weak) IBOutlet CEETableView *symbolTable;
@property (weak) IBOutlet CEETitleView *titlebar;
@property (strong) CEEEditViewController *editViewController;
@property (weak) IBOutlet CEEView *sourceContentView;
@property CEEList* symbols;

@end

@implementation CEEProjectContextViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    [_symbolTable setDataSource:self];
    [_symbolTable setDelegate:self];
    [_symbolTable setTarget:self];
    [_symbolTable setAction:@selector(selectRow:)];
    [_symbolTable setEnableDrawHeader:YES];
    
    _editViewController =  [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_editViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceContentView addSubview:_editViewController.view];
    [_editViewController setIntelligence:YES];
    
    NSDictionary *views = @{
                            @"titlebar" : _titlebar,
                            @"editView" : _editViewController.view,
                            };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[editView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[editView]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [_editViewController setEditable:NO];
    [_editViewController setIntelligence:NO];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* activedPort = [[[delegate currentProject] currentSession] activedPort];
    _symbols = activedPort.context;
    
    if (!_symbols)
        return;
    
    [_symbolTable reloadData];
    [self selectRow:0];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_symbols)
        return 0;
    return cee_list_length(_symbols);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (!_symbols)
        return @"Location info";
    
    CEESourceSymbol* symbol = cee_list_nth_data(_symbols, 0);
    NSString* descriptor = [NSString stringWithUTF8String:symbol->descriptor];
    return [NSString stringWithFormat:@"\"%@\" found at %d locations", descriptor, cee_list_length(_symbols)];
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEESymbolCellView* cellView = [tableView makeViewWithIdentifier:@"IDSymbolCellView"];
    CEESourceSymbol* symbol = cee_list_nth_data(_symbols, (cee_int)row);
    NSString* descriptor = [NSString stringWithUTF8String:symbol->descriptor];
    NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
    
    cellView.title.stringValue = [NSString stringWithFormat:@"%ld %@ - line %ld", row, [filePath lastPathComponent], 0];
    [cellView.icon setImage:[styleManager symbolIconFromSymbolType:symbol->type]];
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (IBAction)selectRow:sender {
    if (!_symbolTable.selectedRowIndexes || _symbolTable.selectedRow == -1)
        return;
    CEESourceSymbol* symbol = cee_list_nth_data(_symbols, (cee_int)_symbolTable.selectedRow);
    NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
    CEESourceBuffer* buffer = [[CEESourceBuffer alloc] initWithFilePath:filePath];
    cee_source_buffer_parse(buffer, kCEESourceBufferParserOptionCreateSymbolWrapper);
    [_editViewController setBuffer:buffer];
    CEEList* ranges = cee_ranges_from_string(symbol->locations);
    if (ranges) {
        [_editViewController highlightRanges:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
    [_titlebar setTitle:filePath];
    return;
}

- (IBAction)select:(id)sender {
    if (_symbols)
        _selectedSymbol = cee_list_nth_data(_symbols, (cee_int)_symbolTable.selectedRow);
    
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)cancel:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

@end

