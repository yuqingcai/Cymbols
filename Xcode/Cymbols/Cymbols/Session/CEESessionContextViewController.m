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
#import "CEESymbolCellView.h"
#import "CEEFileNameCellView.h"
#import "CEEFilePathCellView.h"
#import "CEEImageView.h"
#import "cee_symbol.h"

@interface CEESessionContextViewController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) IBOutlet CEEImageView *titleIcon;
@property (strong) CEEEditViewController *editViewController;
@property (strong) CEETableView *contextTable;
@property CEESessionPort* port;
@property CEEList* context_symbols;
@end

@implementation CEESessionContextViewController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    CGFloat leadingOffset = _titleIcon.frame.size.width ;
    [_titlebar setLeadingOffset:leadingOffset];
    [_titlebar setTitle:@"Context"];
    [self createSubviews];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortCreateContextResponse:) name:CEENotificationSessionPortCreateContext object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateViewStyleConfiguration:) name:CEENotificationUserInterfaceStyleUpdate object:nil];
}

- (void)viewWillAppear {
    [super viewWillAppear];
    
    _port = _session.activedPort;
    
    [self createContextSymbols];
    
    if (!_context_symbols)
        return;
    
    if (cee_list_length(_context_symbols) > 1) {
        [self showContextTable];
        [_contextTable reloadData];
    }
    else {
        [self showEditor];
        CEESourceSymbol* symbol = cee_list_nth_data(_context_symbols, 0);
        NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
        [self presentContextBufferWithSymbol:symbol];
        [_titlebar setTitle:filePath];
    }
    
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    if (_context_symbols)
        cee_list_free(_context_symbols);
    _context_symbols = NULL;
}

- (void)createSubviews {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    _contextTable = [[CEETableView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 300, 100.0)];
    [_contextTable setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_contextTable setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_contextTable setDelegate:self];
    [_contextTable setDataSource:self];
    [_contextTable setColumns:2];
    [_contextTable setEnableDrawHeader:YES];
    [_contextTable setTarget:self];
    [_contextTable setAction:@selector(selectRow:)];
    [_contextTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    
    _editViewController =  [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_editViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
        
    [_editViewController setEditable:NO];
    [_editViewController setIntelligence:NO];
    [_editViewController setWrap:YES];
        
}

- (void)showContextTable {
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"contextTable" : _contextTable,
    };
    
    if (_contextTable.superview)
        return;
    
    if (_editViewController.view.superview) {
        NSMutableArray* constraints = nil;       
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _editViewController.view.superview.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == _editViewController.view || 
                constraint.secondItem == _editViewController.view) {
                [_editViewController.view.superview removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _titlebar.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == _editViewController.view || 
                constraint.secondItem == _editViewController.view) {
                [_titlebar removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _editViewController.view.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == _editViewController.view.superview || 
                constraint.secondItem == _editViewController.view.superview || 
                constraint.firstItem == _titlebar || 
                constraint.secondItem == _titlebar) {
                [_editViewController.view removeConstraint:constraint];
            }
        }
        [_editViewController.view removeFromSuperview];
    }
    
    [self.view addSubview:_contextTable];
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[contextTable]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[contextTable]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
}

- (void)showEditor {
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"editView" : _editViewController.view,
    };
    
    if (_editViewController.view.superview)
        return;
    
    if (_contextTable.superview) {        
        NSMutableArray* constraints = nil;       
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _contextTable.superview.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == _contextTable || 
                constraint.secondItem == _contextTable) {
                [_contextTable.superview removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _titlebar.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == _contextTable || 
                constraint.secondItem == _contextTable) {
                [_titlebar removeConstraint:constraint];
            }
        }
        
        constraints = [[NSMutableArray alloc] init];
        for (NSLayoutConstraint *constraint in _contextTable.constraints)
            [constraints addObject:constraint];
        for (NSLayoutConstraint *constraint in constraints) {
            if (constraint.firstItem == _contextTable.superview || 
                constraint.secondItem == _contextTable.superview ||
                constraint.firstItem == _titlebar ||
                constraint.secondItem == _titlebar) {
                [_contextTable removeConstraint:constraint];
            }
        }
        [_contextTable removeFromSuperview];
    }
    
    [self.view addSubview:_editViewController.view];
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[editView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[editView]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
}

