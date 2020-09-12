//
//  CEETextEditViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2018/11/29.
//  Copyright © 2018 caiyuqing. All rights reserved.
//
#import <math.h>
#import "AppDelegate.h"
#import "CEETextEditViewController.h"
#import "CEEProjectContextViewController.h"
#import "CEEStyleManager.h"
#import "CEEJSONReader.h"
#import "CEETitleView.h"
#import "CEEScroller.h"
#import "CEEStateButton.h"
#import "CEETextField.h"
#import "CEETextLabel.h"
#import "cee_text_edit.h"
#import "cee_backend.h"

@interface CEETextEditViewController()
@property (weak) IBOutlet CEETextView *textView;
@property (weak) IBOutlet CEELineNumberView *lineNmberView;
@property (weak) IBOutlet CEEScroller *verticalScroller;
@property (weak) IBOutlet CEEScroller *horizontalScroller;
@property (weak) IBOutlet CEETitleView *textSearcher;
@property (weak) IBOutlet CEETextField *searchInput;
@property (weak) IBOutlet CEETextField *replaceInput;
@property (weak) IBOutlet CEEStateButton *buttonCaseSensitive;
@property (weak) IBOutlet CEEStateButton *buttonRegex;
@property (weak) IBOutlet CEEStateButton *buttonMatchWord;
@property (weak) IBOutlet CEETextLabel *textSearchLabel;
@property (weak) IBOutlet NSLayoutConstraint *textSearcherHeight;
@property (weak) IBOutlet NSLayoutConstraint *verticalScrollerHeight;
@property (weak) IBOutlet NSLayoutConstraint *lineNumberViewWidth;
@property BOOL showLineNumber;
@property BOOL wrap;
@property BOOL searchingText;
@property BOOL searchCaseSensitive;
@property BOOL searchRegex;
@property BOOL searchMarchWord;
@property BOOL searchTimeout;
@property BOOL searchTextWhenModifing;

@property CEEList* symbolReferences;

@end

static CEEList* buffer_tags_generate(cee_pointer generator,
                                     CEERange range)
{
    AppDelegate* delegate = [NSApp delegate];
    CEETextEditViewController* controller = (__bridge CEETextEditViewController*)generator;
    CEESourceBuffer* buffer = controller.buffer;
    cee_pointer database = [[delegate currentProject] database];
    const cee_uchar* subject = cee_text_storage_buffer_get(buffer.storage);
    CEEList* symbolReferences = NULL;
    CEEList* tags = NULL;
    
    if (!buffer.parser_ref || !subject)
        return NULL;
    
    cee_source_reference_parse(buffer.parser_ref, 
                               (const cee_uchar*)[buffer.filePath UTF8String], 
                               subject, 
                               buffer.source_token_map, 
                               buffer.prep_directive, 
                               buffer.statement, 
                               range, 
                               &symbolReferences);
    if (controller.symbolReferences)
        cee_list_free_full(controller.symbolReferences, cee_reference_free);
    controller.symbolReferences = symbolReferences;
    
    tags = cee_source_tags_create(buffer.parser_ref, 
                                  buffer.source_token_map, 
                                  buffer.prep_directive, 
                                  buffer.statement, 
                                  database, 
                                  range, 
                                  controller.symbolReferences);
    return tags;
}

