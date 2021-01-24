//
//  CEETreeView.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/8.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <objc/message.h>
#import "CEETreeView.h"
#import "CEETitlebarButton.h"

@interface Tree : NSObject
@property (weak) Tree* parent;
@property (strong) NSMutableArray<Tree*>* children;
@property id item;
@property BOOL expanded;
@end

@implementation Tree
@end

@interface Wrapper : NSObject
@property (strong) Tree* leaf;
@property NSInteger level;
@end

@implementation Wrapper
@end

@interface CEETreeView()
@property (strong) NSMutableArray<Wrapper*>* wrappers;
@property (strong) Tree* root;
@end

@implementation CEETreeView

@dynamic delegate;
@dynamic dataSource;

- (void)initProperties {
    [super initProperties];
}

- (NSInteger)numberOfRows {
    return _wrappers.count;
}

- (void)reloadData {
    if (!self.dataSource || !self.delegate)
        return;
    
    if (_root && _root.children.count) {
        Tree* root = [[Tree alloc] init];
        root.children = [[NSMutableArray alloc] init];
        root.parent = nil;
        if (_root.expanded)
            [self expandTree:root compareToTree:_root];
        _root = root;
    }
    else {
        _root = [[Tree alloc] init];
        _root.children = [[NSMutableArray alloc] init];
        _root.parent = nil;
        NSInteger count = [self.dataSource treeView:self numberOfChildrenOfItem:nil];
        for (NSInteger i = 0; i < count; i ++) {
            Tree* tree = [[Tree alloc] init];
            tree.parent = _root;
            tree.item = [self.dataSource treeView:self child:i ofItem:nil];
            [_root.children addObject:tree];
            [_root setExpanded:YES];
        }
    }
    
    NSMutableArray* wrappers = [[NSMutableArray alloc] init];
    [self tree:_root toList:&wrappers level:0];
    _wrappers = wrappers;
    
    [super reloadData];
}

- (void)reloadItem:(id)item {
    
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
    
    Tree* leaf = nil;
    NSInteger i = 0;
    NSUInteger n = 0;
    
    for (Wrapper* wrapper in _wrappers) {
        if (wrapper.leaf.item == item) {
            leaf = wrapper.leaf;
            break;
        }
        i ++;
    }
    
    if (!leaf)
        return;
    
    if (leaf.children) {
        leaf.expanded = YES;
        NSMutableArray* wrappers = [[NSMutableArray alloc] init];
        [self tree:_root toList:&wrappers level:0];
        _wrappers = wrappers;
    }
    else {
        if (![self.dataSource treeView:self isItemExpandable:leaf.item])
            return;
        
        leaf.children = [[NSMutableArray alloc] init];
        n = [self.dataSource treeView:self numberOfChildrenOfItem:leaf.item];
        for (NSInteger i = 0; i < n; i ++) {
            Tree* sub = [[Tree alloc] init];
            sub.parent = leaf;
            sub.item = [self.dataSource treeView:self child:i ofItem:leaf.item];
            [leaf.children addObject:sub];
        }
        
        leaf.expanded = YES;
        NSMutableArray* wrappers = [[NSMutableArray alloc] init];
        [self tree:_root toList:&wrappers level:0];
        _wrappers = wrappers;
    }
    
    [super reloadData];
}

- (void)collapseItem:(id)item {
    if (![self itemIsExpanded:item])
        return;
    
    Tree* leaf = nil;
    NSInteger i = 0;
    
    for (Wrapper* wrapper in _wrappers) {
        if (wrapper.leaf.item == item) {
            leaf = wrapper.leaf;
            break;
        }
        i ++;
    }
    
    if (!leaf)
        return;
    
    if (leaf.children) {
        NSMutableArray* wrappers = [[NSMutableArray alloc] init];
        leaf.expanded = NO;
        [self tree:_root toList:&wrappers level:0];
        _wrappers = wrappers;
    }
    
    [super reloadData];
}

