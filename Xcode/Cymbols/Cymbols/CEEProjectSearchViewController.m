//
//  CEEProjectSearchViewController.m
//  Cymbols
//
//  Created by qing on 2020/9/28.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEEProjectSearchViewController.h"
#import "CEEEditViewController.h"
#import "CEEImageTextTableCellView.h"
#import "CEETextTableCellView.h"
#import "CEECheckBox.h"
#import "cee_backend.h"
#import "cee_datetime.h"
#import "CEETitleView.h"
#import "cee_regex.h"

@interface CEEProjectSearchViewController ()
@property (weak) IBOutlet CEECheckBox *buttonSearchCaseSensitive;
@property (weak) IBOutlet CEECheckBox *buttonSearchMatchWord;
@property (weak) IBOutlet CEECheckBox *buttonSearchAsReference;
@property (weak) IBOutlet CEECheckBox *buttonSearchAsString;
@property (weak) IBOutlet CEECheckBox *buttonSearchAsRegex;
@property (weak) IBOutlet CEETextField *targetInput;
@property (weak) IBOutlet CEETextField *filePatternInput;
@property (weak) IBOutlet CEEButton *buttonCancel;
@property (weak) IBOutlet CEEButton *buttonSearch;
@property (weak) IBOutlet CEEButton *buttonClose;
@property (weak) IBOutlet CEEButton *buttonSelect;

@property BOOL cancel;
@property BOOL searching;
@property (strong) CEEProject* project;
@property (weak) IBOutlet CEETableView *resultTable;
@property (weak) IBOutlet CEEView *sourceContentView;
@property (strong) CEETitleView *titlebar;
@property (strong) CEEEditViewController *editViewController;
@property (weak) IBOutlet CEETextLabel *labelParsing;
@property (weak) IBOutlet NSProgressIndicator *progressBar;
@property (strong) CEESourceBuffer* contextSourceBuffer;
@end

@implementation CEEProjectSearchViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [_resultTable setNibNameOfCellView:@"TableCellViews"];
    [_resultTable setNumberOfColumns:1];
    [_resultTable setDataSource:self];
    [_resultTable setDelegate:self];
    [_resultTable setTarget:self];
    [_resultTable setAction:@selector(selectRow:)];
    [_resultTable setDoubleAction:@selector(selectItem:)];
    [_resultTable setEnableDrawHeader:YES];
    
    [_targetInput setDelegate:self];
    [_filePatternInput setDelegate:self];
    
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
    
    [_labelParsing setHidden:YES];
    [_progressBar setHidden:YES];
}

- (void)dealloc {
    AppDelegate* delegate = [NSApp delegate];
    if (_contextSourceBuffer)
        [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
    _contextSourceBuffer = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    AppDelegate* delegate = [NSApp delegate];
    _project = [delegate currentProject];
    [_resultTable reloadData];
    
    [_labelParsing setHidden:YES];
    [_progressBar setHidden:YES];
    
    [self enableOptionInputs];
    [self updateButtonState];
    
    if (_project.searcher.target) 
        [_targetInput setStringValue:_project.searcher.target];
    
    if (_project.searcher.filePattern)
        [_filePatternInput setStringValue:_project.searcher.filePattern];
    
    [_buttonCancel setEnabled:NO];
    [_resultTable reloadData];
    _searching = NO;
    _cancel = NO;
    _selectedResult = nil;
    
    if (_autoStart)
        [self search:nil];
}

- (void)viewDidDisappear {
    [_editViewController setBuffer:nil];
    [super viewDidDisappear];
}

- (void)serchReference {
    __block AppDelegate* delegate = [NSApp delegate];
    __block NSArray* filePaths = [_project getReferenceFilePathsWithCondition:_project.searcher.filePattern];
    if (!filePaths)
        return;
    
    _selectedResult = nil;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        NSMutableArray* results = [[NSMutableArray alloc] init];
        self->_project.searcher.results = results;
        self->_searching = YES;
                
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self UISetupBeforSearch];
            [self disableOptionInputs];
        });
        
        for (int i = 0; i < filePaths.count; i ++) {
            @autoreleasepool {
                __block NSString* filePath = filePaths[i];
                CEEList* references = NULL;
                
                __block CEESourceBuffer* buffer = nil;
                
                // open source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                dispatch_sync(dispatch_get_main_queue(), ^{
                    buffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
                });
                
                const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
                CEERange range = cee_range_make(0, cee_text_storage_size_get(buffer.storage));
                cee_source_reference_parse(buffer.parser_ref,
                                            [buffer.filePath UTF8String],
                                            (const cee_char*)subject,
                                            buffer.source_token_map,
                                            buffer.prep_directive_fragment,
                                            buffer.statement_fragment,
                                            range,
                                            &references);
                CEEList* p = references;
                while (p) {
                    CEESourceSymbolReference* reference = p->data;
                    if (!strcmp(reference->name, [self->_project.searcher.target UTF8String])) {
                        CEESourcePoint* result = [[CEESourcePoint alloc] initWithSourceSymbolReference:reference];
                        [results addObject:result];
                        dispatch_sync(dispatch_get_main_queue(), ^{
                            [self->_resultTable reloadData];
                            if (!self->_selectedResult) {
                                [self selectResultAtIndex:0];
                                [self->_buttonSelect setEnabled:YES];
                            }
                        });
                    }
                    p = p->next;
                }
                cee_list_free_full(references, cee_source_symbol_reference_free);
                
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [self->_labelParsing setStringValue:[filePath lastPathComponent]];
                    [self->_progressBar setDoubleValue:(double)(i+1)/filePaths.count];
                });
                
                
                // close source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [delegate.sourceBufferManager closeSourceBuffer:buffer];
                });
                
                if (self->_cancel)
                    break;
            }
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self UISetupAfterSearched];
            [self enableOptionInputs];
            self->_searching = NO;
        });
    });
}