@implementation CEETextEditViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    _showLineNumber = YES;
    _wrap = YES;
    _searchingText = NO;
    _searchCaseSensitive = NO;
    _searchRegex = NO;
    _searchMarchWord = NO;
    _searchTimeout = NO;
    _searchTextWhenModifing = YES;
    self.editable = NO;
    self.intelligence = NO;
    
    cee_text_edit_wrap_set(_textView.edit, _wrap);
    
    if (_wrap)
        [self showHorizontalScroller:NO];
    else
        [self showHorizontalScroller:YES];
    
    [_textView setDelegate:self];
    [_searchInput setDelegate:self];
    [_replaceInput setDelegate:self];
    
    CEEStyleManager* manager = [CEEStyleManager defaultStyleManager];
    NSString* descriptor = [manager textHighlighDescriptor];
    [_textView setTextAttributesDescriptor:descriptor];
    
    [_verticalScroller setDirection:kCEEScrollerDirectionVertical];
    [_verticalScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_verticalScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_verticalScroller setEnabled:YES];
    [_verticalScroller setTarget:self];
    [_verticalScroller setAction:@selector(verticalScroll:)];
    
    [_horizontalScroller setDirection:kCEEScrollerDirectionHorizontal];
    [_horizontalScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_horizontalScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_horizontalScroller setEnabled:YES];
    [_horizontalScroller setTarget:self];
    [_horizontalScroller setAction:@selector(horizontalScroll:)];
    
    [self showTextSearch:NO];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(bufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textHighlightStyleResponse:) name:CEENotificationTextHighlightStyleUpdate object:nil];
}

- (void)viewDidAppear {
    if ([self.view.window firstResponder] != _textView)
        [self.view.window makeFirstResponder:_textView];
}