- (void)presentContextBufferWithSymbol:(CEESourceSymbol*)symbol {
    NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
    CEESourceBuffer* buffer = nil;
    // fucking app sandbox make this code so tedious!!!
    if (access([filePath UTF8String], R_OK) != 0) {
        NSArray* bookmarks = [self.session.project getSecurityBookmarksWithFilePaths:@[filePath]];
        if (bookmarks) {
            [self.session.project startAccessSecurityScopedResourcesWithBookmarks:bookmarks];
            buffer = [[CEESourceBuffer alloc] initWithFilePath:filePath];
            [self.session.project stopAccessSecurityScopedResourcesWithBookmarks:bookmarks];
        }
    }
    else {
        buffer = [[CEESourceBuffer alloc] initWithFilePath:filePath];
    }
    
    if (!buffer)
        return;
    
    cee_source_buffer_parse(buffer, kCEESourceBufferParserOptionCreateSymbolWrapper);
    [_editViewController setBuffer:buffer];
    CEEList* ranges = cee_ranges_from_string(symbol->locations);
    if (ranges) {
        [_editViewController highlightRanges:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
}

- (void)sessionPortCreateContextResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    
    _port = port;
    
    [self createContextSymbols];
    
    if (!_context_symbols)
        return;
    
    if (cee_list_length(_context_symbols) > 1) {
        [self showContextTable];
        [_contextTable reloadData];
    }
    else {
        [self showEditor];
        CEESourceSymbol* symbol = cee_list_nth_data(_context_symbols, 0);
        NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
        [self presentContextBufferWithSymbol:symbol];
        [_titlebar setTitle:filePath];
    }
}

- (void)createContextSymbols {
    if (_context_symbols)
        cee_list_free(_context_symbols);
    _context_symbols = NULL;
    
    if (!_port.context)
        return;
    
    CEEList* p = _port.context;
    while (p) {
        CEESourceSymbol* symbol = p->data;
        if (cee_source_symbol_is_definition(symbol))
            _context_symbols = cee_list_prepend(_context_symbols, symbol);
        p = p->next;
    }
    
    if (!_context_symbols) {
        p = _port.context;
        while (p) {
            _context_symbols = cee_list_prepend(_context_symbols, p->data);
            p = p->next;
        }
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_context_symbols)
        return 0;
    return cee_list_length(_context_symbols);
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    if (!_context_symbols)
        return @"Related Source";
    CEESourceSymbol* symbol = cee_list_nth_data(_context_symbols, 0);
    if (column == 0) {
        //NSString* name = [NSString stringWithUTF8String:symbol->name];
        //return [NSString stringWithFormat:@"\"%@\" Releated Sources", name];
        return @"Name";
    }
    else if (column == 1) {
        return @"Path";
    }
    
    return @"";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    if (column == 0) {
        CEESymbolCellView* cellView = [tableView makeViewWithIdentifier:@"IDSymbolCellView"];
        CEESourceSymbol* symbol = cee_list_nth_data(_context_symbols, (cee_int)row);
        NSString* name = [NSString stringWithUTF8String:symbol->name];
        NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
        cellView.title.stringValue = [NSString stringWithFormat:@"%@", [filePath lastPathComponent]];
        [cellView.icon setImage:[styleManager symbolIconFromSymbolType:symbol->type]];
        return cellView;
    }
    else if (column == 1) {
        CEEFileNameCellView* cellView = [tableView makeViewWithIdentifier:@"IDFilePathCellView"];
        CEESourceSymbol* symbol = cee_list_nth_data(_context_symbols, (cee_int)row);
        NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
        cellView.title.stringValue = filePath;
        return cellView;
    }
    return nil;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (IBAction)selectRow:(id)sender {
    if (!_contextTable.selectedRowIndexes || _contextTable.selectedRow == -1)
        return;
    
    [self showEditor];
    CEESourceSymbol* symbol = cee_list_nth_data(_context_symbols, (cee_int)_contextTable.selectedRow);    
    NSString* filePath = [NSString stringWithUTF8String:symbol->filepath];
    [self presentContextBufferWithSymbol:symbol];
    [_titlebar setTitle:filePath];
}

- (void)updateViewStyleConfiguration:(NSNotification*)notification {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [self.view setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    if (!_contextTable.superview)
        [_contextTable setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    
    if (!_editViewController.view.superview)
        [_editViewController setViewStyleConfiguration:[styleManager userInterfaceConfiguration]];
}

@end
