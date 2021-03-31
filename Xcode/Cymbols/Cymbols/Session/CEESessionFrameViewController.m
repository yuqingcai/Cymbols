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

@interface CEESessionFrameViewController()
@property (weak) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETitlebarButton *closeButton;
@property (weak) IBOutlet CEETitlebarButton *previousButton;
@property (weak) IBOutlet CEETitlebarButton *nextButton;
@property (weak) IBOutlet CEETitlebarButton *bufferListButton;
@property (strong) CEEEditViewController* editViewController;
@property (strong) NSPanel* titleDetail;
@property (strong) CEEPopupPanel* sourceHistoryPanel;
@property (strong) CEESourceBuffer* sourceBuffer;
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
    [_titlebar setTitleLeading:_previousButton.frame.origin.x + _previousButton.frame.size.width +
     _nextButton.frame.size.width + _bufferListButton.frame.size.width + 18.0];
    [_titlebar setTitleTailing:_closeButton.frame.size.width];
    
    [(CEESessionFrameView*)self.view setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveSourceBufferResponse:) name:CEENotificationSessionPortSaveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferReloadResponse:) name:CEENotificationSourceBufferReload object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(jumpToSourcePointResponse:) name:CEENotificationSessionPortJumpToSourcePoint object:nil];
    
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
        [_manager closeFrame:self];
}

- (IBAction)prev:(id)sender {
    if (_port)
        [_port moveSourceBufferReferencePrev];
}

- (IBAction)next:(id)sender {
    if (_port)
        [_port moveSourceBufferReferenceNext];
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

- (IBAction)presentHistoryList:(id)sender {
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

- (CEEPresentSourceState)presentSourceBuffer {
    _sourceBuffer = [_port activedSourceBuffer];
    if (!_sourceBuffer)
        return kCEEPresentSourceStateNoBuffer;
    
    CEEEditViewController* viewController = nil;
    
    if (_sourceBuffer.type == kCEESourceBufferTypeText)
        viewController = [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    else if (_sourceBuffer.type == kCEESourceBufferTypeBinary)
        viewController = [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDBinaryEditViewController"];
    else
        viewController = [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDNotSupportedEditViewController"];
    
    if ([_editViewController.view superview])
        [_editViewController.view removeFromSuperview];
    
    // The old _editViewController should release immediately
    // after a new _editViewController is created.
    _editViewController = viewController;
    [_editViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [self.view addSubview:_editViewController.view];
    
    NSDictionary *views = @{
        @"titlebar" : _titlebar,
        @"editView" : _editViewController.view,
    };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[editView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[editView]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [viewController setPort:_port];
    [viewController setBuffer:_sourceBuffer];
    
    NSInteger lineBufferOffset = [[_port currentSourceBufferReference] lineBufferOffset];
    NSInteger caretBufferOffset = [[_port currentSourceBufferReference] caretBufferOffset];
    [viewController setCaretBufferOffset:caretBufferOffset];
    [viewController setLineBufferOffset:lineBufferOffset];
    
    if ([_sourceBuffer stateSet:kCEESourceBufferStateFileTemporary])
        self.title = [_sourceBuffer.filePath lastPathComponent];
    else
        self.title = _sourceBuffer.filePath;
    
    return kCEEPresentSourceStateSuccess;
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

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {
    CEESourceBuffer* buffer = notification.object;
    
    if (buffer != _sourceBuffer)
        return;
    
    if ([_sourceBuffer stateSet:kCEESourceBufferStateFileTemporary])
        self.title = [_sourceBuffer.filePath lastPathComponent];
    else
        self.title = _sourceBuffer.filePath;
    
    if ([_sourceBuffer stateSet:kCEESourceBufferStateShouldSyncToFile])
        self.title = [self.title stringByAppendingFormat:@" *"];
    
    if ([_sourceBuffer stateSet:kCEESourceBufferStateFileDeleted])
        self.title = [self.title stringByAppendingFormat:@" (delete)"];
}

- (void)saveSourceBufferResponse:(NSNotification*)notification {
    if (notification.object != _port)
        return;
        
    if ([_sourceBuffer stateSet:kCEESourceBufferStateFileTemporary])
        self.title = [_sourceBuffer.filePath lastPathComponent];
    else
        self.title = _sourceBuffer.filePath;
    
    if ([_sourceBuffer stateSet:kCEESourceBufferStateShouldSyncToFile])
        self.title = [self.title stringByAppendingFormat:@" *"];
    
    if ([_sourceBuffer stateSet:kCEESourceBufferStateFileDeleted])
        self.title = [self.title stringByAppendingFormat:@" (delete)"];
}

- (void)sourceBufferReloadResponse:(NSNotification*)notification {
    if (notification.object != _port.activedSourceBuffer)
        return;
    [self presentSourceBuffer];
}

- (void)jumpToSourcePointResponse:(NSNotification*)notification {
    if (notification.object != _port)
        return;
    CEESourcePoint* jumpPoint = _port.jumpPoint;
    
    if(![jumpPoint.filePath isEqualToString:_port.activedSourceBuffer.filePath])
            [_port openSourceBufferWithFilePath:jumpPoint.filePath];
    CEEList* ranges = cee_ranges_from_string([jumpPoint.locations UTF8String]);
    if (ranges) {
        [_editViewController highlightRanges: ranges];
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