- (void)searchRegex {
    __block AppDelegate* delegate = [NSApp delegate];
    __block NSArray* filePaths = [_project getReferenceFilePathsWithCondition:_project.searcher.filePattern];
    if (!filePaths)
        return;
    
    _selectedResult = nil;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        NSMutableArray* results = [[NSMutableArray alloc] init];
        self->_project.searcher.results = results;
        self->_searching = YES;
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self UISetupBeforSearch];
            [self disableOptionInputs];
        });
        
        for (int i = 0; i < filePaths.count; i ++) {
            @autoreleasepool {
                NSString* filePath = filePaths[i];
                
                __block CEESourceBuffer* buffer = nil;
                // open source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                dispatch_sync(dispatch_get_main_queue(), ^{
                    buffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
                });
                
                cee_boolean time_out = FALSE;
                const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
                CEEList* ranges = cee_regex_search((const cee_char*)subject,
                                                   [self->_project.searcher.target UTF8String],
                                                   TRUE,
                                                   10000,
                                                   &time_out);
                CEEList* p = ranges;
                while (p) {
                    cee_char* range_string = cee_string_from_range(p->data);
                    if (range_string) {
                        CEESourcePoint* result = [[CEESourcePoint alloc] init];
                        result.filePath = filePath;
                        result.locations = [NSString stringWithUTF8String:range_string];
                        result.lineNumber = -1;
                        [results addObject:result];
                        
                        dispatch_sync(dispatch_get_main_queue(), ^{
                            [self->_resultTable reloadData];
                            if (!self->_selectedResult) {
                                [self selectResultAtIndex:0];
                                [self->_buttonSelect setEnabled:YES];
                            }
                        });
                        
                        cee_free(range_string);
                    }
                    p = p->next;
                }
                cee_list_free_full(ranges, cee_range_free);
                
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [self->_labelParsing setStringValue:[filePath lastPathComponent]];
                    [self->_progressBar setDoubleValue:(double)(i+1)/filePaths.count];
                });
                
                // close source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [delegate.sourceBufferManager closeSourceBuffer:buffer];
                });
                
                if (self->_cancel)
                    break;
            }
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self UISetupAfterSearched];
            [self enableOptionInputs];
            self->_searching = NO;
        });
    });
}

- (void)searchString {
    __block AppDelegate* delegate = [NSApp delegate];
    __block NSArray* filePaths = [_project getReferenceFilePathsWithCondition:_project.searcher.filePattern];
    if (!filePaths)
        return;
    
    _selectedResult = nil;
    
    cee_boolean sensitive = FALSE;
    cee_boolean mode = kCEEStringSearchModeMatchWord;
    if (_project.searcher.options & kCEESearchOptionCaseSensitive)
        sensitive = YES;
    
    if (_project.searcher.options & kCEESearchOptionMatchWord)
        mode = kCEEStringSearchModeMatchWord;
    else
        mode = kCEEStringSearchModeContains;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        NSMutableArray* results = [[NSMutableArray alloc] init];
        self->_project.searcher.results = results;
        self->_searching = YES;
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self UISetupBeforSearch];
            [self disableOptionInputs];
        });
        
        for (int i = 0; i < filePaths.count; i ++) {
            @autoreleasepool {
                NSString* filePath = filePaths[i];
                
                __block CEESourceBuffer* buffer = nil;
                
                // open source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                dispatch_sync(dispatch_get_main_queue(), ^{
                    buffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:filePath];
                });
                
                const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
                CEEList* ranges = cee_str_search((const cee_char*)subject,
                                                    [self->_project.searcher.target UTF8String],
                                                    sensitive,
                                                    mode);
                CEEList* p = ranges;
                while (p) {
                    cee_char* range_string = cee_string_from_range(p->data);
                    if (range_string) {
                        CEESourcePoint* result = [[CEESourcePoint alloc] init];
                        result.filePath = filePath;
                        result.locations = [NSString stringWithUTF8String:range_string];
                        result.lineNumber = -1;
                        [results addObject:result];
                        
                        dispatch_sync(dispatch_get_main_queue(), ^{
                            [self->_resultTable reloadData];
                            if (!self->_selectedResult) {
                                [self selectResultAtIndex:0];
                                [self->_buttonSelect setEnabled:YES];
                            }
                        });
                        
                        cee_free(range_string);
                    }
                    p = p->next;
                }
                cee_list_free_full(ranges, cee_range_free);
                
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [self->_labelParsing setStringValue:[filePath lastPathComponent]];
                    [self->_progressBar setDoubleValue:(double)(i+1)/filePaths.count];
                });
                
                // close source buffer in main queue(cause [NSApp delegate] should be invoked in main queue)
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [delegate.sourceBufferManager closeSourceBuffer:buffer];
                });
                                
                if (self->_cancel)
                    break;
            }
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self UISetupAfterSearched];
            [self enableOptionInputs];
            self->_searching = NO;
        });
    });
}

