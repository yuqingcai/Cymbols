//
//  CEESessionFrameViewController.m
//  Cymbols
//
//  Created by Qing on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEStyleManager.h"
#import "CEEEditViewController.h"
#import "CEESessionFrameViewController.h"
#import "CEESessionFrameManager.h"
#import "CEESourceBuffer.h"
#import "CEETitlebarButton.h"
#import "CEETitlebarStateButton.h"
#import "CEESourceHistoryViewController.h"
#import "CEESourceHistoryPopupPanel.h"
#import "CEEIdentifier.h"

@interface CEESessionFrameViewController()
@property (weak) IBOutlet CEETitleView *titlebar;
@property (weak) IBOutlet CEETitlebarButton *closeButton;
@property (weak) IBOutlet CEETitlebarStateButton *pinButton;
@property (weak) IBOutlet CEETitlebarButton *previousButton;
@property (weak) IBOutlet CEETitlebarButton *nextButton;
@property (weak) IBOutlet CEETitlebarButton *bufferListButton;
@property (strong) CEEEditViewController* editViewController;
//@property (strong) NSPanel* titleDetail;
@property (strong) CEESourceHistoryPopupPanel* sourceHistoryPanel;
@property (strong) CEESourceBuffer* sourceBuffer;
@end

@implementation CEESessionFrameViewController

@synthesize port = _port;

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    _sourceHistoryPanel = [CEESourceHistoryPopupPanel windowWithContentViewController:[[NSStoryboard storyboardWithName:@"Session" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceHistoryViewController"]];
    [_sourceHistoryPanel setDelegate:self];
    
    [_titlebar setDelegate:self];
    [_titlebar setDraggingSource:(CEESessionFrameView*)self.view];
    [_titlebar setTitleLeading:_previousButton.frame.origin.x + _previousButton.frame.size.width +
     _nextButton.frame.size.width + _bufferListButton.frame.size.width + 18.0];
    [_titlebar setTitleTailing:_closeButton.frame.size.width + _pinButton.frame.size.width + 18.0];
    
    [(CEESessionFrameView*)self.view setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(saveSourceBufferResponse:) name:CEENotificationSessionPortSaveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferReloadResponse:) name:CEENotificationSourceBufferReload object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(jumpToSourcePointResponse:) name:CEENotificationSessionJumpToSourcePoint object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateContentViewStyle:) name:CEENotificationUserInterfaceStyleUpdate object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(pinPortResponse:) name:CEENotificationSessionPinPort object:nil];
}

- (void)updateContentViewStyle:(NSNotification*)notification {
   CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
   [_sourceHistoryPanel.contentViewController setViewStyleConfiguration:[styleManager userInterfaceConfiguration]];
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

- (void)setPort:(CEESessionPort *)port {
    _port = port;
    if (_sourceHistoryPanel)
        [_sourceHistoryPanel setPort:port];
}

- (IBAction)pin:(id)sender {
    if (_pinButton.state == NSControlStateValueOn)
        [_port.session setPinnedPort:_port];
    else
        [_port.session setPinnedPort:nil];
}

- (CEESessionPort*)port {
    return _port;
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

- (IBAction)presentHistoryView:(id)sender {
    NSPoint point0 = [self.titlebar convertPoint:_bufferListButton.frame.origin toView:nil];
    NSPoint point1 = [self.view convertPoint:_titlebar.frame.origin toView:nil];
    point0 = [self.view.window convertPointToScreen:point0];
    point1 = [self.view.window convertPointToScreen:point1];
    
    CGFloat width = [_sourceHistoryPanel expactedWidth];
    CGFloat height = [_sourceHistoryPanel expactedHeight];
    
    if (height > point1.y) {
        if (point1.y < [_sourceHistoryPanel minimumHeight])
            height = [_sourceHistoryPanel minimumHeight];
        else
            height = point1.y;
    }
    NSRect rect = NSMakeRect(point0.x, point1.y, width , height);
    rect.origin.y -= height;
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
    
    CEESourceBufferReferenceContext* reference = [self.port currentSourceBufferReference];
    if (reference) {
        NSInteger focusBufferOffset = [reference focusBufferOffset];
        [viewController setFocusBufferOffset:focusBufferOffset];
        
        NSInteger presentBufferOffset = [reference presentBufferOffset];
        [viewController setPresentBufferOffset:presentBufferOffset];
    }
    
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
    
    CEESourcePoint* jumpPoint = _port.session.jumpPoint;
    if (!jumpPoint)
        return;
    
    [_port openSourceBufferWithFilePath:jumpPoint.filePath];
    
    CEEList* ranges = cee_ranges_from_string([jumpPoint.locations UTF8String]);
    if (ranges) {
        [_editViewController highlight:ranges];
        [_editViewController setIntelligentPickup:NO];
        cee_list_free_full(ranges, cee_range_free);
    }
    
    CEESourceBufferReferenceContext* reference = [self.port currentSourceBufferReference];
    if (reference) {
        [reference setPresentBufferOffset:[_editViewController presentBufferOffset]];
        [reference setFocusBufferOffset:[_editViewController focusBufferOffset]];
    }
}

- (void)pinPortResponse:(NSNotification*)notification {
    if (notification.object != _port.session)
        return;
    
    if (_port.session.pinnedPort != _port) {
        if (_pinButton.state == NSControlStateValueOn)
            _pinButton.state = NSControlStateValueOff;
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

- (BOOL)deserialize:(NSDictionary*)dict {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
    [super mouseDown:event];
    if (!_manager)
        return;
    
    [_manager selectFrame:self];
}
@end
