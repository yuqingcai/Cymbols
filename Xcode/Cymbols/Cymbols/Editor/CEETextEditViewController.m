//
//  CEETextEditViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2018/11/29.
//  Copyright © 2018 caiyuqing. All rights reserved.
//
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
#import "cee_regex.h"

@interface CEETextEditViewController()
@property (weak) IBOutlet CEETextView *textView;
@property (weak) IBOutlet CEELineNumberView *lineNumberView;
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
@property (weak) IBOutlet NSLayoutConstraint *horizontalScrollerHeight;
@property (weak) IBOutlet NSLayoutConstraint *lineNumberViewWidthConstraint;
@property BOOL showLineNumber;
@property BOOL searchingText;
@property BOOL searchCaseSensitive;
@property BOOL searchRegex;
@property BOOL searchMarchWord;
@property BOOL replacingSearchedContent;
@property cee_boolean searchTimeout;
@property CEEList* references;
@property CEEList* highlightRanges;
@property CEEList* searchRanges;
@property NSInteger searchRangeIndex;
@end

@implementation CEETextEditViewController

@synthesize showLineNumber = _showLineNumber;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CEEStyleManager* manager = [CEEStyleManager defaultStyleManager];
    NSString* textHighightDescriptor = [manager textHighlightDescriptor];
    
    self.intelligentPickup = YES;
    self.intelligence = YES;
    _searchingText = NO;
    _searchCaseSensitive = NO;
    _searchRegex = NO;
    _searchMarchWord = NO;
    _searchTimeout = FALSE;
    _replacingSearchedContent = NO;
    
    [_textView setDelegate:self];
    [_textView setTextAttributesDescriptor:textHighightDescriptor];
    [_lineNumberView setTextAttributesDescriptor:textHighightDescriptor];
    
    [_searchInput setDelegate:self];
    [_replaceInput setDelegate:self];
    
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
    
    [self setupConfiguration];
    [self showTextSearch:NO];
    [self setShowLineNumber:_showLineNumber];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textHighlightStyleResponse:) name:CEENotificationTextHighlightStyleUpdate object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationConfigurationChanged:) name:CEENotificationApplicationConfigurationChanged object:nil];
}

- (void)applicationConfigurationChanged:(NSNotification*)notification {
    [self setupConfiguration];
}

- (void)setupConfiguration {
    AppDelegate* delegate = [NSApp delegate];
    NSDictionary* configuration = [delegate configuration];
    
    if (configuration[CEEApplicationConfigurationNameLineWrap])
        [self setWrap:[configuration[CEEApplicationConfigurationNameLineWrap] boolValue]];
    else
        [self setWrap:NO];
    
    if (configuration[CEEApplicationConfigurationNameCaretBlinkTimeInterval]) {
        float value = [configuration[CEEApplicationConfigurationNameCaretBlinkTimeInterval] floatValue];
        if (value < 0.1 || value > 100)
            _textView.caretBlinkTimeInterval = 0.5;
        else
            _textView.caretBlinkTimeInterval = value;
    }
    else {
        _textView.caretBlinkTimeInterval = 0.5;
    }
    
    if (configuration[CEEApplicationConfigurationNameShowPageGuideLine])
        [_textView setEnablePageGuideLine:[configuration[CEEApplicationConfigurationNameShowPageGuideLine] boolValue]];
    else
        [_textView setEnablePageGuideLine:NO];
    
    if (configuration[CEEApplicationConfigurationNamePageGuideLineOffset])
        _textView.pageGuildLineOffset = [configuration[CEEApplicationConfigurationNamePageGuideLineOffset] integerValue];
    else
        _textView.pageGuildLineOffset = 80;
    
    if (configuration[CEEApplicationConfigurationNameShowLineNumber])
        self.showLineNumber = [configuration[CEEApplicationConfigurationNameShowLineNumber] boolValue];
    else
        self.showLineNumber = YES;
}

- (void)setEditable:(BOOL)flag {
    _textView.editable = flag;
}

