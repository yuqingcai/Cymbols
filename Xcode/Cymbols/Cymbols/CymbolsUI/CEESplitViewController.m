//
//  CEESplitViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/5.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "CEESplitViewController.h"
#import "CEESplitView.h"
#import "CEEIdentifier.h"
#import "CEEStyleManager.h"


@implementation CEESplitViewController

- (void)initProperties {
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithNibName:(NSNibName)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEEUserInterfaceStyleConfiguration* configuration = [styleManager userInterfaceConfiguration];
    [self.view setStyleConfiguration:configuration];
    [self.splitView setStyleConfiguration:configuration];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateStyle) name:CEENotificationUserInterfaceStyleUpdate object:nil];
}

- (void)updateStyle {
    
}

- (CGFloat)horizontalDividerPositionByIndex:(NSInteger)index inItemsArray:(NSArray*)dicts {
    CGFloat offset = 0.0;
    NSInteger i = 0;
    
    for (i = 0; i <= index; i ++) {
        NSDictionary* dict = dicts[i];
        NSString* frameSizeString = dict[@"viewController"][@"view"][@"frameSize"];
        NSSize size = NSSizeFromString(frameSizeString);
        
        BOOL collapsed = NO;
        if ([dict[@"collapsed"] caseInsensitiveCompare:@"YES"] == NSOrderedSame)
            collapsed = YES;
        else
            collapsed = NO;
        
        if (!collapsed)
            offset += size.height + self.splitView.dividerThickness;
    }
    return offset;
}

- (CGFloat)verticalDividerPositionByIndex:(NSInteger)index inItemsArray:(NSArray*)dicts {
    CGFloat offset = 0.0;
    NSInteger i = 0;
    
    for (i = 0; i <= index; i ++) {
        NSDictionary* dict = dicts[i];
        NSString* frameSizeString = dict[@"viewController"][@"view"][@"frameSize"];
        NSSize size = NSSizeFromString(frameSizeString);
        BOOL collapsed = NO;
        
        if ([dict[@"collapsed"] caseInsensitiveCompare:@"YES"] == NSOrderedSame)
            collapsed = YES;
        else
            collapsed = NO;
        
        if (!collapsed)
            offset += size.width + self.splitView.dividerThickness;
    }
    
    return offset;
}

- (NSViewController*)viewControllerByIdentifier:(NSString*)identifier {
    for (NSSplitViewItem* item in [self splitViewItems]) {
        if ([[[item viewController] identifier] caseInsensitiveCompare:identifier] == NSOrderedSame) {
            return [item viewController];
        }
    }
    return nil;
}

- (void)toggleViewByIdentifier:(NSString*)identifier {
    for (NSSplitViewItem* item in [self splitViewItems]) {
        NSViewController* controller = [item viewController];
        NSUserInterfaceItemIdentifier itemIdentifier = [controller identifier];
        if ([itemIdentifier compare:identifier options:NSCaseInsensitiveSearch] == NSOrderedSame) {
            if ([item isCollapsed])
                [item setCollapsed:NO];
            else
                [item setCollapsed:YES];
        }
    }
}

- (void)showView:(BOOL)shown byIdentifier:(NSString*)identifier {
    BOOL collapsed = NO;
    if (shown)
        collapsed = NO;
    else
        collapsed = YES;
    
    for (NSSplitViewItem* item in [self splitViewItems]) {
        if ([[[item viewController] identifier] compare:identifier options:NSCaseInsensitiveSearch] == NSOrderedSame)
            [item setCollapsed:collapsed];
    }
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.view.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    serializing = [serializing stringByAppendingFormat:@"\"subviews\": ["];
    
    NSSplitViewItem* item = nil;
    NSViewController* controller = nil;
    for (NSInteger i = 0; i < self.splitViewItems.count; i ++) {
        item = self.splitViewItems[i];
        serializing = [serializing stringByAppendingFormat:@"{"];
        
        if (item.collapsed)
            serializing = [serializing stringByAppendingFormat:@"\"collapsed\": \"YES\","];
        else
            serializing = [serializing stringByAppendingFormat:@"\"collapsed\": \"NO\","];
        
        controller = [item viewController];
        if ([[controller class] conformsToProtocol:@protocol(CEESerialization)])
            serializing = [serializing stringByAppendingString:[(id<CEESerialization>)controller serialize]];
        else
            serializing = [serializing stringByAppendingFormat:@"\"%@\":{}", controller.view.identifier];
        
        serializing = [serializing stringByAppendingFormat:@"}"];
        if (i < self.splitViewItems.count - 1)
            serializing = [serializing stringByAppendingFormat:@","];
    }
    
    serializing = [serializing stringByAppendingFormat:@"]"];
    serializing = [serializing stringByAppendingFormat:@"}"];
    
    return serializing;
}

- (NSDictionary*)descriptorFromSubviews:(NSArray*)subviews withIdentifier:(NSString*)identifier {
    for (NSDictionary* subview in subviews) {
        if (subview[identifier])
            return subview;
    }
    return nil;
}

- (void)deserialize:(NSDictionary*)dict {
    NSArray* descroptors = dict[@"subviews"];
    NSDictionary* descriptor = nil;
    CGFloat offset = 0.0;
    NSSplitViewItem* item = nil;
    NSViewController* controller = nil;
    NSSize size;
    
    for (int i = 0; i < self.splitViewItems.count; i ++) {
        item = self.splitViewItems[i];
        controller = item.viewController;
        descriptor = [self descriptorFromSubviews:descroptors withIdentifier:controller.view.identifier];
        if (descriptor) {
            if ([descriptor[@"collapsed"] caseInsensitiveCompare:@"YES"] == NSOrderedSame)
                [item setCollapsed:YES];
            else
                [item setCollapsed:NO];
            
            if ([[controller class] conformsToProtocol:@protocol(CEESerialization)])
                [(id<CEESerialization>)controller deserialize:descriptor[controller.view.identifier]];
        }
    }
    
    offset = 0.0;
    for (int i = 0; i < self.splitViewItems.count - 1; i ++) {
        item = self.splitViewItems[i];
        controller = item.viewController;
        descriptor = [self descriptorFromSubviews:descroptors withIdentifier:controller.view.identifier];
        
        if (!item.collapsed) {
            size = NSSizeFromString(descriptor[controller.view.identifier][@"size"]);
            if (self.splitView.vertical)
                offset += size.width + self.splitView.dividerThickness;
            else
                offset = size.height + self.splitView.dividerThickness;

            [self.splitView setPosition:offset ofDividerAtIndex:i];
        }
    }
    
}
@end

