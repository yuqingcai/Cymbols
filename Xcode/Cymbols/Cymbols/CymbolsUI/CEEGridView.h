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
@property (strong, readonly) NSArray* cellViews;
@property (strong, readonly) NSArray* accessories;
- (NSUInteger)numberOfCells;
- (NSUInteger)numberOfAccessories;
- (void)appendCellViews:(NSArray*)cellViews;
- (void)appendAccessories:(NSArray*)accessories;
- (NSArray*)removeCellViews;
- (NSArray*)removeAccessories;

@end

@interface CEEGridView : CEEView
@property CGFloat rowSpacing;
@property (strong) NSColor* gridColor;
@property CGFloat gridWidth;
@property BOOL enableDrawHorizontalGrid;
@property BOOL enableDrawVerticalGrid;
@property NSUInteger numberOfColumns;
@property NSArray* columnOffsets;
@property NSArray* columnWidths;
@property (readonly) NSArray* rowViews;

- (CGFloat)rowHeight;
- (NSUInteger)numberOfRows;
- (void)appendRowViews:(NSArray*)rowViews;
- (NSArray*)removeRowViews:(NSInteger)numberOfRowViews;
- (__kindof NSView*)cellViewInRow:(NSUInteger)row column:(NSUInteger)column;
- (void)setColumnOffsets:(NSArray *)columnOffsets;
- (void)setColumnWidths:(NSArray *)columnWidths;
- (void)setIndent:(CGFloat)indent row:(NSUInteger)row;
- (void)setExpandButtonAtRow:(NSUInteger)row;
- (void)removeExpandButtonAtRow:(NSUInteger)row;
@end

NS_ASSUME_NONNULL_END
