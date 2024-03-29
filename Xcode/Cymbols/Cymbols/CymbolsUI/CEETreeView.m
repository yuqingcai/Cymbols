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
@property id item;
@property (weak) Tree* parent;
@property (strong) NSMutableArray<Tree*>* children;
@property BOOL expanded;
@end

@implementation Tree
@end

@interface ListWrapper : NSObject
@property (strong) Tree* tree;
@property NSInteger level;
@end

@implementation ListWrapper
@end

@interface CEETreeView()
@property (strong) NSMutableArray<ListWrapper*>* listWrappers;
@property (strong) Tree* root;
@end

@implementation CEETreeView

@dynamic delegate;
@dynamic dataSource;

- (void)initProperties {
    [super initProperties];
}

- (NSInteger)numberOfRows {
    return _listWrappers.count;
}

- (void)reloadData {
    if (!self.dataSource || !self.delegate)
        return;
    
    if (!_root) {
        NSInteger count = [self.dataSource treeView:self numberOfChildrenOfItem:nil];
        if (count) {
            _root = [[Tree alloc] init];
            _root.item = @"root";
            _root.children = [[NSMutableArray alloc] init];
            _root.expanded = YES;
            _root.parent = nil;
            
            for (NSInteger i = 0; i < count; i ++) {
                Tree* tree = [[Tree alloc] init];
                tree.parent = _root;
                tree.item = [self.dataSource treeView:self child:i ofItem:nil];
                [_root.children addObject:tree];
            }
        }
    }
    else {
        Tree* tree = [[Tree alloc] init];
        tree.parent = nil;
        if (_root.expanded)
            [self expandTree:tree compareToTree:_root];
        _root = tree;
        _root.item = @"root";
    }
    
    if (!_root)
        return;
    
    NSMutableArray* listWrappers = [[NSMutableArray alloc] init];
    [self tree:_root toList:&listWrappers level:0];
    _listWrappers = listWrappers;
    
    [super reloadData];
}

- (void)expandAction:(id)sender {
    NSPoint location = [NSEvent mouseLocation];
    location = [self.window convertPointFromScreen:location];
    location = [self convertPoint:location fromView:nil];
    NSInteger i = [self rowIndexByLocation:location];
    
    if (![self isRowViewExpandable:i])
        return;
    
    if ([self isRowViewExpanded:i])
        [self collapseItem:_listWrappers[i].tree.item];
    else
        [self expandItem:_listWrappers[i].tree.item];
    
}

- (void)reloadItem:(id)item {
    Tree* tree = nil;
    for (ListWrapper* listWrapper in _listWrappers) {
        if (listWrapper.tree.item == item) {
            tree = listWrapper.tree;
            break;
        }
    }
    
    if (!tree)
        return;
    
    Tree* reload = [[Tree alloc] init];
    reload.parent = tree.parent;
    reload.item = tree.item;
    if (tree.expanded)
        [self expandTree:reload compareToTree:tree];
    
    NSInteger i = 0;
    while (i < tree.parent.children.count) {
        if (tree.parent.children[i] == tree)
            break;
        i ++;
    }
    [tree.parent.children replaceObjectAtIndex:i withObject:reload];
    
    NSMutableArray* listWrappers = [[NSMutableArray alloc] init];
    [self tree:_root toList:&listWrappers level:0];
    _listWrappers = listWrappers;
    
    [super reloadData];
}

- (BOOL)itemIsExpanded:(id)item {
    for (ListWrapper* listWrapper in _listWrappers) {
        if (listWrapper.tree.item == item && listWrapper.tree.expanded)
            return YES;
    }
    return NO;
}

- (void)expandItem:(id)item {
    if ([self itemIsExpanded:item])
        return;
    
    Tree* tree = nil;
    NSInteger i = 0;
    NSUInteger n = 0;
    
    for (ListWrapper* listWrapper in _listWrappers) {
        if (listWrapper.tree.item == item) {
            tree = listWrapper.tree;
            break;
        }
        i ++;
    }
    
    if (!tree)
        return;
    
    if (tree.children) {
        tree.expanded = YES;
        NSMutableArray* listWrapper = [[NSMutableArray alloc] init];
        [self tree:_root toList:&listWrapper level:0];
        _listWrappers = listWrapper;
    }
    else {
        if (![self.dataSource treeView:self isItemExpandable:tree.item])
            return;
        
        tree.children = [[NSMutableArray alloc] init];
        n = [self.dataSource treeView:self numberOfChildrenOfItem:tree.item];
        for (NSInteger i = 0; i < n; i ++) {
            Tree* child = [[Tree alloc] init];
            child.parent = tree;
            child.item = [self.dataSource treeView:self child:i ofItem:tree.item];
            [tree.children addObject:child];
        }
        
        tree.expanded = YES;
        NSMutableArray* listWrapper = [[NSMutableArray alloc] init];
        [self tree:_root toList:&listWrapper level:0];
        _listWrappers = listWrapper;
    }
    
    [super reloadData];
}