- (void)setWrap:(BOOL)flag {
    _textView.wrap = flag;
    if (_textView.wrap)
        [self showHorizontalScroller:NO];
    else
        [self showHorizontalScroller:YES];
    
    [self updateReferences];
    [self styledText];
}

- (void)viewDidAppear {
    if ([self.view.window firstResponder] != _textView)
        [self.view.window makeFirstResponder:_textView];
    
    if (!self.buffer)
        return;
    
    [self updateLineNumberView];
    [self updatePortStatusInfo];
    [self adjustScrollers];
    [self updateReferences];
    [self styledText];
}

- (void)setBuffer:(CEESourceBuffer*)buffer {
    (void)self.view;
    [super setBuffer:buffer];
    
    if (!buffer)
        cee_text_edit_storage_set(_textView.edit, NULL);
    else
        cee_text_edit_storage_set(_textView.edit, buffer.storage);
    
    [self updateLineNumberView];
    [self updatePortStatusInfo];
    [self adjustScrollers];
    [self updateReferences];
    [self styledText];
}

- (void)showTextSearch:(BOOL)shown {
    if (shown)
        _textSearcherHeight.constant = 66.0;
    else
        _textSearcherHeight.constant = 0.0;
    [self.view updateConstraints];
}

- (void)setShowLineNumber:(BOOL)shown {
    _showLineNumber = shown;
    if (shown)
        _lineNumberViewWidthConstraint.constant = 60.0;
    else
        _lineNumberViewWidthConstraint.constant = 0.0;
    [self.view updateConstraints];
}

- (BOOL)showLineNumber {
    return _showLineNumber;
}

- (void)showHorizontalScroller:(BOOL)shown {
    if (shown)
        _horizontalScrollerHeight.constant = CEEHorizontalScrollerHeight;
    else
        _horizontalScrollerHeight.constant = 0.0;
    [self.view updateConstraints];
}

- (void)clearHighlightRanges {
    if (_highlightRanges)
        cee_list_free_full(_highlightRanges, cee_range_free);
    _highlightRanges = NULL;
}

- (void)clearSearchRanges {
    if (_searchRanges)
        cee_list_free_full(_searchRanges, cee_range_free);
    _searchRanges = NULL;
}

- (void)clearReferences {
    if (_references)
        cee_list_free_full(_references, cee_source_symbol_reference_free);
    _references = NULL;
}

- (void)dealloc {
    [self clearReferences];
    [self clearHighlightRanges];
    [self clearSearchRanges];
        
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)becomeFirstResponder {
    if ([self.view.window firstResponder] != _textView)
        [self.view.window makeFirstResponder:_textView];
    return YES;
}

- (void)adjustScrollers {
    _verticalScroller.floatValue = cee_text_edit_vertical_scroller_offset_get(_textView.edit);
    _verticalScroller.knobProportion = cee_text_edit_vertical_scroller_proportion_get(_textView.edit);
    _horizontalScroller.floatValue = cee_text_edit_horizontal_scroller_offset_get(_textView.edit);
    _horizontalScroller.knobProportion = cee_text_edit_horizontal_scroller_proportion_get(_textView.edit);
    
    // make sure horizontal scroller always draw the knob
    if (fabs(_horizontalScroller.knobProportion - 1.0) < FLT_EPSILON)
        _horizontalScroller.knobProportion = 0.99;
}

- (void)updateReferences {
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    CEERange range = cee_text_layout_range_get(layout);
    [self clearReferences];
    _references = [self.buffer referencesInRange:range];
}

- (void)styledText {
    AppDelegate* delegate = [NSApp delegate];
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    CEERange range = cee_text_layout_range_get(layout);
    CEEList* tags = NULL;
    
    if (self.port)
        tags = [self.buffer tagsInRange:range withDataBase:self.port.session.project.database];
    else
        tags = [self.buffer tagsInRange:range withDataBase:delegate.currentProject.database];
        
    cee_text_layout_styled(layout, range, tags);
    [_textView setNeedsDisplay:YES];
}

