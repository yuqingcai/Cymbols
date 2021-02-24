//
//  CEETreeView.h
//  Cymbols
//
//  Created by caiyuqing on 2019/8/8.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEETableView.h"

NS_ASSUME_NONNULL_BEGIN

@protocol CEETreeViewDelegate, CEETreeViewDataSource;

@interface CEETreeView : CEETableView <NSDraggingDestination, NSDraggingSource, NSPasteboardWriting>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-property-type"

@property (nullable, weak) id <CEETreeViewDelegate> delegate;
@property (nullable, weak) id <CEETreeViewDataSource> dataSource;
@property BOOL autosaveExpandedItems;

#pragma clang diagnostic pop

- (void)expandItem:(id)item;
- (void)collapseItem:(id)item;
- (void)reloadItem:(id)item;
- (BOOL)itemIsExpanded:(id)item;
- (NSArray*)selectedItems;
- (NSString*)serialize;
- (void)deserialize:(NSDictionary*)dict;
@end

@protocol CEETreeViewDelegate <NSObject>
@required
- (NSString *)treeView:(CEETreeView *)treeView titleForColumn:(NSInteger)column;
- (CEEView*)treeView:(CEETreeView *)treeView viewForColumn:(NSInteger)column item:(id)item;
- (BOOL)treeView:(CEETreeView *)treeView shouldExpandItem:(nullable id)item;
- (BOOL)treeView:(CEETreeView *)treeView shouldCollapseItem:(nullable id)item;
@optional
- (id)treeView:(CEETreeView *)treeView itemForPersistentObject:(id)object;
- (id)treeView:(CEETreeView *)treeView persistentObjectForItem:(id)item;
@end

@protocol CEETreeViewDataSource <NSObject>
@required
- (id)treeView:(CEETreeView *)treeView child:(NSInteger)index ofItem:(nullable id)item;
- (BOOL)treeView:(CEETreeView *)treeView isItemExpandable:(nullable id)item;
- (NSInteger)treeView:(CEETreeView *)treeView numberOfChildrenOfItem:(nullable id)item;
@optional
- (NSDragOperation)treeView:(CEETreeView *)treeView validateDrop:(id<NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)dropOperation;
- (BOOL)treeView:(CEETreeView *)treeView acceptDrop:(id<NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)dropOperation;
- (BOOL)treeView:(CEETreeView *)treeView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pasteboard;
@end

NS_ASSUME_NONNULL_END