- (IBAction)search:(id)sender {
    if (!_project.searcher.target || [_project.searcher.target isEqualToString:@""])
        return;
    
    _project.searcher.results = nil;
    [_resultTable reloadData];
    _cancel = NO;
    [_editViewController setBuffer:nil];
    
    if (_project.searcher.mode == kCEESearchModeReference)
        [self serchReference];
    else if (_project.searcher.mode == kCEESearchModeString)
        [self searchString];
    else if (_project.searcher.mode == kCEESearchModeRegex) 
        [self searchRegex];
}

- (IBAction)cancel:(id)sender {
    _cancel = YES;
}

- (void)disableOptionInputs {
    [_buttonSearchAsString setEnabled:NO];
    [_buttonSearchAsReference setEnabled:NO];
    [_buttonSearchAsRegex setEnabled:NO];
    [_buttonSearchCaseSensitive setEnabled:NO];
    [_buttonSearchMatchWord setEnabled:NO];
}

- (void)enableOptionInputs {
    [_buttonSearchAsString setEnabled:YES];
    [_buttonSearchAsReference setEnabled:YES];
    [_buttonSearchAsRegex setEnabled:YES];
    
    CEESearchMode mode = _project.searcher.mode;
    
    if (mode == kCEESearchModeReference) {
        [_buttonSearchMatchWord setEnabled:NO];
        [_buttonSearchCaseSensitive setEnabled:NO];
    }
    else if (mode == kCEESearchModeString) {
        [_buttonSearchMatchWord setEnabled:YES];
        [_buttonSearchCaseSensitive setEnabled:YES];
    }
    else if (mode == kCEESearchModeRegex) {
        [_buttonSearchMatchWord setEnabled:NO];
        [_buttonSearchCaseSensitive setEnabled:NO];
    }
}

- (void)UISetupBeforSearch {
    [self disableOptionInputs];
    [self->_labelParsing setHidden:NO];
    [self->_progressBar setHidden:NO];
    [self->_progressBar setMinValue:0.0];
    [self->_progressBar setMaxValue:1.0];
    [self->_progressBar setDoubleValue:0.0];
    [self->_buttonSearch setEnabled:NO];
    [self->_buttonCancel setEnabled:YES];
    [self->_buttonClose setEnabled:NO];
    [self->_buttonSelect setEnabled:NO];
}

- (void)UISetupAfterSearched {
    [self->_labelParsing setHidden:YES];
    [self->_progressBar setHidden:YES];
    [self->_buttonSearch setEnabled:YES];
    [self->_buttonCancel setEnabled:NO];
    [self->_buttonClose setEnabled:YES];
    [self->_buttonSelect setEnabled:YES];
    [self enableOptionInputs];
}

- (IBAction)close:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (IBAction)searchCaseSensitive:(id)sender {
    CEESearchOption options = _project.searcher.options;
    if (options & kCEESearchOptionCaseSensitive)
        options &= ~kCEESearchOptionCaseSensitive;
    else
        options |= kCEESearchOptionCaseSensitive;
    [_project.searcher setOptions:options];
    [self updateButtonState];
}

- (IBAction)searchWord:(id)sender {
    CEESearchOption options = _project.searcher.options;
    if (options & kCEESearchOptionMatchWord)
        options &= ~kCEESearchOptionMatchWord;
    else
        options |= kCEESearchOptionMatchWord;
    [_project.searcher setOptions:options];
    [self updateButtonState];
}

- (IBAction)searchAsReference:(id)sender {
    [_project.searcher setMode:kCEESearchModeReference];
    [self updateButtonState];
}