- (void)verticalScroll:(id)sender {
    cee_text_edit_scroll_vertical_to(_textView.edit, _verticalScroller.floatValue);
    _horizontalScroller.floatValue = cee_text_edit_horizontal_scroller_offset_get(_textView.edit);
    _horizontalScroller.knobProportion = cee_text_edit_horizontal_scroller_proportion_get(_textView.edit);
    [self updateLineNumberView];
    [self updateReferences];
    [self updateBufferReferenceContextPresentOffset];
    [self styledText];
}

- (void)horizontalScroll:(id)sender {
    cee_text_edit_scroll_horizontal_to(_textView.edit, _horizontalScroller.floatValue);
    [self updateLineNumberView];
    [self updateReferences];
    [self updateBufferReferenceContextPresentOffset];
    [self styledText];
}

- (IBAction)closeTextSearcher:(id)sender {
    _searchingText = !_searchingText;
    [self showTextSearch:_searchingText];
    [self.view.window makeFirstResponder:_textView];
    [_textView setHighLightSearched:_searchingText];
    [self updateLineNumberView];
    
    [self clearSearchRanges];
    [self clearHighlightRanges];
}

- (void)searchText {
    [self clearHighlightRanges];
    [self clearSearchRanges];
    
    CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
    const cee_uchar* buffer = cee_text_storage_buffer_get(storage);
    const cee_char* subject = (cee_char*)[_searchInput.stringValue UTF8String];
    
    if (_searchRegex) {
        _searchRanges = cee_regex_search((const cee_char*)buffer,
                                         subject,
                                         TRUE,
                                         10000,
                                         &_searchTimeout);
    }
    else {
        _searchRanges = cee_str_search((const cee_char*)buffer,
                                       subject,
                                       _searchCaseSensitive,
                                       _searchMarchWord);
    }
    
    if (_searchTimeout) {
        cee_text_edit_select_all(_searchInput.edit);
        cee_text_edit_insert(_searchInput.edit,
                             (const cee_uchar*)"search timeout");
        cee_text_edit_select_all(_searchInput.edit);
        _searchTimeout = FALSE;
    }
    
    cee_ulong nb_matched = cee_list_length(_searchRanges);
    _textSearchLabel.stringValue = [NSString stringWithFormat:@"%lu matches", nb_matched];
    _searchRangeIndex = -1;
    [_textView setNeedsDisplay:YES];
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
    NSUInteger n = cee_list_length(_searchRanges);
    
    if (!n)
        return;
    
    _searchRangeIndex --;
        
    if (_searchRangeIndex < 0)
        _searchRangeIndex = n - 1;
    
    [self highlightSearchWithIndex:_searchRangeIndex];
}

- (IBAction)nextSearched:(id)sender {
    NSUInteger n = cee_list_length(_searchRanges);
    
    if (!n)
        return;
    
    _searchRangeIndex ++;
    
    if (_searchRangeIndex >= n)
        _searchRangeIndex = 0;
    
    [self highlightSearchWithIndex:_searchRangeIndex];
}

- (IBAction)replaceSearched:(id)sender {
    if (!_searchRanges)
        return;
    
    if (_searchRangeIndex < 0 || _searchRangeIndex >= cee_list_length(_searchRanges))
        _searchRangeIndex = 0;
    
    _replacingSearchedContent = YES;
    
    const cee_uchar* str = (const cee_uchar*)[_replaceInput.stringValue UTF8String];
    CEERange replace = *(CEERange*)cee_list_nth_data(_searchRanges, (cee_int)_searchRangeIndex);;
    CEEList* replaces = cee_list_append(NULL, &replace);
    CEERange* range = NULL;
    CEEList* p = NULL;
    
    cee_text_edit_replace_ranges(_textView.edit, str, replaces);
    cee_list_free(replaces);
    
    [self.buffer setState:kCEESourceBufferStateShouldSyncToFile];
    _replacingSearchedContent = NO;
    
    if (_searchingText)
        [self searchText];
    
    if (_searchRanges) {
        _searchRangeIndex = 0;
        p = _searchRanges;
        while (p) {
            range = p->data;
            if (range->location > replace.location)
                break;
            
            _searchRangeIndex ++;
            
            p = p->next;
        }
        
        if (_searchRangeIndex >= cee_list_length(_searchRanges))
            _searchRangeIndex = 0;
        
        [self highlightSearchWithIndex:_searchRangeIndex];
    }
    
}

