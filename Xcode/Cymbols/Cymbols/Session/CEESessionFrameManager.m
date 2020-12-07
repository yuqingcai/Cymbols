//
//  CEESessionFrameManager.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/5.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEStyleManager.h"
#import "CEEIdentifier.h"
#import "CEESessionFrameManager.h"
#import "CEESourceBufferManagerViewController.h"


@interface CEESessionFrameManager ()
@property (strong) NSMutableArray<NSView*>* splitViews;
@property (strong) CEESessionFrameViewController* selectedFrame;
@property (strong) NSWindowController* sourceBufferManagerWindowController;
@end

@implementation CEESessionFrameManager

@synthesize session = _session;

- (void)initProperties {
    [super initProperties];
    _frameAttachable = YES;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    _splitViews = [[NSMutableArray alloc] init];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [(CEESessionFrameManagerView*)self.view setDelegate:self];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activeSourceBufferResponse:) name:CEENotificationSessionPortActiveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(presentHistoryResponse:) name:CEENotificationSessionPortPresentHistory object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deletePortResponse:) name:CEENotificationSessionDeletePort object:nil];
}

- (CEESessionFrameSplitView*)createSplitViewWithFrame:(NSRect)theFrame vertical:(BOOL)vertical splitViewDelegate:(id<NSSplitViewDelegate>)splitViewDelegate {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    CEESessionFrameSplitView * splitView = [[CEESessionFrameSplitView alloc] initWithFrame:theFrame];
    
    [splitView setStyleState:kCEEViewStyleStateActived];
    [splitView setDelegate:splitViewDelegate];
    [splitView setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [splitView becomeFirstResponder];
    [splitView setIdentifier:CreateObjectID(splitView)];
    [splitView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [splitView setDividerStyle:NSSplitViewDividerStyleThin];
    [splitView setVertical:vertical];
    
    return splitView;
}

- (void)addView:(NSView*)subview toView:(NSView*)view {
    [view addSubview:subview];
    NSDictionary *views = @{@"subview" : subview,};
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[subview]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[subview]-0-|" options:0 metrics:nil views:views];
    [view addConstraints:constraintsH];
    [view addConstraints:constraintsV];
}

- (NSView*)removeView:(NSView*)view fromView:(NSView*)superview {
    NSMutableArray* constraints = nil;
    
    constraints = [[NSMutableArray alloc] init];
    for (NSLayoutConstraint *constraint in superview.constraints)
        [constraints addObject:constraint];
    for (NSLayoutConstraint *constraint in constraints) {
        if (constraint.firstItem == view || constraint.secondItem == view) {
            [superview removeConstraint:constraint];
        }
    }
    
    constraints = [[NSMutableArray alloc] init];
    for (NSLayoutConstraint *constraint in view.constraints)
        [constraints addObject:constraint];
    for (NSLayoutConstraint *constraint in constraints) {
        if (constraint.firstItem == superview || constraint.secondItem == superview) {
            [view removeConstraint:constraint];
        }
    }
    
    [view removeFromSuperview];
    
    return view;
}

- (void)setHoldingPrioritiesInSplitView:(NSSplitView*)splitView {
    for (NSInteger i = 0; i < splitView.subviews.count; i ++)
        [splitView setHoldingPriority:250 forSubviewAtIndex:i];
}

- (void)setDividersPositionInSplitView:(NSSplitView*)splitView {
    CGFloat position;
    CGFloat distance;
    
    if (splitView.vertical)
        distance = (splitView.frame.size.width - (splitView.dividerThickness * (splitView.subviews.count - 1)))/splitView.subviews.count;
    else
        distance = (splitView.frame.size.height - (splitView.dividerThickness * (splitView.subviews.count - 1)))/splitView.subviews.count;
    
    for (NSInteger i = 0; i < splitView.subviews.count - 1; i ++) {
        position = distance + ((distance + splitView.dividerThickness) * i);
        [splitView setPosition:position ofDividerAtIndex:i];
    }
}

- (NSView*)splitView:(NSView*)view withView:(NSView*)newView vertical:(BOOL)vertical order:(NSWindowOrderingMode)order {
    
    NSView* superview = view.superview;
    
    if (![superview isKindOfClass:[NSSplitView class]]) {
        [self removeView:view fromView:superview];
        NSSplitView* splitView = [self createSplitViewWithFrame:view.frame vertical:vertical splitViewDelegate:self];
        [_splitViews addObject:splitView];
        [self addView:splitView toView:superview];
        
        if (order == NSWindowAbove) {
            [splitView addSubview:newView];
            [splitView addSubview:view];
        }
        else if (order == NSWindowBelow){
            [splitView addSubview:view];
            [splitView addSubview:newView];
        }
        
        [self setHoldingPrioritiesInSplitView:splitView];
        [self setDividersPositionInSplitView:splitView];
        [splitView adjustSubviews];
    }
    else {
        NSSplitView* splitView = (NSSplitView*)superview;
        NSSplitView* newSplitView = nil;
        NSMutableArray *subviews = [[NSMutableArray alloc] init];
        NSInteger replaceIndex = 0;
        for (NSInteger i = 0; i < splitView.subviews.count; i ++) {
            [subviews addObject:splitView.subviews[i]];
            if (splitView.subviews[i] == view)
                replaceIndex = i;
        }
        
        for (NSInteger i = 0; i < subviews.count; i ++)
            [subviews[i] removeFromSuperview];
        
        newSplitView = [self createSplitViewWithFrame:view.frame vertical:vertical splitViewDelegate:self];
        [_splitViews addObject:newSplitView];
        subviews[replaceIndex] = newSplitView;
        for (NSInteger i = 0; i < subviews.count; i ++)
            [splitView addSubview:subviews[i]];
        
        if (order == NSWindowAbove) {
            [newSplitView addSubview:newView];
            [newSplitView addSubview:view];
        }
        else if (order == NSWindowBelow){
            [newSplitView addSubview:view];
            [newSplitView addSubview:newView];
        }
        [self setHoldingPrioritiesInSplitView:newSplitView];
        [self setDividersPositionInSplitView:newSplitView];
        [newSplitView adjustSubviews];
    }
    
    for (NSViewController* controller in self.childViewControllers)
        [controller.view setNeedsDisplay:YES];
    
    return newView;
}

- (NSView*)splitView:(NSSplitView*)splitView replace:(NSView*)view with:(NSView*)newView {
    NSMutableArray *subviews = [[NSMutableArray alloc] init];
    NSInteger replaceIndex = 0;
    for (NSInteger i = 0; i < splitView.subviews.count; i ++) {
        [subviews addObject:splitView.subviews[i]];
        if (splitView.subviews[i] == view)
            replaceIndex = i;
    }
    
    for (NSInteger i = 0; i < subviews.count; i ++)
        [subviews[i] removeFromSuperview];
    subviews[replaceIndex] = newView;
    
    CGRect rect = view.frame;
    [newView setFrame:rect];
    
    for (NSInteger i = 0; i < subviews.count; i ++)
        [splitView addSubview:subviews[i]];
    
    [splitView adjustSubviews];
    
    return view;
}

- (NSView*)collapseView:(NSView*)view {
    NSView *superview = [view superview];
    
    if (![superview isKindOfClass:[NSSplitView class]]) {
        [self removeView:view fromView:superview];
    }
    else {
        [view removeFromSuperview];
        
        if (superview.subviews.count == 1) {
            
            NSView* subviewLeft = superview.subviews[0];
            
            if (![superview.superview isKindOfClass:[NSSplitView class]]) {
                NSView* supperSupperView = superview.superview;
                [self removeView:superview fromView:supperSupperView];
                [self addView:subviewLeft toView:supperSupperView];
            }
            else {
                NSSplitView* supperSupperView = (NSSplitView*)superview.superview;
                
                NSMutableArray *subviewsInSupperSupperView = [[NSMutableArray alloc] init];
                NSInteger replaceIndex = 0;
                for (NSInteger i = 0; i < supperSupperView.subviews.count; i ++) {
                    [subviewsInSupperSupperView addObject:supperSupperView.subviews[i]];
                    if (supperSupperView.subviews[i] == superview)
                        replaceIndex = i;
                }
                
                for (NSInteger i = 0; i < subviewsInSupperSupperView.count; i ++)
                    [subviewsInSupperSupperView[i] removeFromSuperview];
                
                subviewsInSupperSupperView[replaceIndex] = subviewLeft;
                
                for (NSInteger i = 0; i < subviewsInSupperSupperView.count; i ++)
                    [supperSupperView addSubview:subviewsInSupperSupperView[i]];
                
                [self setDividersPositionInSplitView:supperSupperView];
                [supperSupperView adjustSubviews];
            }
            
            [_splitViews removeObject:superview];
        }
        else {
            NSSplitView* splitView = (NSSplitView*)superview;
            [self setDividersPositionInSplitView:splitView];
            [splitView adjustSubviews];
        }
    }
    
    return view;
}

- (BOOL)findViewOfFrame:(CEESessionFrameViewController*)frame {
    for (NSView* view in self.view.subviews) {
        if (view == frame.view)
            return YES;
    }
    
    for (NSSplitView* splitView in _splitViews) {
        for (NSView* view in splitView.subviews) {
            if (view == frame.view)
                return YES;
        }
    }
    return NO;
}

- (void)attach:(CEESessionFrameViewController*)frame0 at:(CEEViewRegion)region relateTo:(CEESessionFrameViewController*)frame1 {
    
    if ([self findViewOfFrame:frame0])
        [self collapseView:frame0.view];
    
    if (region == kCEEViewRegionLeft) {
        [self splitView:frame1.view withView:frame0.view vertical:YES order:NSWindowAbove];
    }
    else if (region == kCEEViewRegionTop) {
        [self splitView:frame1.view withView:frame0.view vertical:NO order:NSWindowAbove];
    }
    else if (region == kCEEViewRegionRight) {
        [self splitView:frame1.view withView:frame0.view vertical:YES order:NSWindowBelow];
    }
    else if (region == kCEEViewRegionBottom) {
        [self splitView:frame1.view withView:frame0.view vertical:NO order:NSWindowBelow];
    }
    else if (region == kCEEViewRegionEntire) {
        NSView* superview = frame1.view.superview;
        NSInteger i = 0;
        if (![superview isKindOfClass:[NSSplitView class]]) {
            [self removeView:frame1.view fromView:superview];
            [self addView:frame0.view toView:superview];
        }
        else {
            [self splitView:(NSSplitView*)superview replace:frame1.view with:frame0.view];
            [(NSSplitView*)superview adjustSubviews];
        }
        
        while (i < self.childViewControllers.count) {
            if (frame1 == self.childViewControllers[i])
                break;
            i ++;
        }
        
        [self removeChildViewControllerAtIndex:i];
        [_session deletePort:frame1.port];
    }
}

- (void)split:(CEESplitFrameDirection)direction withFrame:(CEESessionFrameViewController*)frame {   
    
    [self addChildViewController:frame];
    
    if (!self.viewLoaded)
        (void)self.view; // force the view loading!!

    if (!self.view.subviews.count) {
        [self addView:frame.view toView:self.view];
    }
    else {
        if (direction == kCEESplitFrameDirectionVertical)
            [self splitView:_selectedFrame.view withView:frame.view vertical:YES order:NSWindowBelow];
        else
            [self splitView:_selectedFrame.view withView:frame.view vertical:NO order:NSWindowBelow];
    }
}

-(CEESessionFrameViewController*)createFrame {
    CEESessionFrameViewController* frame = [[NSStoryboard storyboardWithName:@"Session" bundle:nil] instantiateControllerWithIdentifier:@"IDSessionFrameViewController"];
    if (!frame)
        return nil;
    [frame setIdentifier:CreateObjectID(frame)];
    [frame.view setIdentifier:CreateObjectID(frame.view)];
    [frame setManager:self];
    return frame;
}

- (void)closeFrame:(CEESessionFrameViewController*)frame {
    __block BOOL shouldClose = YES;
    NSMutableArray* syncBuffers = nil;
    for (CEESourceBuffer* buffer in frame.port.openedSourceBuffers) {
        if ([buffer stateSet:kCEESourceBufferStateShouldSyncWhenClose]) {
            if (!syncBuffers)
                syncBuffers = [[NSMutableArray alloc] init];
            [syncBuffers addObject:buffer];
        }
    }
    
    if (syncBuffers) {
        if (!_sourceBufferManagerWindowController)
            _sourceBufferManagerWindowController = [[NSStoryboard storyboardWithName:@"SourceBufferManager" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceBufferManagerWindowController"];
        CEESourceBufferManagerViewController* controller = (CEESourceBufferManagerViewController*)_sourceBufferManagerWindowController.contentViewController;
        [controller setModifiedSourceBuffers:syncBuffers];
        [self.view.window beginSheet:_sourceBufferManagerWindowController.window completionHandler:(^(NSInteger result) {
            if (result == NSModalResponseCancel)
                shouldClose = NO;
            else
                shouldClose = YES;
            [NSApp stopModalWithCode:result];
            [controller setModifiedSourceBuffers:nil];
        })];
        [NSApp runModalForWindow:self.view.window];
    }
    
    if (!shouldClose)
        return;
    
    NSInteger i = 0;
    while (i < self.childViewControllers.count) {
        if (frame == self.childViewControllers[i]) {
            [self collapseView:frame.view];
            break;
        }
        i ++;
    }
    
    [[NSNotificationCenter defaultCenter] removeObserver:self.childViewControllers[i]];
    [self removeChildViewControllerAtIndex:i];
    [_session deletePort:frame.port];
    
    if (self.childViewControllers.count)
        [self selectFrame:self.childViewControllers.lastObject];
    else
        _selectedFrame = nil;
}

- (void)selectFrame:(CEESessionFrameViewController*)frame {
    
    for (CEESessionFrameViewController* controller in self.childViewControllers)
        [controller deselect];
    
    _selectedFrame = frame;
    [_selectedFrame select];
    [_session setActivedPort:_selectedFrame.port];
}

- (void)deriveFrameWithFilePaths:(NSArray*)filePaths attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameViewController*)frame {
    CEESessionFrameViewController* newFrame = nil;
    CEESessionPort* port = nil;
    
    if (region == kCEEViewRegionNone || !frame)
        return;
    
    [self.session.project addSecurityBookmarksWithFilePaths:filePaths];
    
    if (region == kCEEViewRegionEntire) {
        [frame.port openSourceBuffersWithFilePaths:filePaths];
        return;
    }
    else {
        newFrame = [self createFrame];
        if (!newFrame)
            return;
        
        port = [_session createPort];
        if (!port)
            return;
        
        [newFrame setPort:port];
        [newFrame setIdentifier:[self frame:newFrame identifierFromPort:port]];
        [newFrame.view setIdentifier:[self frameView:newFrame.view identifierFromPort:port]];
        [self addChildViewController:newFrame];
        [self attach:newFrame at:region relateTo:frame];
        
        [newFrame.port openSourceBuffersWithFilePaths:filePaths];        
        [self selectFrame:newFrame];
    }
}

- (void)moveFrameView:(CEESessionFrameView*)view0 attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameView*)view1 {
    CEESessionFrameViewController* frame0 = nil;
    CEESessionFrameViewController* frame1 = nil;
    
    for (CEESessionFrameViewController* frame in self.childViewControllers) {
        if (frame.view == view0)
            frame0 = frame;
        
        if (frame.view == view1)
            frame1 = frame;
    }
    
    if (frame0 && frame1)
        [self attach:frame0 at:region relateTo:frame1];
    [self selectFrame:frame0];
}

- (void)split:(CEESplitFrameDirection)direction {
    CEESessionFrameViewController* frame = nil;
    CEESessionPort* port = nil;
    NSArray* duplicatedBuffers = nil;
    CEESourceBuffer* activedSourceBuffer = nil;
    
    activedSourceBuffer = [_session.activedPort activedSourceBuffer];
    if (!activedSourceBuffer)
        return;
    
    frame = [self createFrame];
    if (!frame)
        return;
    
    [self split:direction withFrame:frame];
    
    port = [_session createPort];
    if (!port)
        return;
        
    [frame setPort:port];
    [frame setIdentifier:[self frame:frame identifierFromPort:port]];
    [frame.view setIdentifier:[self frameView:frame.view identifierFromPort:port]];
    
    @autoreleasepool {
        NSMutableArray* filePaths = [[NSMutableArray alloc] init];
        for (CEESourceBuffer* buffer in [_session.activedPort openedSourceBuffers]) {
            [filePaths addObject:buffer.filePath];
        }
        duplicatedBuffers = [frame.port openSourceBuffersWithFilePaths:filePaths];
        if (duplicatedBuffers) {
            if ([duplicatedBuffers containsObject:activedSourceBuffer])
                [frame.port setActivedSourceBuffer:activedSourceBuffer];
            else
                [frame.port setActivedSourceBuffer:duplicatedBuffers[0]];
            [self selectFrame:frame];
        }
    }
    
    //if ([activedSourceBuffer stateSet:kCEESourceBufferStateFileTemporary]) {
    //    activedSourceBuffer = [frame.port openUntitledSourceBuffer];
    //}
    //else {
    //    duplicatedBuffers = [frame.port openSourceBuffersWithFilePaths:@[activedSourceBuffer.filePath]];
    //    if (duplicatedBuffers)
    //        activedSourceBuffer = duplicatedBuffers[0];
    //}
    //
    //if (activedSourceBuffer) {
    //    [frame.port setActivedSourceBuffer:activedSourceBuffer];
    //    [self selectFrame:frame];
    //}
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
}

- (void)openSourceBufferResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    
    if (!self.childViewControllers.count) {
        CEESessionFrameViewController* frame = [self createFrame];
        if (!frame)
            return;
        
        [self split:kCEESplitFrameDirectionVertical withFrame:frame];
        [frame setPort:port];
        [frame setIdentifier:[self frame:frame identifierFromPort:port]];
        [frame.view setIdentifier:[self frameView:frame.view identifierFromPort:port]];
        [self selectFrame:frame];
        [frame presentSourceBuffer];
    }
    else {
        for (CEESessionFrameViewController* frame in self.childViewControllers) {
            if (frame.port == port) {
                [frame presentSourceBuffer];
                break;
            }
        }
    }
}

- (void)presentHistoryResponse:(NSNotification*)notification {
    [self openSourceBufferResponse:notification];
}

- (void)activeSourceBufferResponse:(NSNotification*)notification {
    [self openSourceBufferResponse:notification];
}

- (void)deletePortResponse:(NSNotification*)notification {
    if (notification.object != _session)
        return;
    
    CEESessionFrameViewController* target = nil;
    NSInteger i = 0;
    while (i < self.childViewControllers.count) {
        CEESessionFrameViewController* frame = self.childViewControllers[i];
        if (!frame.port.openedSourceBuffers || 
            !frame.port.openedSourceBuffers.count) {
            target = frame;
            break;
        }
        i ++;
    }
    
    if (target) {
        [self collapseView:target.view];
        [[NSNotificationCenter defaultCenter] removeObserver:self.childViewControllers[i]];
        [self removeChildViewControllerAtIndex:i];
        if (self.childViewControllers.count)
            [self selectFrame:self.childViewControllers.lastObject];
    }
}

- (NSString*)frameView:(NSView*)view identifierFromPort:(CEESessionPort*)port {
    NSString* suffix = IdentifierByDeletingPrefix([NSString stringWithString:port.identifier]);
    NSString* prefix = CreateObjectIDPrefix(view);
    NSString* identifier = CreateObjectIDWithPrefixAndSuffix(prefix, suffix);
    return identifier;
}

- (NSString*)frame:(CEESessionFrameViewController*)frame identifierFromPort:(CEESessionPort*)port {
    NSString* suffix = IdentifierByDeletingPrefix([NSString stringWithString:port.identifier]);
    NSString* prefix = CreateObjectIDPrefix(frame);
    NSString* identifier = CreateObjectIDWithPrefixAndSuffix(prefix, suffix);
    return identifier;
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.view.identifier];
    serializing = [serializing stringByAppendingString:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    serializing = [serializing stringByAppendingString:@"\"subviews\":["];
    
    serializing = [serializing stringByAppendingString:@"{"];
    if (self.view.subviews.count) {
        NSView* subview = self.view.subviews[0];
        if ([subview isKindOfClass:[NSSplitView class]])
            [self serializeSplitView:(NSSplitView*)subview withSerializing:&serializing];
        else
            serializing = [serializing stringByAppendingString:[self serializeFrameView:subview]];
    }
    serializing = [serializing stringByAppendingString:@"}"];
    
    serializing = [serializing stringByAppendingFormat:@"]"];
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (void)serializeSplitView:(NSSplitView*)splitView withSerializing:(NSString**)serializing {
    // serialize start
    *serializing = [*serializing stringByAppendingFormat:@"\"%@\":", splitView.identifier];
    *serializing = [*serializing stringByAppendingString:@"{"];
    
    if (splitView.isVertical)
        *serializing = [*serializing stringByAppendingString:@"\"vertical\":\"YES\","];
    else
        *serializing = [*serializing stringByAppendingString:@"\"vertical\":\"NO\","];
    
    *serializing = [*serializing stringByAppendingFormat:@"\"frame\":\"%@\",", NSStringFromRect(splitView.frame)];
    
    *serializing = [*serializing stringByAppendingString:@"\"subviews\":["];
    for (int i = 0; i < splitView.subviews.count; i ++) {
        
        *serializing = [*serializing stringByAppendingString:@"{"];
        
        NSView* view = splitView.subviews[i];
        if ([view isKindOfClass:[NSSplitView class]])
            [self serializeSplitView:(NSSplitView*)view withSerializing:serializing];
        else
            *serializing = [*serializing stringByAppendingString:[self serializeFrameView:view]];
        
        *serializing = [*serializing stringByAppendingString:@"}"];
        if (i < splitView.subviews.count - 1)
            *serializing = [*serializing stringByAppendingString:@","];
    }
    *serializing = [*serializing stringByAppendingString:@"]"];
    
    // serialize end
    *serializing = [*serializing stringByAppendingString:@"}"];
}

- (NSString*)serializeFrameView:(NSView*)view {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", view.identifier];
    serializing = [serializing stringByAppendingString:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"frame\":\"%@\"", NSStringFromRect(view.frame)];
    serializing = [serializing stringByAppendingString:@"}"];
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    NSArray* descriptors = dict[@"subviews"];
    NSSize size = NSSizeFromString(dict[@"size"]);
    NSView* subview = [self deserializeSubview:descriptors[0]];
    if (subview)
        [self addView:subview toView:self.view];
    
    [self.view setFrameSize:size];
    [self connectFramesToPorts];
}

- (void)connectFramesToPorts {
    for (CEESessionFrameViewController* frame in self.childViewControllers) {
        CEESessionPort* port = [self portWithIdentifierSuffix:IdentifierByDeletingPrefix(frame.identifier)];
        CEEPresentSourceState state = kCEEPresentSourceStateSuccess;
        if (port) {
            [frame setPort:port];
            state = [frame presentSourceBuffer];
            
            if (state == kCEEPresentSourceStateNoBuffer)
                [port openUntitledSourceBuffer];
            
            [frame deselect];
            if (port == _session.activedPort) {
                _selectedFrame = frame;
                [_selectedFrame select];
            }
        }
    }
} 

- (CEESessionPort*)portWithIdentifierSuffix:(NSString*)suffix {
    for (CEESessionPort* port in _session.ports) {
        if ([IdentifierByDeletingPrefix(port.identifier) isEqualToString:suffix])
            return port;
    }
    return nil;
}

- (NSView*)deserializeSubview:(NSDictionary*)dict {
    NSString* identifier = [self identifierFromDescriptor:dict];
    if (identifier) {
        NSDictionary* descriptor = dict[identifier];
        if ([self isFrameViewIdentifier:identifier])
            return [self createFrameViewFromDescriptor:descriptor withIdentifier:identifier];
        else if ([self isSplitViewIdentifier:identifier])
            return [self createSplitViewFromDescriptor:descriptor withIdentifier:identifier];
    }
    return nil;
}

- (NSString*)identifierFromDescriptor:(NSDictionary*)descriptor {
    NSArray* keys = [descriptor allKeys];
    if (keys && keys.count)
        return keys[0];
    return nil;
}

- (BOOL)isFrameViewIdentifier:(NSString*)identifier {
    NSString* className = ClassNameFromObjectID(identifier);
    return [className isEqualToString:[CEESessionFrameView className]];
}

- (BOOL)isSplitViewIdentifier:(NSString*)identifier {
    NSString* className = ClassNameFromObjectID(identifier);
    return [className isEqualToString:[CEESessionFrameSplitView className]];
}

- (NSView*)createFrameViewFromDescriptor:(NSDictionary*)dict withIdentifier:(NSString*)identifier {
    CEESessionFrameViewController* frame = [self createFrame];
    NSString* suffix = IdentifierByDeletingPrefix(identifier);
    NSString* prefix = nil;
    
    prefix = CreateObjectIDPrefix(frame);
    [frame setIdentifier:CreateObjectIDWithPrefixAndSuffix(prefix, suffix)];
    
    prefix = CreateObjectIDPrefix(frame.view);
    [frame.view setIdentifier:CreateObjectIDWithPrefixAndSuffix(prefix, suffix)];
    
    [self addChildViewController:frame];
    return frame.view;
}

- (NSView*)createSplitViewFromDescriptor:(NSDictionary*)dict withIdentifier:(NSString*)identifier {
    BOOL vertical = NO;
    if ([dict[@"vertical"] caseInsensitiveCompare:@"YES"] == NSOrderedSame)
        vertical = YES;
    else
        vertical = NO;
    
    NSRect frame = NSRectFromString(dict[@"frame"]);
    NSView* splitView = [self createSplitViewWithFrame:frame vertical:vertical splitViewDelegate:self];
    [_splitViews addObject:splitView];
    [splitView setFrame:frame];
    
    NSArray* descriptors = dict[@"subviews"];
    for (NSDictionary* descriptor in descriptors) {
        NSView* subview = nil;
        NSString* identifier = [self identifierFromDescriptor:descriptor];
        if (identifier) {
            if ([self isFrameViewIdentifier:identifier])
                subview = [self createFrameViewFromDescriptor:descriptor[identifier] withIdentifier:identifier];
            else if ([self isSplitViewIdentifier:identifier])
                subview = [self createSplitViewFromDescriptor:descriptor[identifier] withIdentifier:identifier];
            [splitView addSubview:subview];
        }
    }
    return splitView;
}

#pragma mark - CEESessionFrameManagerViewDelegate protocol

- (BOOL)frameViewAttachable:(CEESessionFrameView*)frameView {
    if (self.childViewControllers.count || !_frameAttachable)
        return NO;
    return YES;
}

- (void)createInitFrameViewWithFilePaths:(NSArray*)filePaths {    
    CEESessionFrameViewController* frame = [self createFrame];
    if (!frame)
        return;
    
    [self split:kCEESplitFrameDirectionVertical withFrame:frame];
    
    if (!_session.activedPort)
        [_session setActivedPort:[_session createPort]];
    
    [self.session.project addSecurityBookmarksWithFilePaths:filePaths];
    
    [frame setPort:_session.activedPort];
    [frame setIdentifier:[self frame:frame identifierFromPort:_session.activedPort]];
    [frame.view setIdentifier:[self frameView:frame.view identifierFromPort:_session.activedPort]];
    [frame.port openSourceBuffersWithFilePaths:filePaths];
    [self selectFrame:frame];
}

@end