- (IBAction)searchAsString:(id)sender {
    [_project.searcher setMode:kCEESearchModeString];
    [self updateButtonState];
}

- (IBAction)searchAsRegex:(id)sender {
    [_project.searcher setMode:kCEESearchModeRegex];
    [self updateButtonState];
}

- (void)updateButtonState {
    CEESearchOption options = _project.searcher.options;
    CEESearchMode mode = _project.searcher.mode;
    
    if (mode == kCEESearchModeReference) {
        [_buttonSearchAsReference setState:NSControlStateValueOn];
        [_buttonSearchMatchWord setEnabled:NO];
        [_buttonSearchCaseSensitive setEnabled:NO];
    }
    else {
        [_buttonSearchAsReference setState:NSControlStateValueOff];
    }
    
    if (mode == kCEESearchModeString) {
        [_buttonSearchAsString setState:NSControlStateValueOn];
        [_buttonSearchMatchWord setEnabled:YES];
        [_buttonSearchCaseSensitive setEnabled:YES];
    }
    else {
        [_buttonSearchAsString setState:NSControlStateValueOff];
    }
    
    if (mode == kCEESearchModeRegex) {
        [_buttonSearchAsRegex setState:NSControlStateValueOn];
        [_buttonSearchMatchWord setEnabled:NO];
        [_buttonSearchCaseSensitive setEnabled:NO];
    }
    else {
        [_buttonSearchAsRegex setState:NSControlStateValueOff];
    }
    
    if (options & kCEESearchOptionMatchWord)
        [_buttonSearchMatchWord setState:NSControlStateValueOn];
    else
        [_buttonSearchMatchWord setState:NSControlStateValueOff];
    
    if (options & kCEESearchOptionCaseSensitive)
        [_buttonSearchCaseSensitive setState:NSControlStateValueOn];
    else
        [_buttonSearchCaseSensitive setState:NSControlStateValueOff];
    
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (!_project.searcher.results)
        return 0;
    return _project.searcher.results.count;
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
    CEESourcePoint* sourcePoint = _project.searcher.results[row];
    CEEImageTextTableCellView* cellView = [tableView makeViewWithIdentifier:@"IDImageTextTableCellView"];
    NSString* fileName = [sourcePoint.filePath lastPathComponent];
    
    if (sourcePoint.lineNumber != -1)
        cellView.text.stringValue = [NSString stringWithFormat:@"%ld %@ - line %ld", row, fileName, sourcePoint.lineNumber + 1];
    else
        cellView.text.stringValue = [NSString stringWithFormat:@"%ld %@", row, fileName];
        
    [cellView.icon setImage:[styleManager iconFromFileName:fileName]];
    return cellView;
}

- (IBAction)selectRow:sender {
    if (!_resultTable.selectedRowIndexes || _resultTable.selectedRow == -1)
        return;
    
    AppDelegate* delegate = [NSApp delegate];
    CEESourcePoint* result = _project.searcher.results[_resultTable.selectedRow];
    
    if (_contextSourceBuffer)
        [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
    _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:result.filePath];
    [_editViewController setBuffer:_contextSourceBuffer];
    CEEList* ranges = cee_ranges_from_string([result.locations UTF8String]);
    if (ranges) {
        [_editViewController highlight:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
    [_titlebar setTitle:result.filePath];
    
    return;
}

- (void)selectResultAtIndex:(NSInteger)index {
    NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:index];
    [_resultTable selectRowIndexes:indexSet byExtendingSelection:NO];
    [_resultTable scrollRowToVisible:[indexSet firstIndex]];
    
    
    AppDelegate* delegate = [NSApp delegate];
    _selectedResult = _project.searcher.results[index];
    
    if (_contextSourceBuffer)
        [delegate.sourceBufferManager closeSourceBuffer:_contextSourceBuffer];
    _contextSourceBuffer = [delegate.sourceBufferManager openSourceBufferWithFilePath:_selectedResult.filePath];
    [_editViewController setBuffer:_contextSourceBuffer];
    CEEList* ranges = cee_ranges_from_string([_selectedResult.locations UTF8String]);
    if (ranges) {
        [_editViewController highlight:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
    [_titlebar setTitle:_selectedResult.filePath];
    return;
}

- (IBAction)selectItem:(id)sender {
    if (_searching)
        [self cancel:self];
    
    if (_project.searcher.results)
        _selectedResult = _project.searcher.results[(cee_int)_resultTable.selectedRow];
    
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (void)textViewTextChanged:(CEETextView *)textView {
    if (textView == _targetInput)
        _project.searcher.target = _targetInput.stringValue;
    else if (textView == _filePatternInput)
        _project.searcher.filePattern = _filePatternInput.stringValue;
}

@end