- (IBAction)replaceAllSearched:(id)sender {
    if (!_searchRanges)
        return;
    
    const cee_uchar* str = (const cee_uchar*)[_replaceInput.stringValue UTF8String];
    _replacingSearchedContent = YES;
    cee_text_edit_replace_ranges(_textView.edit, str, _searchRanges);
    [self.buffer setState:kCEESourceBufferStateShouldSyncToFile];
    _replacingSearchedContent = NO;
    if (_searchingText)
        [self searchText];
}

- (void)highlight:(CEEList*)ranges {
    [self clearHighlightRanges];
    _highlightRanges = cee_list_copy_deep(ranges, cee_range_list_copy, NULL);
    
    if (!_highlightRanges)
        return;
    
    CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
    CEERange* range0 = cee_list_nth_data(ranges, 0);
    
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    CEERange range = cee_text_layout_range_get(layout);
    if (!cee_location_in_range(range0->location, range)) {
        cee_long index = cee_text_storage_paragraph_index_get(storage, range0->location);
        cee_text_edit_scroll_to_pragraph(_textView.edit, index);
        [self adjustScrollers];
        [self updateLineNumberView];
    }
    
    cee_text_edit_caret_buffer_offset_set(_textView.edit, range0->location);
    
    [self styledText];
}

- (void)highlightSearchWithIndex:(NSInteger)index {
    if (index < 0)
        return;
    
    CEEList* p = cee_list_append(NULL, cee_list_nth_data(_searchRanges, (cee_uint)index));
    [self highlight:p];
}

- (CEEList*)textViewSearchRanges:(CEETextView*)textView {
    return _searchRanges;
}

- (CEEList*)textViewHighlightRanges:(CEETextView*)textView {
    return _highlightRanges;
}

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {
    CEESourceBuffer* buffer = notification.object;
    if (buffer != self.buffer)
        return;
        
    if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile]) {
        if (_searchingText && !_replacingSearchedContent)
            [self searchText];
        
        // if view is the first responder, that means buffer is modified by
        // this editor, we don't need to do anyting.
        if (self.view.window.firstResponder == _textView && [self.view.window isKeyWindow])
            return;
        
        cee_text_edit_modified_update(_textView.edit);
    }
    else {
        cee_text_edit_modified_update(_textView.edit);
    }
    
    [self adjustScrollers];
    [self updateLineNumberView];
    [self styledText];
}

- (void)textHighlightStyleResponse:(NSNotification*)notification {
    CEEStyleManager* manager = [CEEStyleManager defaultStyleManager];
    NSString* descriptor = [manager textHighlightDescriptor];
    [_textView setTextAttributesDescriptor:descriptor];
    [_lineNumberView setTextAttributesDescriptor:descriptor];
}

- (BOOL)filePathIsSource:(NSString*)filePath {
    NSString* extension = [filePath pathExtension];
    
    if ([extension compare:@"c" options:NSCaseInsensitiveSearch] == NSOrderedSame || 
        [extension compare:@"cpp" options:NSCaseInsensitiveSearch] == NSOrderedSame || 
        [extension compare:@"cc" options:NSCaseInsensitiveSearch] == NSOrderedSame || 
        [extension compare:@"m" options:NSCaseInsensitiveSearch] == NSOrderedSame || 
        [extension compare:@"mm" options:NSCaseInsensitiveSearch] == NSOrderedSame)
        return YES;
    return NO;
}

