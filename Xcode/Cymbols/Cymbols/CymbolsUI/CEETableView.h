//
//  CEETableView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/8.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEEScroller.h"
#import "CEEGridView.h"

NS_ASSUME_NONNULL_BEGIN

@protocol CEETableViewDelegate, CEETableViewDataSource;

@interface CEETableViewHeader : CEEView
- (void)setColumnTitles:(NSArray*)titles;
@end

typedef NS_ENUM(NSInteger, CEETableViewColumnAutoresizingStyle) {
    kCEETableViewNoColumnAutoresizing,
    kCEETableViewUniformColumnAutoresizingStyle,
};

@interface CEETableView : CEEView <NSDraggingDestination, NSDraggingSource, NSPasteboardWriting>
@property (nullable, weak) id<CEETableViewDelegate> delegate;
@property (nullable, weak) id<CEETableViewDataSource> dataSource;
@property (strong) CEEScroller* verticalScroller;
@property (strong) CEEScroller* horizontalScroller;
@property (strong) CEETableViewHeader* header;
@property (strong) CEETableViewHeader* headerPadding;
@property (strong) CEEGridView* grid;
@property (strong) NSPasteboard* draggingSessionPasteboard;
@property BOOL enableDrawHeader;
@property (readonly) NSInteger selectedRow;
@property (readonly) NSIndexSet* selectedRowIndexes;
@property BOOL allowsMultipleSelection;
@property SEL action;
@property SEL doubleAction;
@property id target;
@property (readonly) NSInteger clickedRow;
@property CEETableViewColumnAutoresizingStyle columnAutoresizingStyle;
@property (readonly) NSInteger numberOfRows;
@property NSInteger numberOfColumns;
@property (strong) NSString* nibNameOfCellView;
@property NSInteger firstRowIndex;

- (CGFloat)rowHeight;
- (void)scrollRowToVisible:(NSInteger)row;
- (void)selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend;
- (void)setDraggingSourceOperationMask:(NSDragOperation)mask forLocal:(BOOL)isLocal;
- (void)setColumnWidth:(CGFloat)width atIndex:(NSInteger)column;
- (void)adjustHorizontalOffsetWithDelta:(CGFloat)delta;
- (__kindof NSView*)makeViewWithIdentifier:(NSUserInterfaceItemIdentifier)identifier;
- (void)reloadData;
- (void)reloadHeader;

@end

@protocol CEETableViewDelegate <NSObject>
@required
- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column;
- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row;
@end

@protocol CEETableViewDataSource <NSObject>
@required
- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView;
@optional
- (NSDragOperation)tableView:(CEETableView *)tableView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation;
- (BOOL)tableView:(CEETableView *)tableView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation;
- (BOOL)tableView:(CEETableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pasteboard;
@end

NS_ASSUME_NONNULL_END
