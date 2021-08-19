//
//  CEEBinaryEditViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2018/11/29.
//  Copyright © 2018 caiyuqing. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEBinaryEditViewController.h"
#import "CEEStyleManager.h"
#import "CEEJSONReader.h"
#import "CEETitleView.h"
#import "CEEScroller.h"
#import "CEEBinaryView.h"
#import "CEEASCIIView.h"
#import "CEELineNumberView.h"

@interface CEEBinaryEditViewController()
@property (weak) IBOutlet CEEBinaryView *binaryView;
@property (weak) IBOutlet CEEASCIIView *ASCIIView;
@property (weak) IBOutlet CEEScroller *verticalScroller;
@property (weak) IBOutlet CEEScroller *binaryScroller;
@property (weak) IBOutlet CEEScroller *ASCIIScroller;
@property (weak) IBOutlet CEELineNumberView *lineNumberView;
@property (weak) IBOutlet NSLayoutConstraint *lineNumberViewWidthConstraint;

@end

@implementation CEEBinaryEditViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CEEStyleManager* manager = [CEEStyleManager defaultStyleManager];
    NSString* textHighightDescriptor = [manager textHighlightDescriptor];
    
    [_binaryView setDelegate:self];
    [_binaryView setTextAttributesDescriptor:textHighightDescriptor];
    
    [_ASCIIView setDelegate:self];
    [_ASCIIView setTextAttributesDescriptor:textHighightDescriptor];
    
    [_lineNumberView setTextAttributesDescriptor:textHighightDescriptor];
        
    [_verticalScroller setDirection:kCEEScrollerDirectionVertical];
    [_verticalScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_verticalScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_verticalScroller setEnabled:YES];
    [_verticalScroller setTarget:self];
    [_verticalScroller setAction:@selector(verticalScroll:)];
    
    [_binaryScroller setDirection:kCEEScrollerDirectionHorizontal];
    [_binaryScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_binaryScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_binaryScroller setEnabled:YES];
    [_binaryScroller setTarget:self];
    [_binaryScroller setAction:@selector(binaryScroll:)];
    
    [_ASCIIScroller setDirection:kCEEScrollerDirectionHorizontal];
    [_ASCIIScroller setScrollerStyle:NSScrollerStyleOverlay];
    [_ASCIIScroller setKnobStyle:NSScrollerKnobStyleDefault];
    [_ASCIIScroller setEnabled:YES];
    [_ASCIIScroller setTarget:self];
    [_ASCIIScroller setAction:@selector(ASCIIScroll:)];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textHighlightStyleResponse:) name:CEENotificationTextHighlightStyleUpdate object:nil];
    
}

- (void)viewDidAppear {
    if ([self.view.window firstResponder] != _binaryView)
        [self.view.window makeFirstResponder:_binaryView];
    
    if (!self.buffer)
        return;
    
    [self adjustScrollers];
    [_binaryView setNeedsDisplay:YES];
    [_ASCIIView setNeedsDisplay:YES];
}

- (void)setBuffer:(CEESourceBuffer*)buffer {
    (void)self.view;
    [super setBuffer:buffer];
        
    if (!buffer) {
        cee_binary_edit_storage_set(_binaryView.edit, NULL);
        cee_ascii_edit_storage_set(_ASCIIView.edit, NULL);
    }
    else {
        cee_binary_edit_storage_set(_binaryView.edit, buffer.storage);
        cee_ascii_edit_storage_set(_ASCIIView.edit, buffer.storage);
    }
    
    [self adjustScrollers];
    [self.port setDescriptor:[self createPortDescriptor]];
    
    [self setLineNumberViewWidth];
}

- (void)setLineNumberViewWidth {
    CEEBinaryLayoutRef layout = cee_binary_edit_layout_get(_binaryView.edit);
    CEEBinaryStorageRef storage = cee_binary_edit_storage_get(_binaryView.edit);
    NSInteger size = cee_binary_storage_size_get(storage);
    NSInteger character_per_line = cee_binary_layout_characters_per_line(layout);
    NSInteger n = (((size + character_per_line) / character_per_line) - 1) * character_per_line;
    NSString* str = [NSString stringWithFormat:@" 0x%lX ", (long)n];
    NSMutableDictionary* attributes = [[NSMutableDictionary alloc] init];
    [attributes setValue:_lineNumberView.font forKey:NSFontAttributeName];
    NSAttributedString* drawingString = [[NSAttributedString alloc] initWithString:str attributes:attributes];
    NSSize drawingSize = [drawingString size];
    [_lineNumberViewWidthConstraint setConstant:drawingSize.width];
}

- (NSString*)createPortDescriptor {
    return @"Binary Preview";
}

- (void)textHighlightStyleResponse:(NSNotification*)notification {
    CEEStyleManager* manager = [CEEStyleManager defaultStyleManager];
    NSString* descriptor = [manager textHighlightDescriptor];
    [_binaryView setTextAttributesDescriptor:descriptor];
    [_ASCIIView setTextAttributesDescriptor:descriptor];
    [_lineNumberView setTextAttributesDescriptor:descriptor];
}

