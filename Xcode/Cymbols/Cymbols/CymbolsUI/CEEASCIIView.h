//
//  CEEASCIIView.h
//  Cymbols
//
//  Created by qing on 2020/10/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "cee_ascii_edit.h"
#import "cee_ascii_layout.h"
#import "cee_ascii_storage.h"

NS_ASSUME_NONNULL_BEGIN

@class CEEASCIIView;

@protocol CEEASCIIViewDelegate <NSObject>
@optional
- (void)ASCIIViewScrolled:(CEEASCIIView*)ASCIIView;
- (void)ASCIIViewDragged:(CEEASCIIView*)ASCIIView;
- (void)ASCIIViewFrameChanged:(CEEASCIIView*)ASCIIView;
@end

@interface CEEASCIIView : CEEView
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
@property (readonly) CEEASCIIEditRef edit;
@property (weak) id<CEEASCIIViewDelegate> delegate;

- (void)setTextAttributesDescriptor:(NSString*)descriptor;
- (NSString*)textAttributesDescriptorFromUIContext;
@end

NS_ASSUME_NONNULL_END
