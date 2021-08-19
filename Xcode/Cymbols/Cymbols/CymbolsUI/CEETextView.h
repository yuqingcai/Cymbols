//
//  CEETextView.h
//  Cymbols
//
//  Created by qing on 2019/11/6.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "CEEView.h"
#import "cee_lib.h"
#import "cee_text_edit.h"
#import "cee_text_layout.h"
#import "cee_text_storage.h"

NS_ASSUME_NONNULL_BEGIN

@class CEETextView;

IB_DESIGNABLE

@protocol CEETextViewDelegate <NSObject>
@optional
- (void)textViewTextChanged:(CEETextView*)textView;
- (void)textViewScrolled:(CEETextView*)textView;
- (void)textViewCaretSet:(CEETextView*)textView;
- (void)textViewSelectionChanged:(CEETextView*)textView;
- (void)textViewDragged:(CEETextView*)textView;
- (void)textViewSelectionChangedWhenCaretMove:(CEETextView*)textView;
- (void)textViewFrameChanged:(CEETextView*)textView;
- (void)textViewSearchText:(CEETextView*)textView;
- (void)textViewEscape:(CEETextView*)textView;
- (void)textViewNewLine:(CEETextView*)textView;
- (void)textViewSelectWithCommand:(CEETextView*)textView;
- (void)textView:(CEETextView*)textView pickWordAtPoint:(NSPoint)point;
- (void)textView:(CEETextView*)textView pickWordCompleteAtPoint:(NSPoint)point;
- (void)textView:(CEETextView*)textView modifyMenu:(NSMenu**)menu;
- (CEEList*)textViewSearchRanges:(CEETextView*)textView;
- (CEEList*)textViewHighlightRanges:(CEETextView*)textView;

@end

@class CEETextView;

@interface CEETextView : CEEView <NSTextInputClient>
@property (strong) IBInspectable NSString* stringValue;
@property (strong) NSColor* textBackgroundColorSelected;
@property (strong) NSColor* textBackgroundColorSelectedOutline;
@property (strong) NSColor* textBackgroundColorSearched;
@property (strong) NSColor* textBackgroundColorSearchedOutline;
@property (strong) NSColor* textBackgroundColorHighlight;
@property (strong) NSColor* textBackgroundColorHighlightOutline;
@property (strong) NSColor* textBackgroundColorMarked;
@property (strong) NSColor* textBackgroundColorMarkedOutline;
@property (strong) NSColor* textBackgroundColorMarkedSelected;
@property (strong) NSColor* textBackgroundColorMarkedSelectedOutline;
@property (strong) NSColor* pageGuideLineColor;
@property CEETextLayoutAlignment alignment;
@property (readonly) CEETextEditRef edit;
@property BOOL wrap;
@property BOOL editable;
@property BOOL highLightSearched;
@property BOOL enablePageGuideLine;
@property BOOL enableHighlight;
@property CGFloat caretBlinkTimeInterval;
@property (weak) id<CEETextViewDelegate> delegate;
@property NSInteger pageGuildLineOffset;

- (void)setTextAttributesDescriptor:(NSString*)descriptor;
- (NSString*)textAttributesDescriptorFromUIContext;
- (NSPoint)viewPointFromLayoutPoint:(NSPoint)point;

@end

NS_ASSUME_NONNULL_END
