//
//  CEEGridView.h
//  Cymbols
//
//  Created by qing on 2020/7/27.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, CEEGridRowViewStyle) {
    kCEEGridRowViewStylePlain,
    kCEEGridRowViewStyleHierarchical,
};

@interface CEEGridRowView : CEEView
@property NSInteger index;
@property CGFloat indent;
@property NSArray* columnWidths;
@property NSArray* columnOffsets;
@property (strong, readonly) NSArray* cellViews;
@property CEEGridRowViewStyle rowStyle;
@property BOOL expandable;
@property BOOL expanded;
@property id target;
@property SEL expandAction;

- (NSUInteger)numberOfCellViews;
- (void)setCellViews:(NSArray*)cellViews;
- (NSArray*)removeAllCellViews;
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
