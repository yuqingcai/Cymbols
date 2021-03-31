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
- (void)textViewHighlightTokenCluster:(CEETextView*)textView;
- (void)textViewIgnoreTokenCluster:(CEETextView*)textView;
- (void)textView:(CEETextView*)textView modifyMenu:(NSMenu**)menu;
@end

@class CEETextView;

@interface CEETextView : CEEView <NSTextInputClient> {
@protected
    CEETextStorageRef _storage;
    CEETextEditRef _edit;
    BOOL _retain_storage;
}

@property (strong) NSColor* textBackgroundColorSelected;
@property (strong) NSColor* textBackgroundColorSelectedOutline;
@property (strong) NSColor* textBackgroundColorHighlight;
@property (strong) NSColor* textBackgroundColorHighlightOutline;
@property CEETextLayoutAlignment aligment;
@property (readonly) CEETextEditRef edit;
@property CEETextStorageRef _Nullable storage;
@property BOOL wrap;
@property BOOL editable;
@property BOOL intelligence;
@property BOOL highLightSearched;
@property CGFloat caretBlinkTimeInterval;

@property (weak) id<CEETextViewDelegate> delegate;
- (NSString*)textAttributesDescriptor;
- (void)setTextAttributesDescriptor:(NSString*)descriptor;
- (NSPoint)viewPointFromLayoutPoint:(NSPoint)point;

@end

NS_ASSUME_NONNULL_END