- (void)adjustScrollers {
    _verticalScroller.floatValue = cee_binary_edit_vertical_scroller_offset_get(_binaryView.edit);
    _verticalScroller.knobProportion = cee_binary_edit_vertical_scroller_proportion_get(_binaryView.edit);
    
    _binaryScroller.floatValue = cee_binary_edit_horizontal_scroller_offset_get(_binaryView.edit);
    _binaryScroller.knobProportion = cee_binary_edit_horizontal_scroller_proportion_get(_binaryView.edit);
    
    // make sure horizontal scroller always draw the knob
    if (fabs(_binaryScroller.knobProportion - 1.0) < FLT_EPSILON)
        _binaryScroller.knobProportion = 0.99;
    
    _ASCIIScroller.floatValue = cee_ascii_edit_horizontal_scroller_offset_get(_ASCIIView.edit);
    _ASCIIScroller.knobProportion = cee_ascii_edit_horizontal_scroller_proportion_get(_ASCIIView.edit);
    
    // make sure horizontal scroller always draw the knob
    if (fabs(_ASCIIScroller.knobProportion - 1.0) < FLT_EPSILON)
        _ASCIIScroller.knobProportion = 0.99;
}

- (void)verticalScroll:(id)sender {
    cee_binary_edit_scroll_vertical_to(_binaryView.edit, _verticalScroller.floatValue);
    [_binaryView setNeedsDisplay:YES];
        
    cee_ascii_edit_scroll_vertical_to(_ASCIIView.edit, _verticalScroller.floatValue);
    [_ASCIIView setNeedsDisplay:YES];
    
    [self adjustScrollers];
    [self updateBufferReferenceContextPresentOffset];
    [self updateLineNumberView];
}

- (void)binaryScroll:(id)sender {
    cee_binary_edit_scroll_horizontal_to(_binaryView.edit, _binaryScroller.floatValue);
    [_binaryView setNeedsDisplay:YES];
}

- (void)ASCIIScroll:(id)sender {
    cee_ascii_edit_scroll_horizontal_to(_ASCIIView.edit, _ASCIIScroller.floatValue);
    [_ASCIIView setNeedsDisplay:YES];
}

- (void)binaryViewScrolled:(CEEBinaryView*)binaryView {
    if (binaryView == _binaryView) {
        _verticalScroller.floatValue = cee_binary_edit_vertical_scroller_offset_get(_binaryView.edit);
        cee_ascii_edit_scroll_vertical_to(_ASCIIView.edit, _verticalScroller.floatValue);
        [_ASCIIView setNeedsDisplay:YES];
        [self updateBufferReferenceContextPresentOffset];
        [self updateLineNumberView];
    }
}

- (void)binaryViewDragged:(CEEBinaryView*)binaryView {
    
}

- (void)binaryViewFrameChanged:(CEEBinaryView*)binaryView {
    if (binaryView == _binaryView) {
        [self adjustScrollers];
        [self updateLineNumberView];
    }
}

- (void)ASCIIViewScrolled:(CEEASCIIView*)ASCIIView {
    if (ASCIIView == _ASCIIView) {
        _verticalScroller.floatValue = cee_ascii_edit_vertical_scroller_offset_get(_ASCIIView.edit);
        cee_binary_edit_scroll_vertical_to(_binaryView.edit, _verticalScroller.floatValue);
        [_binaryView setNeedsDisplay:YES];
        [self updateBufferReferenceContextPresentOffset];
        [self updateLineNumberView];
    }
}

- (void)ASCIIViewDragged:(CEEASCIIView*)ASCIIView {
    
}

- (void)ASCIIViewFrameChanged:(CEEASCIIView*)ASCIIView {
    if (ASCIIView == _ASCIIView) {
        [self adjustScrollers];
        [self updateLineNumberView];
    }
}

- (void)updateBufferReferenceContextPresentOffset {
    CEESourceBufferReferenceContext* reference = [self.port currentSourceBufferReference];
    CEEBinaryLayoutRef layout = cee_binary_edit_layout_get(_binaryView.edit);
    NSInteger index = cee_binary_layout_paragraph_index_get(layout);
    NSInteger offset = cee_binary_layout_buffer_offset_by_paragraph_index(layout, index);
    [reference setPresentBufferOffset:offset];
}

- (void)setPresentBufferOffset:(NSInteger)offset {
    CEEBinaryLayoutRef binary_layout = cee_binary_edit_layout_get(_binaryView.edit);
    NSInteger paragraph = cee_binary_layout_paragraph_index_by_buffer_offset(binary_layout, offset);
    
    CEEASCIILayoutRef ascii_layout = cee_ascii_edit_layout_get(_ASCIIView.edit);
    cee_binary_layout_paragraph_index_set(binary_layout, paragraph);
    cee_ascii_layout_paragraph_index_set(ascii_layout, paragraph);
    
    [self adjustScrollers];
    
    [_binaryView setNeedsDisplay:YES];
    [_ASCIIView setNeedsDisplay:YES];
}


- (void)updateLineNumberView {
    if (!self.buffer) {
        [_lineNumberView setLineNumberTags:nil];
        return;
    }
    
    NSMutableArray* tags = [[NSMutableArray alloc] init];
    CEEBinaryLayoutRef layout = cee_binary_edit_layout_get(_binaryView.edit);
    cee_uint character_per_line = cee_binary_layout_characters_per_line(layout);
    CEEList* p = cee_binary_layout_lines_get(layout);
    
    while (p) {
        NSRect rect;
        CEEBinaryLineRef line = p->data;
        CEERect bounds = cee_binary_line_bounds_get(line);
        cee_long index = cee_binary_line_paragraph_index_get(line);
        
        NSString* number = [NSString stringWithFormat:@"0x%0lX", index * character_per_line];
        rect = NSMakeRect(bounds.origin.x,
                          bounds.origin.y,
                          bounds.size.width,
                          bounds.size.height);
        [tags addObject:@{ @"number": number, @"bounds": NSStringFromRect(rect) }];
        p = p->next;
    }
    
    [_lineNumberView setLineNumberTags:tags];
}


@end