- (void)collapseItem:(id)item {
    if (![self itemIsExpanded:item])
        return;
    
    Tree* tree = nil;
    NSInteger i = 0;
    
    for (ListWrapper* listWrapper in _listWrappers) {
        if (listWrapper.tree.item == item) {
            tree = listWrapper.tree;
            break;
        }
        i ++;
    }
    
    if (!tree)
        return;
    
    if (tree.children) {
        NSMutableArray* listWrappers = [[NSMutableArray alloc] init];
        tree.expanded = NO;
        [self tree:_root toList:&listWrappers level:0];
        _listWrappers = listWrappers;
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
            if (childOfTree2.expanded && [self item:childOfTree1.item equalToItem:childOfTree2.item])
                [self expandTree:childOfTree1 compareToTree:childOfTree2];
        }
    }
}

- (void)tree:(Tree*)tree toList:(NSMutableArray**)listWrappers level:(NSInteger)level {
    for (Tree* child in tree.children) {
        ListWrapper* listWrapper = [[ListWrapper alloc] init];
        listWrapper.tree = child;
        listWrapper.level = level;
        [*listWrappers addObject:listWrapper];
        if (child.expanded)
            [self tree:child toList:listWrappers level:level + 1];
    }
}

- (CEEGridRowViewStyle)rowGridRowViewStyle {
    return kCEEGridRowViewStyleHierarchical;
}

- (BOOL)isRowViewExpandable:(NSInteger)row {
    ListWrapper* listWrapper = _listWrappers[row];
    return [self.dataSource treeView:self isItemExpandable:listWrapper.tree.item];
}

- (BOOL)isRowViewExpanded:(NSInteger)row {
    ListWrapper* listWrapper = _listWrappers[row];
    return [self itemIsExpanded:listWrapper.tree.item];
}

- (NSInteger)rowViewIndent:(NSInteger)row {
    return _listWrappers[row].level;
}

- (__kindof NSView*)viewForColumn:(NSInteger)column row:(NSInteger)row {
    ListWrapper* listWrapper = _listWrappers[row];
    return [self.delegate treeView:self viewForColumn:column item:listWrapper.tree.item];
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
        [items addObject:_listWrappers[i].tree.item];
        i = [self.selectedRowIndexes indexGreaterThanIndex:i];
    }
    return items;
}

- (NSString*)jsonString:(Tree*)tree {
    
    id object = [self.delegate treeView:self persistentObjectForItem:tree.item];
    NSString* string = [NSString stringWithFormat:@"{\"%@\": {", object];
        
    if (tree.expanded)
        string = [string stringByAppendingFormat:@"\"expanded\": \"YES\","];
    else
        string = [string stringByAppendingFormat:@"\"expanded\": \"NO\","];
    
    string = [string stringByAppendingFormat:@"\"children\": ["];
    for (int i = 0; i < tree.children.count; i ++) {
        string = [string stringByAppendingFormat:@"%@", [self jsonString:tree.children[i]]];
        if (i < tree.children.count - 1)
            string = [string stringByAppendingFormat:@","];
    }
    string = [string stringByAppendingFormat:@"]"];
    
    string = [string stringByAppendingFormat:@"}"];
    string = [string stringByAppendingFormat:@"}"];
    
    return string;
}

- (NSString*)serialize {
    if (![self.delegate respondsToSelector:@selector(treeView:persistentObjectForItem:)])
        return nil;
    
    NSString* serializing = [NSString stringWithFormat:@"\"%@\": ", self.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"firstRowIndex\": %ld,", self.firstRowIndex];
    serializing = [serializing stringByAppendingFormat:@"\"content\": %@", [self jsonString:_root]];
    serializing = [serializing stringByAppendingFormat:@"}"];
        
    return serializing;
}

- (Tree*)createTreeFromDict:(NSDictionary*)dict parent:(Tree*)parent {
    NSString* item = dict.allKeys[0];
    NSDictionary* descriptor = dict[item];
    NSArray* children = descriptor[@"children"];
    
    Tree* tree = [[Tree alloc] init];
    tree.item = item;
    tree.parent = parent;
    tree.children = [[NSMutableArray alloc] init];
    
    if ([descriptor[@"expanded"] compare:@"YES" options:NSCaseInsensitiveSearch] == NSOrderedSame)
        tree.expanded = YES;
    else
        tree.expanded = NO;
    
    if (children && children.count) {
        for (NSDictionary* dict in children)
            [tree.children addObject:[self createTreeFromDict:dict parent:tree]];
    }
    
    return tree;
}

- (void)deserialize:(NSDictionary*)dict {
    _root = [self createTreeFromDict:dict parent:nil];
    NSMutableArray* listWrappers = [[NSMutableArray alloc] init];
    [self tree:_root toList:&listWrappers level:0];
    _listWrappers = listWrappers;
    [super reloadData];
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