- (void)setBuffer:(CEESourceBuffer*)buffer {
    (void)self.view;
    [super setBuffer:buffer];
    if (buffer) {
        CEETextLayoutRef layout = cee_text_edit_layout([_textView edit]);
        cee_text_layout_attribute_generator_set(layout, (__bridge cee_pointer)self);
        cee_text_layout_attribute_generate_set(layout, buffer_tags_generate);
        [_textView setStorage:buffer.storage];
        [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    }
    else {
        [_textView setStorage:nil];
    }
    
    [_textView setNeedsDisplay:YES];
    [self adjustScrollers];
}

- (void)setEditable:(BOOL)editable {
    [super setEditable:editable];
    [_textView setEditable:self.editable];
}

- (void)setParagraphIndex:(NSInteger)paragraphIndex {
    CEETextEditRef edit = [_textView edit];
    CEETextLayoutRef layout = cee_text_edit_layout(edit);
    cee_text_layout_paragraph_index_set(layout, paragraphIndex);
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    [_textView setNeedsDisplay:YES];
    [self adjustScrollers];
}

- (void)adjustScrollers {
    CEETextEditRef edit = [_textView edit];
    _verticalScroller.floatValue = cee_text_edit_vertical_scroller_offset_get(edit);
    _verticalScroller.knobProportion = cee_text_edit_vertical_scroller_proportion_get(edit);
    _horizontalScroller.floatValue = cee_text_edit_horizontal_scroller_offset_get(edit);
    _horizontalScroller.knobProportion = cee_text_edit_horizontal_scroller_proportion_get(edit);
    
    // make sure horizontal scroller always draw the knob
    if (fabs(_horizontalScroller.knobProportion - 1.0) < FLT_EPSILON)
        _horizontalScroller.knobProportion = 0.99;
    
    CEETextLayoutRef layout = cee_text_edit_layout(edit);
    NSInteger paragraphIndex = cee_text_layout_paragraph_index_get(layout);
    [[self.port currentBufferReference] setParagraphIndex:paragraphIndex];
}

- (void)showTextSearch:(BOOL)shown {
    if (shown)
        _textSearcherHeight.constant = 67.0;
    else
        _textSearcherHeight.constant = 0.0;
    [self.view updateConstraints];
}

- (void)showLineNumber:(BOOL)shown {
    if (shown)
        _lineNumberViewWidth.constant = 60.0;
    else
        _lineNumberViewWidth.constant = 0.0;  
    [self.view updateConstraints];
}

- (void)showHorizontalScroller:(BOOL)shown {
    if (shown)
        _verticalScrollerHeight.constant = CEEHorizontalScrollerHeight;
    else
        _verticalScrollerHeight.constant = 0.0;
    [self.view updateConstraints];
}

-(void)dealloc {
    if (_symbolReferences)
        cee_list_free_full(_symbolReferences, cee_reference_free);
    _symbolReferences = NULL;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)becomeFirstResponder {
    if ([self.view.window firstResponder] != _textView)
        [self.view.window makeFirstResponder:_textView];
    return YES;
}

- (void)verticalScroll:(id)sender {
    CEETextEditRef edit = [_textView edit];
    CEETextLayoutRef layout = cee_text_edit_layout(edit);
    cee_text_edit_scroll_vertical_to(edit, _verticalScroller.floatValue);
    _horizontalScroller.floatValue = cee_text_edit_horizontal_scroller_offset_get(edit);
    _horizontalScroller.knobProportion = cee_text_edit_horizontal_scroller_proportion_get(edit);
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    [_textView setNeedsDisplay:YES];
    
    NSInteger paragraphIndex = cee_text_layout_paragraph_index_get(layout);
    [[self.port currentBufferReference] setParagraphIndex:paragraphIndex];
}

- (void)horizontalScroll:(id)sender {
    CEETextEditRef edit = [_textView edit];
    cee_text_edit_scroll_horizontal_to(edit, _horizontalScroller.floatValue);
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    [_textView setNeedsDisplay:YES];
}

- (IBAction)closeTextSearcher:(id)sender {
    _searchingText = !_searchingText;
    [self showTextSearch:_searchingText];
    [self.view.window makeFirstResponder:_textView];
    [_textView setHighLightSearched:_searchingText];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
}

- (NSArray*)createlineNumberTags {
    NSMutableArray* tags = [[NSMutableArray alloc] init];
    
    CEETextLayoutRef layout = cee_text_edit_layout(_textView.edit);
    CEEList* p = cee_text_layout_lines_get(layout);
    while (p) {
        NSRect rect;
        CEETextLineRef line = p->data;
        CEERect bounds = cee_text_line_bounds_get(line);
        cee_long index = cee_text_line_paragraph_index_get(line);
        
        NSString* number = nil;
        if (index != -1)
            number = [NSString stringWithFormat:@"%lu", index + 1];
        else
            number = @"...";
        
        rect = NSMakeRect(bounds.origin.x, 
                          bounds.origin.y, 
                          bounds.size.width, 
                          bounds.size.height);
        [tags addObject:@{ @"number": number, @"bounds": NSStringFromRect(rect) }];
        p = p->next;
    }
    
    return tags;
}

- (void)searchText {
    cee_text_edit_search(_textView.edit,
                         (cee_char*)[_searchInput.stringValue UTF8String],
                         _searchRegex,
                         _searchCaseSensitive,
                         _searchMarchWord,
                         &_searchTimeout);
    if (_searchTimeout) {
        cee_text_edit_select_all(_searchInput.edit);
        cee_text_edit_insert(_searchInput.edit, 
                             (const cee_uchar*)"search timeout");
        cee_text_edit_select_all(_searchInput.edit);
        _searchTimeout = NO;
    }
    
    CEEList* ranges = cee_text_edit_searched_ranges_get(_textView.edit);
    cee_ulong nb_matched = cee_list_length(ranges);
    _textSearchLabel.stringValue = [NSString stringWithFormat:@"%lu matches", nb_matched];
}

- (IBAction)searchCaseSensitive:(id)sender {
    [_buttonRegex setState:NSControlStateValueOff];
    _searchRegex = NO;
    
    if (_buttonCaseSensitive.state == NSControlStateValueOn)
        _searchCaseSensitive = YES;
    else
        _searchCaseSensitive = NO;
    
    [self searchText];
}

- (IBAction)searchRegex:(id)sender {
    [_buttonMatchWord setState:NSControlStateValueOff];
    [_buttonCaseSensitive setState:NSControlStateValueOff];
    
    _searchCaseSensitive = NO;
    _searchMarchWord = NO;
    
    if (_buttonRegex.state == NSControlStateValueOn)
        _searchRegex = YES;
    else
        _searchRegex = NO;
    
    [self searchText];
}

- (IBAction)searchMatchWord:(id)sender {
    [_buttonRegex setState:NSControlStateValueOff];
    _searchRegex = NO;
    
    if (_buttonMatchWord.state == NSControlStateValueOn)
        _searchMarchWord = YES;
    else
        _searchMarchWord = NO;
    
    [self searchText];        
}

- (IBAction)prevSearched:(id)sender {
    CEETextEditRef edit = _textView.edit;
    cee_long index = cee_text_edit_searched_index_get(edit);
    index --;
    cee_text_edit_searched_index_set(edit, index);
    
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
}

- (IBAction)nextSearched:(id)sender {
    CEETextEditRef edit = _textView.edit;
    cee_long index = cee_text_edit_searched_index_get(edit);
    index ++;
    cee_text_edit_searched_index_set(edit, index);
    
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
}

- (IBAction)replaceSearched:(id)sender {
    CEETextEditRef edit = _textView.edit;
    CEEList* ranges = NULL;
    const cee_uchar* str = NULL;
    cee_long index = -1;
    CEEList* replaces = NULL;
    CEERange replace;
    CEERange* range = NULL;
    CEEList* p = NULL;
        
    ranges = cee_text_edit_searched_ranges_get(edit);
    if (!ranges)
        return;
    
    _searchTextWhenModifing = NO;
    
    str = (const cee_uchar*)[_replaceInput.stringValue UTF8String];
    index = cee_text_edit_searched_index_get(edit);
    replace = *(CEERange*)cee_list_nth_data(ranges, (cee_int)index);
    replaces = cee_list_append(replaces, &replace);
    
    cee_text_edit_replace_ranges(edit, str, replaces);
    cee_list_free(replaces);
    
    if (_searchingText)
        [self searchText];
    
    ranges = cee_text_edit_searched_ranges_get(edit);
    if (ranges) {
        index = 0;
        p = ranges;
        while (TRUE) {
            range = p->data;
            if (range->location > replace.location)
                break;
            
            p = p->next;
            if (!p)
                break;
            index ++;
        }
        cee_text_edit_searched_index_set(edit, index);
    }
    
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    _searchTextWhenModifing = YES;
}

- (IBAction)replaceAllSearched:(id)sender {
    CEETextEditRef edit = _textView.edit;
    CEEList* ranges = NULL;
    const cee_uchar* str = NULL;
    
    str = (const cee_uchar*)[_replaceInput.stringValue UTF8String];
    ranges = cee_text_edit_searched_ranges_get(edit);
    if (!ranges)
        return;
    
    _searchTextWhenModifing = NO;
    
    cee_text_edit_replace_ranges(edit, str, ranges);
    
    if (_searchingText)
        [self searchText];
    
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    _searchTextWhenModifing = YES;
}

- (void)mouseDown:(NSEvent *)event {
    cee_text_edit_highlight_clear(_textView.edit);
    [super mouseDown:event];
}

- (void)highlightRanges:(CEEList*)ranges {
    cee_text_edit_highlight_set(_textView.edit, ranges);

    CEETextEditRef edit = _textView.edit;
    CEETextStorageRef storage = self.buffer.storage;
    CEERange* range0 = cee_list_nth_data(ranges, 0);
    cee_long index = cee_text_storage_paragraph_index_get(storage, range0->location);
    cee_text_edit_scroll_to_pragraph(edit, index);    
    [_textView setNeedsDisplay:YES];
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
}

- (void)bufferStateChangedResponse:(NSNotification*)notification {
    CEESourceBuffer* buffer = notification.object;
    if (buffer != self.buffer)
        return;
    
    if ([buffer stateSet:kCEESourceBufferStateModified]) {
        
        if (_searchingText && _searchTextWhenModifing)
            [self searchText];
        
        // if view is actived and selected, that means buffer is modified by 
        // this editor, we don't need to do anyting. 
        if ([self.view styleState] == kCEEViewStyleStateActived)
            return;
        
        cee_text_edit_modified_update(_textView.edit);
    }
    else if ([buffer stateSet:kCEESourceBufferStateReload]) {
        cee_text_edit_modified_update(_textView.edit);
    }
    
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
}

- (void)textHighlightStyleResponse:(NSNotification*)notification {
    CEEStyleManager* manager = [CEEStyleManager defaultStyleManager];
    NSString* descriptor = [manager textHighlighDescriptor];
    [_textView setTextAttributesDescriptor:descriptor];
}

#pragma mark - CEETextViewDelegate protocol

-(void)textViewTextChanged:(CEETextView*)textView {
    if (textView == _textView) {
        [self adjustScrollers];
        [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    }
    else if (textView == _searchInput) {
        [self searchText];
    }
}

-(void)textViewScrolled:(CEETextView*)textView {
    if (textView == _textView) {
        [self adjustScrollers];
        [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    }
}

- (void)textViewFrameChangedResponse:(NSNotification*)notification {
    [self adjustScrollers];
    [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
}

- (void)textViewCaretMove:(CEETextView*)textView {
    if (textView == _textView) {
        [self adjustScrollers];
        [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
    }
}

- (void)textViewSearchText:(CEETextView*)textView {
    if (textView == _textView) {
        if (!_searchingText) {
            _searchingText = YES;
            [self showTextSearch:_searchingText];
            [_textView setHighLightSearched:_searchingText];
            [_lineNmberView setLineNumberTags:[self createlineNumberTags]];
        }
        [self.view.window makeFirstResponder:_searchInput];
        cee_text_edit_select_all(_searchInput.edit);
    }
}

- (void)textViewEscape:(CEETextView*)textView {
    if (textView == _searchInput ||
        textView == _replaceInput) {
        [self closeTextSearcher:self];
    }
}

- (void)textViewNewLine:(CEETextView*)textView {
    if (textView == _searchInput)
        [self nextSearched:self];
}

- (void)textViewSelectWithCommand:(CEETextView*)textView {
    if (!self.intelligence || textView != _textView)
        return;
    
    cee_long offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    CEETokenCluster* cluster = cee_token_cluster_search_by_buffer_offset(_symbolReferences,
                                                                         self.buffer.prep_directive,
                                                                         self.buffer.statement,
                                                                         offset);
    
    [self.port jumpToTargetSymbolByCluster:cluster];
    cee_token_cluster_free(cluster);
    return;
}

- (void)textViewHighlightTokenCluster:(CEETextView*)textView {
    if (!self.intelligence || textView != _textView)
        return;
    
    CEEList* ranges = NULL;
    cee_long offset = cee_text_edit_cursor_buffer_offset_get(_textView.edit);
    CEETokenCluster* cluster = cee_token_cluster_search_by_buffer_offset(_symbolReferences, 
                                                                         self.buffer.prep_directive, 
                                                                         self.buffer.statement, 
                                                                         offset);
    if (cluster) {
        if (cluster->type == kCEETokenClusterTypeReference) {
            CEESourceReference* reference = (CEESourceReference*)cluster->content_ref;
            ranges = cee_ranges_from_string(reference->locations);
        }
        else if (cluster->type == kCEETokenClusterTypeSymbol) {
            CEESourceSymbol* symbol = (CEESourceSymbol*)cluster->content_ref;
            ranges = cee_ranges_from_string(symbol->locations);
        }
        
        if (ranges) {
            cee_text_edit_highlight_set([_textView edit], ranges);
            [_textView setNeedsDisplay:YES];
            cee_list_free_full(ranges, cee_range_free);
        }
        cee_token_cluster_free(cluster);
    }
    else {
        cee_text_edit_highlight_set([_textView edit], NULL);
        [_textView setNeedsDisplay:YES];
    }
}

- (void)textViewIgnoreTokenCluster:(CEETextView*)textView {
    if (!self.intelligence || textView != _textView)
        return;
    cee_text_edit_highlight_set([_textView edit], NULL);
    [_textView setNeedsDisplay:YES];
}

- (void)textViewCreateContext:(CEETextView*)textView {
    if (!self.intelligence || textView != _textView)
        return;    

    cee_long offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    CEETokenCluster* cluster = cee_token_cluster_search_by_buffer_offset(_symbolReferences, 
                                                                         self.buffer.prep_directive, 
                                                                         self.buffer.statement, 
                                                                         offset);
    [self.port createContextByCluster:cluster];
    cee_token_cluster_free(cluster);
}

@end
