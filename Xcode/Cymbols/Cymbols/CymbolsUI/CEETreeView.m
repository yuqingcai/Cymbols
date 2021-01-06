//
//  CEETreeView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/8.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <objc/message.h>
#import "CEETreeView.h"

@interface Leaf : NSObject
@property (weak) Leaf* parent;
@property (strong) NSMutableArray<Leaf*>* children;
@property id item;
@property BOOL expanded;
@end

@implementation Leaf
@end

@interface Wrapper : NSObject
@property (strong) Leaf* leaf;
@property NSInteger level;
@end

@implementation Wrapper
@end



@interface CEETreeView()
@property (strong) NSMutableArray<Wrapper*>* wrappers;
@property (strong) Leaf* root;
@end

@implementation CEETreeView

@dynamic delegate;
@dynamic dataSource;

- (NSInteger)numberOfRows {
    return _wrappers.count;
}

- (void)reloadData {
    if (!self.dataSource || !self.delegate)
        return;
        
    _root = [[Leaf alloc] init];
    _root.children = [[NSMutableArray alloc] init];
    _root.parent = nil;
    
    NSInteger count = [self.dataSource treeView:self numberOfChildrenOfItem:nil];
    for (NSInteger i = 0; i < count; i ++) {
        Leaf* leaf = [[Leaf alloc] init];
        leaf.parent = _root;
        leaf.item = [self.dataSource treeView:self child:i ofItem:nil];
        [_root.children addObject:leaf];
    }
    _root.expanded = YES;
    
    NSMutableArray* wrappers = [[NSMutableArray alloc] init];
    [self expandLeaf:_root toWrappers:&wrappers level:0];
    _wrappers = wrappers;
    
    [super reloadData];
}

- (void)expandLeaf:(Leaf*)leaf toWrappers:(NSMutableArray**)wrappers level:(NSInteger)level {
    NSMutableArray* _wrappers = *wrappers;
    for (Leaf* child in leaf.children) {
        Wrapper* wrapper = [[Wrapper alloc] init];
        wrapper.leaf = child;
        wrapper.level = level;
        [_wrappers addObject:wrapper];
        if (child.expanded)
            [self expandLeaf:child toWrappers:wrappers level:level+1];
    }
}

- (BOOL)itemIsExpanded:(id)item {
    for (Wrapper* wrapper in _wrappers) {
        if (wrapper.leaf.item == item && wrapper.leaf.expanded)
            return YES;
    }
    return NO;
}

- (void)expandItem:(id)item {
    if ([self itemIsExpanded:item])
        return;
    
    Leaf* leaf = nil;
    for (Wrapper* wrapper in _wrappers) {
        if (wrapper.leaf.item == item) {
            leaf = wrapper.leaf;
            break;
        }
    }
    
    if (!leaf)
        return;
    
    if (leaf.children) {
        leaf.expanded = YES;
        NSMutableArray* wrappers = [[NSMutableArray alloc] init];
        [self expandLeaf:_root toWrappers:&wrappers level:0];
        _wrappers = wrappers;
        
    }
    else {
        if (![self.dataSource treeView:self isItemExpandable:leaf.item])
            return;
        
        leaf.children = [[NSMutableArray alloc] init];
        NSUInteger n = [self.dataSource treeView:self numberOfChildrenOfItem:leaf.item];
        for (NSInteger i = 0; i < n; i ++) {
            Leaf* sub = [[Leaf alloc] init];
            sub.parent = leaf;
            sub.item = [self.dataSource treeView:self child:i ofItem:leaf.item];
            [leaf.children addObject:sub];
        }
        
        leaf.expanded = YES;
        NSMutableArray* wrappers = [[NSMutableArray alloc] init];
        [self expandLeaf:_root toWrappers:&wrappers level:0];
        _wrappers = wrappers;
    }
        
    [self gridAdjustRows];
    [self tintedGridRowViews];
    [self adjustVerticalScroller];
    [self adjustHorizontalScroller];
    [self updateGrid];
}

- (void)collapseItem:(id)item {
    if (![self itemIsExpanded:item])
        return;
    
    Leaf* leaf = nil;
    for (Wrapper* wrapper in _wrappers) {
        if (wrapper.leaf.item == item) {
            leaf = wrapper.leaf;
            break;
        }
    }
    
    if (!leaf)
        return;
    
    if (leaf.children) {
        leaf.expanded = NO;
        NSMutableArray* wrappers = [[NSMutableArray alloc] init];
        [self expandLeaf:_root toWrappers:&wrappers level:0];
        _wrappers = wrappers;
    }
    
    [self gridAdjustRows];
    [self tintedGridRowViews];
    [self adjustVerticalScroller];
    [self adjustHorizontalScroller];
    [self updateGrid];
}

- (void)updateGrid {
    [super updateGrid];
    
    NSInteger row = self.firstRowIndex;
    for (NSInteger i = 0; i < self.grid.subviews.count; i ++) {
        Wrapper* wrapper = _wrappers[row];
        [self.grid setIndent:wrapper.level row:i];
        if ([self.dataSource treeView:self isItemExpandable:wrapper.leaf.item])
            [self.grid setExpandButtonAtRow:i];
        else
            [self.grid removeExpandButtonAtRow:i];
        row ++;
    }
}

- (__kindof NSView*)viewForColumn:(NSInteger)column row:(NSInteger)row {
    Wrapper* wrapper = _wrappers[row];
    return [self.delegate treeView:self viewForColumn:column item:wrapper.leaf.item];
}

- (NSString*)titleForColumn:(NSInteger)column {
   return [self.delegate treeView:self titleForColumn:column];
}

- (BOOL)dataSourceSupportDragAndDrop {
    return [self.dataSource respondsToSelector:@selector(treeView:writeRowsWithIndexes:toPasteboard:)];
}

- (BOOL)dataSourceWriteSelectedIndexesToPasteboard {
    return [self.dataSource treeView:self writeRowsWithIndexes:self.selectedRowIndexes toPasteboard:self.draggingSessionPasteboard];
}

- (NSArray*)selectedItems {
    if (!self.selectedRowIndexes.count)
        return nil;
    
    NSMutableArray* items = [[NSMutableArray alloc] init];
    NSInteger i = [self.selectedRowIndexes firstIndex];
    while (i != NSNotFound) {
        [items addObject: _wrappers[i].leaf.item];
        i = [self.selectedRowIndexes indexGreaterThanIndex:i];
    }
    return items;
}


#pragma mark - protocol NSDraggingDestination

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return NSDragOperationNone;
    
    if (!self.dataSource)
        return NSDragOperationNone;
    
    if ([self.dataSource respondsToSelector:@selector(treeView:validateDrop:proposedRow:proposedDropOperation:)])
        return [self.dataSource treeView:self validateDrop:sender proposedRow:(self.firstRowIndex + self.grid.subviews.count - 1)  proposedDropOperation:NSTableViewDropOn];
    
    return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return NO;
    
    if (!self.dataSource)
        return NO;
    
    if ([self.dataSource respondsToSelector:@selector(treeView:acceptDrop:row:dropOperation:)])
        return [self.dataSource treeView:self acceptDrop:sender row:(self.firstRowIndex + self.grid.subviews.count - 1) dropOperation:NSTableViewDropOn];
    
    return NO;
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return;
    return;
}

@end
