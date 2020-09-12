//
//  CEESessionFrameViewController.m
//  Cymbols
//
//  Created by Qing on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEEditViewController.h"
#import "CEESessionFrameViewController.h"
#import "CEESessionFrameManager.h"
#import "CEESourceBuffer.h"
#import "CEETitlebarButton.h"
#import "CEESourceHistoryViewController.h"
#import "CEEPopupPanel.h"
#import "CEEIdentifier.h"
#import "CEEProjectContextViewController.h"

@interface CEESessionFrameViewController()
@property (weak) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETitlebarButton *closeButton;
@property (weak) IBOutlet CEETitlebarButton *previousButton;
@property (weak) IBOutlet CEETitlebarButton *nextButton;
@property (weak) IBOutlet CEETitlebarButton *bufferListButton;
@property (strong) CEEEditViewController* editViewController;
@property (strong) NSPanel* titleDetail;
@property (strong) CEEPopupPanel* sourceHistoryPanel;
@end

@implementation CEESessionFrameViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    _sourceHistoryPanel = [CEEPopupPanel windowWithContentViewController:[[NSStoryboard storyboardWithName:@"SourceHistory" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceHistoryViewController"]];
    [_sourceHistoryPanel setStyleMask:NSWindowStyleMaskBorderless];
    [_sourceHistoryPanel setHasShadow:YES];
    [_sourceHistoryPanel setBackgroundColor:[NSColor clearColor]];
    [_sourceHistoryPanel setDelegate:self];
    [_sourceHistoryPanel setFloatingPanel:YES];
    [_sourceHistoryPanel setExclusived:YES];
    
    [_titlebar setDelegate:self];
    [_titlebar setDraggingSource:(CEESessionFrameView*)self.view];
    CGFloat leadingOffset = _previousButton.frame.size.width + 
        _nextButton.frame.size.width + _bufferListButton.frame.size.width + 12.0;
    CGFloat tailingOffset = self.view.frame.size.width - _closeButton.frame.origin.x;
    [_titlebar setLeadingOffset:leadingOffset];
    [_titlebar setTailingOffset:tailingOffset];
    
    [(CEESessionFrameView*)self.view setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textBufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deletePortResponse:) name:CEENotificationSessionDeletePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(requestTargetSymbolSelectionResponse:) name:CEENotificationSessionPortRequestTargetSymbolSelection object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setTargetSymbolResponse:) name:CEENotificationSessionPortSetTargetSymbol object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setActivedSymbolResponse:) name:CEENotificationSessionPortSetActivedSymbol object:nil];
}

- (BOOL)becomeFirstResponder {
    [self.view.window makeFirstResponder:_editViewController];
    return YES;
}

- (void)dealloc {
    [_sourceHistoryPanel close];
    _sourceHistoryPanel = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSImage*)titleViewIcon:(CEETitleView*)titleView {
    return nil;
}

- (IBAction)close:(id)sender {
    if (_manager)
        [_manager closeFrame:self]; //bug here...
}

- (IBAction)prev:(id)sender {
    if (_port)
        [_port prevBufferReference];
}

- (IBAction)next:(id)sender {
    if (_port)
        [_port nextBufferReference];
}

- (void)expandTitleViewDetail:(CEETitleView *)titleView {
    //NSPoint pointInWindow = [self.view convertPoint:_titlebar.frame.origin toView:nil];
    //NSRect rect = [self.view.window convertRectToScreen:NSMakeRect(pointInWindow.x, pointInWindow.y-30, 100, 30)];
    //[(CEEView*)_titleDetail.contentView setStyle:kCEEViewStyleActived];
    //[_titleDetail setFrame:rect display:NO];
    //[_titleDetail orderFront:self];
}

- (void)collapseTitleViewDetail:(CEETitleView *)titleView {
    //[(CEEView*)_titleDetail.contentView clearStyle:kCEEViewStyleActived];
    //[_titleDetail orderOut:self];
}

- (IBAction)presentHistory:(id)sender {
    NSPoint point0 = [self.titlebar convertPoint:_bufferListButton.frame.origin toView:nil];    
    NSPoint point1 = [self.view convertPoint:_titlebar.frame.origin toView:nil];
    point1 = [self.view.window convertPointToScreen:point1];
    NSRect rect = NSMakeRect(point1.x, point1.y, 400 , 400);
    rect.origin.y -= 400;    
    [(CEEView*)_sourceHistoryPanel.contentView setStyleState:kCEEViewStyleStateActived];
    [(CEESourceHistoryViewController*)_sourceHistoryPanel.contentViewController setPort:self.port];
    [_sourceHistoryPanel setFrame:rect display:YES];
    [_sourceHistoryPanel orderFront:nil];
}