- (BOOL)filePathIsHeader:(NSString*)filePath {
    NSString* extension = [filePath pathExtension];
    if ([extension compare:@"h" options:NSCaseInsensitiveSearch] == NSOrderedSame ||
        [extension compare:@"hpp" options:NSCaseInsensitiveSearch] == NSOrderedSame )
        return YES;
    return NO;
}

- (IBAction)switchHeaderSource:(id)sender {
    NSString* name = nil;
    NSString* targetName = nil;
    NSArray* filePaths = nil;
    if ([self filePathIsSource:self.buffer.filePath]) {
        name = [[self.buffer.filePath lastPathComponent] stringByDeletingPathExtension];
        targetName = [name stringByAppendingPathExtension:@"h"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
        
        targetName = [name stringByAppendingPathExtension:@"hpp"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
    }
    else if ([self filePathIsHeader:self.buffer.filePath]) {
        name = [[self.buffer.filePath lastPathComponent] stringByDeletingPathExtension];
        targetName = [name stringByAppendingPathExtension:@"c"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
        
        targetName = [name stringByAppendingPathExtension:@"cpp"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
        
        targetName = [name stringByAppendingPathExtension:@"cc"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
        
        targetName = [name stringByAppendingPathExtension:@"m"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
        
        targetName = [name stringByAppendingPathExtension:@"mm"];
        filePaths = [self.port.session.project getReferenceFilePathsWithCondition:targetName];
        if (filePaths) {
            for (NSString* filePath in filePaths)
                [self.port openSourceBufferWithFilePath:filePath];
            return;
        }
    }
}

- (void)updateBufferReferenceContextPresentOffset {
    CEESourceBufferReferenceContext* reference = [self.port currentSourceBufferReference];
    CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    NSInteger index = cee_text_layout_paragraph_index_get(layout);
    NSInteger offset = cee_text_storage_buffer_offset_by_paragraph_index(storage, index);
    [reference setPresentBufferOffset:offset];
}

- (void)updateBufferReferenceContextFocusOffset {
    CEESourceBufferReferenceContext* reference = [self.port currentSourceBufferReference];
    NSInteger offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    [reference setFocusBufferOffset:offset];
}

- (void)updatePortStatusInfo {
    if (!self.buffer)
        [self.port setDescriptor:@""];
    
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    
    cee_long buffer_offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    cee_long paragraph = cee_text_edit_paragraph_index_by_buffer_offset_get(_textView.edit,
                                                                            buffer_offset);
    cee_long character = cee_text_edit_character_index_in_paragraph_by_buffer_offset_get(_textView.edit,
                                                                                         buffer_offset);
    CEETextStorageLineBreakType line_break_type = cee_text_storage_line_break_type_get(self.buffer.storage);
    NSString* sourceType = [sourceBufferManager sourceType:self.buffer];
    NSString* lineBreakTypeString = @"";
    NSString* encodedTypeString = @"";
    
    if (self.buffer.type == kCEESourceBufferTypeText) {
        const cee_uchar* bom = bom;
        cee_ulong bom_size = 0;
        CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
        cee_text_storage_bom_get(storage, &bom, &bom_size);
        if (bom) {
            encodedTypeString = [NSString stringWithFormat:@"%s with BOM", cee_codec_encoded_type_from_bom(bom, bom_size)];
        }
        else
            encodedTypeString = @"UTF-8";
    
    }
    else if (self.buffer.type == kCEESourceBufferTypeBinary) {
        encodedTypeString = @"Binary File";
    }
    
    if (line_break_type == kCEETextStorageLineBreakTypeCR)
        lineBreakTypeString = @"CR";
    else if (line_break_type == kCEETextStorageLineBreakTypeLF)
        lineBreakTypeString = @"LF";
    else if (line_break_type == kCEETextStorageLineBreakTypeCRLF)
        lineBreakTypeString = @"CRLF";
    
    NSString* infoString = [NSString stringWithFormat:@"Offset:%ld, Line:%ld, Column:%ld    %@    %@    %@",
                            buffer_offset,
                            paragraph + 1,
                            character + 1,
                            lineBreakTypeString,
                            encodedTypeString,
                            sourceType];
    
    [self.port setDescriptor:infoString];
}

- (void)updateLineNumberView {
    if (!self.buffer) {
        [_lineNumberView setLineNumberTags:nil];
        return;
    }
    
    NSMutableArray* tags = [[NSMutableArray alloc] init];
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
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
    
    [_lineNumberView setLineNumberTags:tags];
}

- (void)createContext {
    if (!self.intelligence)
        return;
    
    cee_long offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    CEETokenCluster* cluster =
        cee_token_cluster_search_by_buffer_offset(_references,
                                                  self.buffer.prep_directive_fragment,
                                                  self.buffer.statement_fragment,
                                                  offset);
    [self.port createContextByCluster:cluster];
    cee_token_cluster_free(cluster);
}

- (IBAction)jumpToSymbol:(id)sender {
    cee_long offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    CEETokenCluster* cluster =
        cee_token_cluster_search_by_buffer_offset(_references,
                                                  self.buffer.prep_directive_fragment,
                                                  self.buffer.statement_fragment,
                                                  offset);
    [self.port jumpToSymbolByCluster:cluster];
    cee_token_cluster_free(cluster);
    return;
}

- (IBAction)searchReferences:(id)sender {
    cee_long offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    CEETokenCluster* cluster =
        cee_token_cluster_search_by_buffer_offset(_references,
                                                  self.buffer.prep_directive_fragment,
                                                  self.buffer.statement_fragment,
                                                  offset);
    [self.port searchReferencesByCluster:cluster];
    cee_token_cluster_free(cluster);
    return;
}


- (void)setPresentBufferOffset:(NSInteger)offset {
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
    NSInteger paragraph = cee_text_storage_paragraph_index_get(storage, offset);
    cee_text_layout_paragraph_index_set(layout, paragraph);
    
    [self updatePortStatusInfo];
    [self adjustScrollers];
    [self updateReferences];
    [self adjustScrollers];
    [self styledText];
}

- (void)setFocusBufferOffset:(NSInteger)offset {
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
    NSInteger paragraph = cee_text_storage_paragraph_index_get(storage, offset);
    cee_text_layout_paragraph_index_set(layout, paragraph);
    cee_text_edit_caret_buffer_offset_set(_textView.edit, offset);
    
    [self updatePortStatusInfo];
    [self adjustScrollers];
    [self updateReferences];
    [self adjustScrollers];
    [self styledText];
}

- (NSInteger)presentBufferOffset {
    CEETextStorageRef storage = cee_text_edit_storage_get(_textView.edit);
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    NSInteger index = cee_text_layout_paragraph_index_get(layout);
    NSInteger offset = cee_text_storage_buffer_offset_by_paragraph_index(storage, index);
    return offset;
}

- (NSInteger)focusBufferOffset {
    NSInteger offset = cee_text_edit_caret_buffer_offset_get(_textView.edit);
    return offset;
}
#pragma mark - CEETextViewDelegate protocol

- (void)textViewTextChanged:(CEETextView*)textView {
    if (textView == _textView) {
        [self updatePortStatusInfo];
        [self adjustScrollers];
        [self updateLineNumberView];
        [self.buffer setState:kCEESourceBufferStateShouldSyncToFile];
        [self updateReferences];
        [self styledText];
        [self createContext];
    }
    else if (textView == _searchInput) {
        [self searchText];
    }
}

- (void)textViewScrolled:(CEETextView*)textView {
    if (textView == _textView) {
        [self adjustScrollers];
        [self updateReferences];
        [self updateBufferReferenceContextPresentOffset];
        [self updateLineNumberView];
        [self updatePortStatusInfo];
        [self styledText];
    }
}

- (void)textViewCaretSet:(CEETextView*)textView {
    if (textView == _textView) {
        [self clearHighlightRanges];
        [self updateBufferReferenceContextPresentOffset];
        [self updateBufferReferenceContextFocusOffset];
        [self updateLineNumberView];
        [self updatePortStatusInfo];
        [self styledText];
        [self createContext];
    }
}

- (void)textViewSelectionChangedWhenCaretMove:(CEETextView*)textView {
    if (textView == _textView) {
        [self adjustScrollers];
        [self updateLineNumberView];
        [self updatePortStatusInfo];
        [self styledText];
        [self createContext];
    }
}

- (void)textViewFrameChanged:(CEETextView*)textView {
    if (textView == _textView) {
        [self updateReferences];
        [self adjustScrollers];
        [self updateLineNumberView];
        [self styledText];
    }
}

- (void)textViewSelectionChanged:(CEETextView*)textView {
    if (textView == _textView) {
        [self adjustScrollers];
        [self updateLineNumberView];
        [self createContext];
    }
}

- (void)textViewSearchText:(CEETextView*)textView {
    if (textView == _textView) {
        if (!_searchingText) {
            _searchingText = YES;
            [self showTextSearch:_searchingText];
            [_textView setHighLightSearched:_searchingText];
            [self searchText];
            [self updateLineNumberView];
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
    [self jumpToSymbol:self];
    return;
}


- (void)textView:(CEETextView*)textView pickWordAtPoint:(NSPoint)point {
    if (textView != _textView || !self.intelligence || !self.intelligentPickup)
        return;
        
    CEETextLayoutRef layout = cee_text_edit_layout_get(_textView.edit);
    CEEPoint p = {
        point.x,
        point.y
    };
    CEETextUnitRef unit = cee_text_unit_get_by_location(layout, p);
    CEEList* ranges_ref = NULL;
    cee_long offset = cee_text_unit_buffer_offset_get(unit);
    CEETokenCluster* cluster =
        cee_token_cluster_search_by_buffer_offset(_references,
                                                  self.buffer.prep_directive_fragment,
                                                  self.buffer.statement_fragment,
                                                  offset);
    if (cluster) {
        if (cluster->type == kCEETokenClusterTypeReference) {
            CEESourceSymbolReference* reference = (CEESourceSymbolReference*)cluster->content_ref;
            ranges_ref = reference->ranges;
        }
        else if (cluster->type == kCEETokenClusterTypeSymbol) {
            CEESourceSymbol* symbol = (CEESourceSymbol*)cluster->content_ref;
            ranges_ref = symbol->ranges;
        }
        
        if (ranges_ref) {
            [self clearHighlightRanges];
            _highlightRanges = cee_list_copy_deep(ranges_ref, cee_range_list_copy, NULL);
            [_textView setNeedsDisplay:YES];
        }
        cee_token_cluster_free(cluster);
    }
    else {
        [self clearHighlightRanges];
        [_textView setNeedsDisplay:YES];
    }
}


- (void)textView:(CEETextView*)textView pickWordCompleteAtPoint:(NSPoint)point {
    if (textView != _textView || !self.intelligence || !self.intelligentPickup)
        return;
    
    [self clearHighlightRanges];
    [_textView setNeedsDisplay:YES];
}


- (void)textView:(CEETextView*)textView modifyMenu:(NSMenu**)menu {
    NSMenu* modify = *menu;
    if ([self filePathIsSource:self.buffer.filePath] ||
        [self filePathIsHeader:self.buffer.filePath]) {
        NSString* itemTitle = @"Switch Header/Source";
        BOOL found = NO;
        NSArray* items = [modify itemArray];
        for (NSMenuItem* item in items) {
            if ([item.title isEqualToString:itemTitle])
                found = YES;
        }
        
        if (!found) {
            NSMenuItem* item = [modify insertItemWithTitle:itemTitle action:@selector(switchHeaderSource:) keyEquivalent:@"" atIndex:0];
            [item setImage:[NSImage imageNamed:@"icon_file_switch_16x16"]];
        }
    }
}

- (void)textViewUserInterfaceUpdated:(CEETextView*)textView {
    [self styledText];
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
    if (!self.intelligentPickup)
        self.intelligentPickup = YES;
}

@end