- (BOOL)item:(id)item1 equalToItem:(id)item2 {
    if ([item1 isKindOfClass:[NSString class]] && [item2 isKindOfClass:[NSString class]])
        return [(NSString*)item1 isEqualToString:(NSString*)item2];
    else
        return item1 == item2;
    return NO;
}

- (void)expandTree:(Tree*)tree1 compareToTree:(Tree*)tree2 {
    NSInteger count = [self.dataSource treeView:self numberOfChildrenOfItem:tree1.item];
    tree1.expanded = YES;
    tree1.children = [[NSMutableArray alloc] init];
    
    for (NSInteger i = 0; i < count; i ++) {
        Tree* childOfTree1 = [[Tree alloc] init];
        childOfTree1.parent = tree1;
        childOfTree1.item = [self.dataSource treeView:self child:i ofItem:tree1.item];
        [tree1.children addObject:childOfTree1];
        
        for (Tree* childOfTree2 in tree2.children) {
            if (childOfTree2.expanded &&
                [self item:childOfTree1.item equalToItem:childOfTree2.item])
                [self expandTree:childOfTree1 compareToTree:childOfTree2];
        }
    }
}

- (void)tree:(Tree*)tree toList:(NSMutableArray**)wrappers level:(NSInteger)level {
    NSMutableArray* _wrappers = *wrappers;
    for (Tree* child in tree.children) {
        Wrapper* wrapper = [[Wrapper alloc] init];
        wrapper.leaf = child;
        wrapper.level = level;
        [_wrappers addObject:wrapper];
        if (child.expanded)
            [self tree:child toList:wrappers level:level+1];
    }
}

- (BOOL)isRowViewExpandable:(NSInteger)index {
    Wrapper* wrapper = _wrappers[index];
    return [self.dataSource treeView:self isItemExpandable:wrapper.leaf.item];
}

- (BOOL)isRowViewExpanded:(NSInteger)index {
    Wrapper* wrapper = _wrappers[index];
    return [self itemIsExpanded:wrapper.leaf.item];
}

- (NSInteger)rowViewIndent:(NSInteger)index {
    return _wrappers[index].level;
}

- (void)cellViewAddOffsetSubview:(NSView*)cellView {
    for (NSView* view in cellView.subviews) {
        NSRect rect  = view.frame;
        rect.origin.x += 8.0;
        [view setFrame:rect];
    }
}

- (void)cellViewMinusOffsetSubview:(NSView*)cellView {
    for (NSView* view in cellView.subviews) {
        NSRect rect  = view.frame;
        rect.origin.x -= 8.0;
        [view setFrame:rect];
    }
}

- (CEETitlebarButton*)cellViewHasbutton:(NSView*)cellView {
    for (NSView* view in cellView.subviews) {
        if ([view isKindOfClass:[CEETitlebarButton class]])
            return (CEETitlebarButton*)view;
    }
    return nil;
}

- (void)toggleItemExpand:(id)sender {
    NSInteger row = self.firstRowIndex;
    NSButton* button = (NSButton*)sender;
    Wrapper* wrapper = _wrappers[row + button.tag];
    if ([self itemIsExpanded:wrapper.leaf.item])
        [self collapseItem:wrapper.leaf.item];
    else
        [self expandItem:wrapper.leaf.item];
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
        return [self.dataSource treeView:self validateDrop:sender proposedRow:(self.firstRowIndex + self.grid.rowViews.count - 1)  proposedDropOperation:NSTableViewDropOn];
    
    return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return NO;
    
    if (!self.dataSource)
        return NO;
    
    if ([self.dataSource respondsToSelector:@selector(treeView:acceptDrop:row:dropOperation:)])
        return [self.dataSource treeView:self acceptDrop:sender row:(self.firstRowIndex + self.grid.rowViews.count - 1) dropOperation:NSTableViewDropOn];
    
    return NO;
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender {
    if ([sender draggingSource] == self)
        return;
    return;
}

@end