- (void)presentSource {
    CEEEditViewController* viewController = nil;
    CEESourceBuffer* buffer = [[_port currentBufferReference] buffer];
    NSInteger paragraphIndex = [[_port currentBufferReference] paragraphIndex];
    
    if (!buffer)
        return;
    
    if (buffer.type == kCEEBufferTypeUTF8)
        viewController = [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    else if (buffer.type == kCEEBufferTypeBinary) {
        viewController = [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDBinaryEditViewController"];
    }
    
    if (!viewController)
        return;
    
    if (_editViewController) {
        [_editViewController.view removeFromSuperview];
        [_editViewController removeFromParentViewController];
    }
    
    _editViewController = viewController;
    [_editViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
        
    [self.view addSubview:_editViewController.view];
    [self addChildViewController:_editViewController];
    
    NSDictionary *views = @{
                            @"titlebar" : _titlebar,
                            @"editView" : _editViewController.view,
                            };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[editView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[editView]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [viewController setEditable:YES];
    [viewController setIntelligence:YES];
    [viewController setPort:_port];
    [viewController setBuffer:buffer];
    [viewController setParagraphIndex:paragraphIndex];
    
    self.title = buffer.filePath;
    [_titlebar setTitle:self.title];
}

- (void)select {
    [self.view.window makeFirstResponder:_editViewController];
    [_titlebar setStyleState:kCEEViewStyleStateActived];
    [_editViewController setViewStyleState:kCEEViewStyleStateActived];
}

- (void)deselect {
    [_titlebar setStyleState:kCEEViewStyleStateDeactived];
    [_editViewController setViewStyleState:kCEEViewStyleStateDeactived];
}

- (void)setTitle:(NSString *)title {
    [super setTitle:title];
    [_titlebar setTitle:self.title];
}

- (void)textBufferStateChangedResponse:(NSNotification*)notification {
    CEESourceBuffer* buffer = [[_port currentBufferReference] buffer];
    if (notification.object != buffer)
        return;
    
    self.title = buffer.filePath;
    
    if ([buffer stateSet:kCEESourceBufferStateModified])
        self.title = [self.title stringByAppendingFormat:@" *"];
    
    if ([buffer stateSet:kCEESourceBufferStateFileDeleted])
        self.title = [self.title stringByAppendingFormat:@" (delete)"];
}

- (void)deletePortResponse:(NSNotification*)notification {
    if (notification.object != _port)
        return;
    
    [_manager closeFrame:self];
}

- (void)requestTargetSymbolSelectionResponse:(NSNotification*)notification {
    if (notification.object != _port)
        return;
    
    NSWindowController* contextWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectContextWindowController"];
    NSModalResponse responese = NSModalResponseCancel;
    responese = [NSApp runModalForWindow:contextWindowController.window];
    if (responese == NSModalResponseOK) {
        CEEProjectContextViewController* contextViewController = (CEEProjectContextViewController*)contextWindowController.contentViewController;
        CEESourceSymbol* selection = contextViewController.selectedSymbol;
        [_port setTargetSourceSymbol:selection];
    }
    [contextWindowController close];
}

- (void)setTargetSymbolResponse:(NSNotification*)notification {
    if (notification.object != _port || !_port.target_symbol)
        return;
    
    NSString* filePath = [NSString stringWithUTF8String:_port.target_symbol->filepath];
    CEEList* ranges = cee_ranges_from_string(_port.target_symbol->locations);
    if (ranges) {
        [_port openSourceBufferWithFilePath:filePath];
        [self.editViewController highlightRanges:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
}

- (void)setActivedSymbolResponse:(NSNotification*)notification {
    if (notification.object != _port || !_port.actived_symbol)
        return;
    
    CEEList* ranges = cee_ranges_from_string(_port.actived_symbol->locations);
    if (ranges) {
        [self.editViewController highlightRanges:ranges];
        cee_list_free_full(ranges, cee_range_free);
    }
}

- (void)windowDidResignKey:(NSNotification *)notification {
    if (notification.object == _sourceHistoryPanel)
        [_sourceHistoryPanel close];
}

#pragma mark - CEESessionFrameViewDelegate protocol

- (void)deriveFrameViewWithFilePaths:(NSArray*)filePaths attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameView*)frameView {
    if (!_manager)
        return;
    
    [_manager deriveFrameWithFilePaths:filePaths attachAt:region relateTo:self];
}

- (void)moveFrameView:(CEESessionFrameView*)frameView0 attachAt:(CEEViewRegion)region relateTo:(CEESessionFrameView*)frameView1 {
    if (!_manager)
        return;
    
    [_manager moveFrameView:frameView0 attachAt:region relateTo:frameView1];
}

- (BOOL)frameViewAttachable:(CEESessionFrameView*)frameView {
    return _manager.frameAttachable;
}

- (CEEView*)titleViewInFrameView:(CEESessionFrameView*)frameView {
    return _titlebar;
}

- (NSString*)titleOfFrameView:(CEESessionFrameView*)frameView {
    return self.title;
}

#pragma mark - serialize
- (NSString*)serialize {
    return @"";
}

- (void)deserialize:(NSDictionary*)dict {
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
    if (!_manager)
        return;
    
    [_manager selectFrame:self];
}


@end
