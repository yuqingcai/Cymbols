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

@class CEETableView;
@class CEETableRowView;

@protocol CEETableViewDelegate <NSObject>
@required
- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column;
- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row;
- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier;
@end

@protocol CEETableViewDataSource <NSObject>
@required
- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView;
@optional
- (NSDragOperation)tableView:(CEETableView *)tableView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation;
- (BOOL)tableView:(CEETableView *)tableView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation;
- (BOOL)tableView:(CEETableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pasteboard;
@end

@interface CEETableViewHeader : CEEView
@property NSUInteger columns;
@end

@interface CEETableRowView : CEEView
@property NSInteger index;
@end

@interface CEETableView : CEEView <NSDraggingDestination, NSDraggingSource, NSPasteboardWriting>

@property (strong) CEEScroller* verticalScroller;
@property (strong) CEEScroller* horizontalScroller;
@property (strong) CEETableViewHeader* header;
@property (strong) CEETableViewHeader* headerPadding;
@property (strong) CEEGridView* grid;

@property BOOL enableDrawHeader;
@property (readonly) NSInteger selectedRow;
@property (readonly) NSIndexSet* selectedRowIndexes;
@property (weak) id<CEETableViewDelegate> delegate;
@property (weak) id<CEETableViewDataSource> dataSource;
@property BOOL allowsMultipleSelection;
@property SEL action;
@property SEL doubleAction;
@property id target;
@property(readonly) NSInteger clickedRow;

- (void)reloadData;
- (__kindof NSView*)makeViewWithIdentifier:(NSUserInterfaceItemIdentifier)identifier;
- (void)setColumns:(NSUInteger)columns;
- (NSUInteger)columns;
- (CGFloat)rowHeight;
- (void)scrollRowToVisible:(NSInteger)row;
- (void)selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend;
- (void)setDraggingSourceOperationMask:(NSDragOperation)mask forLocal:(BOOL)isLocal;
- (NSInteger)tableRowIndexByLocation:(NSPoint)point;
@end

NS_ASSUME_NONNULL_END
