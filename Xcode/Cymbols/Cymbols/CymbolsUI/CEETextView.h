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
- (void)textViewCaretMove:(CEETextView*)textView;
- (void)textViewSelectionChangedWhenCaretMove:(CEETextView*)textView;
- (void)textViewFrameChanged:(CEETextView*)textView;
- (void)textViewSearchText:(CEETextView*)textView;
- (void)textViewEscape:(CEETextView*)textView;
- (void)textViewMouseUp:(CEETextView*)textView;
- (void)textViewMouseDown:(CEETextView*)textView;
- (void)textViewNewLine:(CEETextView*)textView;
@end

@class CEETextView;

@interface CEETextView : CEEView <NSTextInputClient> {
@protected
    CEETextStorageRef _storage;
    CEETextEditRef _edit;
    BOOL _retain_storage;
}

@property NSColor* caretColor;
@property NSColor* caretColorMarked;
@property NSColor* textBackgroundColorSelected;
@property NSColor* textBackgroundColorSelectedOutline;
@property NSColor* textBackgroundColorMarked;
@property NSColor* textBackgroundColorMarkedOutline;
@property NSColor* textBackgroundColorSearched;
@property NSColor* textBackgroundColorSearchedOutline;
@property NSColor* textBackgroundColorHighlight;
@property NSColor* textBackgroundColorHighlightOutline;

@property CEETextLayoutAlignment aligment;
@property (readonly) CEETextEditRef edit;
@property (readonly) CEETextStorageRef storage;
@property BOOL editable;
@property BOOL highLightSearched;

@property (weak) id<CEETextViewDelegate> delegate;
- (NSString*)textAttributesDescriptor;
- (void)setTextAttributesDescriptor:(NSString*)descriptor;
- (void)setStorage:(CEETextStorageRef __nullable)storage;
- (NSPoint)viewPointFromLayoutPoint:(NSPoint)point;

@end

NS_ASSUME_NONNULL_END
