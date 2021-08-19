//
//  ClassBinaryView.h
//  Cymbols
//
//  Created by qing on 2020/10/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "cee_binary_edit.h"
#import "cee_binary_layout.h"
#import "cee_binary_storage.h"

NS_ASSUME_NONNULL_BEGIN

@class CEEBinaryView;

@protocol CEEBinaryViewDelegate <NSObject>
@optional
- (void)binaryViewScrolled:(CEEBinaryView*)binaryView;
- (void)binaryViewDragged:(CEEBinaryView*)binaryView;
- (void)binaryViewFrameChanged:(CEEBinaryView*)binaryView;
@end

@interface CEEBinaryView : CEEView
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
@property (readonly) CEEBinaryEditRef edit;
@property (weak) id<CEEBinaryViewDelegate> delegate;

- (void)setTextAttributesDescriptor:(NSString*)descriptor;
@end

NS_ASSUME_NONNULL_END
