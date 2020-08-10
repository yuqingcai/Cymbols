//
//  CEESessionContextController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/1.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CymbolsDelegate.h"
#import "CEEStyleManager.h"
#import "CEESessionContextController.h"
#import "CEEImageView.h"
#import "CEESplitView.h"
#import "CEESourceViewController.h"
#import "cee_symbol.h"

@interface CEESessionContextController ()
@property (strong) IBOutlet CEETitleView *titlebar;
@property (strong) IBOutlet CEEImageView *titleIcon;
@property (strong) CEESplitView *splitView;
@property (strong) CEESourceViewController *sourceViewController;
@property (strong) CEETableView *sourceTable;
@property (strong) CEESourceBuffer *buffer;
@property CEEList* symbols_ref;

@end

@implementation CEESessionContextController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    CGFloat leadingOffset = _titleIcon.frame.size.width ;
    [_titlebar setLeadingOffset:leadingOffset];
    [_titlebar setTitle:@"Context"];
    [self createSubviews];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferSymbolSearchedResponse:) name:CEENotificationSourceBufferSymbolSearched object:nil];
}

- (void)viewDidAppear {
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)setSourceViewHeight:(CGFloat)proportion {
    CGFloat subviewHeight = (_splitView.frame.size.height - (_splitView.dividerThickness * (_splitView.subviews.count - 1)));
    CGFloat position = (subviewHeight) * (1.0 - proportion);
    [_splitView setPosition:position ofDividerAtIndex:0];
}

- (void)setSourceViewWidth:(CGFloat)proportion {
    CGFloat subviewWidth = (_splitView.frame.size.width - (_splitView.dividerThickness * (_splitView.subviews.count - 1)));
    CGFloat position = (subviewWidth) * (1.0 - proportion);
    [_splitView setPosition:position ofDividerAtIndex:0];
}

- (void)createSubviews {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    
    _splitView =  [[CEESplitView alloc] init];
    [_splitView setStyle:self.view.style];
    [_splitView setDelegate:self];
    [_splitView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [_splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_splitView setDividerStyle:NSSplitViewDividerStyleThin];
    [_splitView setVertical:YES];
    [self.view addSubview:_splitView];
    
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"splitView" : _splitView,
    };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[splitView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[splitView(>=180)]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
        
    //_sourceTable = [[CEETableView alloc] init];
    //[_sourceTable setTranslatesAutoresizingMaskIntoConstraints:NO];
    //[_sourceTable setIdentifier:@"IDContextSourceTableView"];
    //[_sourceTable setDelegate:self];
    //[_sourceTable setDataSource:self];
    //[_sourceTable setColumns:1];
    //[_sourceTable reloadData];
    //[_sourceTable setTarget:self];
    //[_sourceTable setAction:@selector(selectRow:)];
    
    _sourceViewController =  [[NSStoryboard storyboardWithName:@"SourceViewController" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_sourceViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    //[_splitView addSubview:_sourceTable];
    [_splitView addSubview:_sourceViewController.view];
        
    // set holding priorities
    // source table has lower holding priority
    //[_splitView setHoldingPriority:200 forSubviewAtIndex:0];
    //[_splitView setHoldingPriority:200 forSubviewAtIndex:1];
    
    [_splitView adjustSubviews];
    
    //[self setSourceViewWidth:0.7];
}

- (void)sourceBufferSymbolSearchedResponse:(NSNotification*)notification {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    CEESourceBuffer* buffer = [[_session.activedPort currentReference] buffer];
    if ([notification object] != buffer)
        return;
    
    _symbols_ref = [buffer symbolsSearched];
    
    if (_symbols_ref) {
        [_sourceTable reloadData];
        CEESourceSymbol* symbol = cee_list_first(_symbols_ref)->data;
        _buffer = [[delegate sourceBufferManager] openSourceBufferWithFilePath:[NSString stringWithUTF8String:symbol->filepath]];
        [_sourceViewController setBuffer:_buffer];
        CEEList* ranges = cee_ranges_from_string(symbol->locations);
        if (ranges) {
            CEERange range = cee_range_consistent_from_discrete(ranges);
            [_sourceViewController highlightRange:range];
            cee_list_free_full(ranges, cee_range_free);
        }
    }
    else {
        [_sourceViewController setBuffer:nil];
    }
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

- (IBAction)selectRow:(id)sender {
    CymbolsDelegate* delegate = (CymbolsDelegate*)[NSApp delegate];
    if (!_sourceTable.selectedRowIndexes)
        return;
    
    cee_int index = (cee_int)_sourceTable.selectedRow;
    CEESourceSymbol* symbol = (CEESourceSymbol*)cee_list_nth_data(_symbols_ref, index);
    _buffer = [[delegate sourceBufferManager] openSourceBufferWithFilePath:[NSString stringWithUTF8String:symbol->filepath]];
    [_sourceViewController setBuffer:_buffer];
    CEEList* ranges = cee_ranges_from_string(symbol->locations);
    if (ranges) {
        CEERange range = cee_range_consistent_from_discrete(ranges);
        [_sourceViewController highlightRange:range];
        cee_list_free_full(ranges, cee_range_free);
    }
    
    NSString* titleString = nil;
    NSString* descriptor = nil;
    NSString* filePath = nil;
    NSString* locations = nil;
    
    if (symbol->filepath && symbol->descriptor && symbol->locations) {
        descriptor = [NSString stringWithFormat:@" %s", symbol->descriptor];
        filePath = [NSString stringWithFormat:@" %s", symbol->filepath];
        
        CEEList* ranges = cee_ranges_from_string(symbol->locations);
        if (ranges) {
            CEERange range = cee_range_consistent_from_discrete(ranges);
            locations = [NSString stringWithFormat:@"%ld", range.location];
            cee_list_free_full(ranges, cee_range_free);
        }
        else {
            locations = [NSString stringWithFormat:@""];
        }        
        
        titleString = [NSString stringWithFormat:@"%@ at file: [%@], offset: [%@]", descriptor, filePath, locations];
        
        [_titlebar setTitle:titleString];
    }
}

@end
