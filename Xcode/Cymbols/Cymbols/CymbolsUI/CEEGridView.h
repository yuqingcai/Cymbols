//
//  CEEGridView.h
//  Cymbols
//
//  Created by qing on 2020/7/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEEGridRowView : CEEView
@property NSInteger index;
@property CGFloat indent;
@property NSArray* columnWidths;
@property NSArray* columnOffsets;
@property (strong, readonly) NSArray* cellViews;
- (NSUInteger)numberOfCells;
- (void)setCellViews:(NSArray*)cellViews;
- (NSArray*)removeAllCellViews;
- (void)setExpandable:(BOOL)expandable;
- (void)setExpanded:(BOOL)expanded;
@end

@interface CEEGridView : CEEView
@property CGFloat rowSpacing;
@property (strong) NSColor* gridColor;
@property CGFloat gridWidth;
@property BOOL enableDrawHorizontalGrid;
@property BOOL enableDrawVerticalGrid;
@property NSUInteger numberOfColumns;
@property NSArray* columnWidths;
@property NSArray* columnOffsets;
@property (readonly) NSArray* rowViews;

- (CGFloat)rowHeight;
- (NSUInteger)numberOfRows;
- (void)appendRowViews:(NSArray*)rowViews;
- (NSArray*)removeRowViewsFromTail:(NSInteger)n;
- (NSArray*)removeAllRowViews;
- (__kindof NSView*)cellViewInRow:(NSUInteger)row column:(NSUInteger)column;

@end

NS_ASSUME_NONNULL_END
