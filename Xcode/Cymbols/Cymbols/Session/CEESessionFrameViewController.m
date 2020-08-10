//
//  CEESessionFrameViewController.m
//  Cymbols
//
//  Created by Qing on 2018/8/16.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//
#import "CymbolsDelegate.h"
#import "CEESourceViewController.h"
#import "CEESessionFrameViewController.h"
#import "CEESessionFrameManager.h"
#import "CEETabView.h"
#import "CEETabbarButton.h"
#import "CEESourceBuffer.h"
#import "CEESourceHistoryController.h"
#import "CEEPopupPanel.h"
#import "CEEIdentifier.h"

@interface CEESessionFrameViewController()
@property (strong) IBOutlet CEETabView *titlebar;
@property (strong) IBOutlet CEETabbarButton *closeButton;
@property (strong) IBOutlet CEETabbarButton *previousButton;
@property (strong) IBOutlet CEETabbarButton *nextButton;
@property (weak) IBOutlet CEETabbarButton *bufferListButton;
@property (strong) CEESourceViewController* sourceViewController;
@property (strong) NSPanel* titleDetail;
@property (strong) CEEPopupPanel* sourceHistoryPanel;
@end

@implementation CEESessionFrameViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
        
    //_titleDetail = [NSPanel windowWithContentViewController:[[NSStoryboard storyboardWithName:@"Session" bundle:nil] instantiateControllerWithIdentifier:@"IDAutoCompleteController"]];
    //[_titleDetail setFloatingPanel:YES];
    //[_titleDetail setBecomesKeyOnlyIfNeeded:YES];
    //[_titleDetail setStyleMask:NSWindowStyleMaskBorderless];
    //[_titleDetail setHasShadow:NO];
    //[_titleDetail setBackgroundColor:[NSColor clearColor]];
    
    _sourceHistoryPanel = [CEEPopupPanel windowWithContentViewController:[[NSStoryboard storyboardWithName:@"SourceHistory" bundle:nil] instantiateControllerWithIdentifier:@"IDSourceHistoryController"]];
    [_sourceHistoryPanel setStyleMask:NSWindowStyleMaskBorderless];
    [_sourceHistoryPanel setHasShadow:YES];
    [_sourceHistoryPanel setBackgroundColor:[NSColor clearColor]];
    [_sourceHistoryPanel setDelegate:self];
    [_sourceHistoryPanel setFloatingPanel:YES];
    [_sourceHistoryPanel registerEventMonitor];
    
    [_titlebar setDelegate:self];
    [_titlebar setHost:self.view];
    CGFloat leadingOffset = _previousButton.frame.size.width + 
        _nextButton.frame.size.width + _bufferListButton.frame.size.width + 12.0;
    CGFloat tailingOffset = self.view.frame.size.width - _closeButton.frame.origin.x;
    [_titlebar setLeadingOffset:leadingOffset];
    [_titlebar setTailingOffset:tailingOffset];
    
    [(CEESessionFrameView*)self.view setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textBufferStateChangedResponse:) name:CEENotificationSourceBufferStateChanged object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deletePortResponse:) name:CEENotificationSessionDeletePort object:nil];
}

- (BOOL)becomeFirstResponder {
    [self.view.window makeFirstResponder:_sourceViewController];
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
        [_port prevReference];
}

- (IBAction)next:(id)sender {
    if (_port)
        [_port nextReference];
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

- (IBAction)showSourceHistory:(id)sender {
    NSPoint point0 = [self.titlebar convertPoint:_bufferListButton.frame.origin toView:nil];    
    NSPoint point1 = [self.view convertPoint:_titlebar.frame.origin toView:nil];
    NSRect rect = [self.view.window convertRectToScreen:NSMakeRect(point0.x, point1.y, 300, 0)];
    [(CEEView*)_sourceHistoryPanel.contentView setStyle:kCEEViewStyleActived];
    [(CEESourceHistoryController*)_sourceHistoryPanel.contentViewController setPort:self.port];
    [_sourceHistoryPanel setFrame:rect display:NO];
    [_sourceHistoryPanel orderFront:self];
}


- (void)present {
    CEESourceViewController* viewController = nil;
    CEESourceBuffer* buffer = [[_port currentReference] buffer];
    NSInteger paragraphIndex = [[_port currentReference] paragraphIndex];
    
    if (!buffer)
        return;
    
    if (buffer.type == kCEEBufferTypeUTF8)
        viewController = [[NSStoryboard storyboardWithName:@"SourceViewController" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    else if (buffer.type == kCEEBufferTypeBinary) {
        viewController = [[NSStoryboard storyboardWithName:@"SourceViewController" bundle:nil] instantiateControllerWithIdentifier:@"IDBinaryEditViewController"];
    }
    
    if (!viewController)
        return;
    
    if (_sourceViewController) {
        [_sourceViewController.view removeFromSuperview];
        [_sourceViewController removeFromParentViewController];
    }
    
    _sourceViewController = viewController;
    [_sourceViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [self.view addSubview:_sourceViewController.view];
    [self addChildViewController:_sourceViewController];
    [self.view setStyle:kCEEViewStyleActived | kCEEViewStyleSelected];
    
    NSDictionary *views = @{
                            @"titlebar" : _titlebar,
                            @"sourceView" : _sourceViewController.view,
                            };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[sourceView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[titlebar]-0-[sourceView]-0-|" options:0 metrics:nil views:views];
    [self.view addConstraints:constraintsH];
    [self.view addConstraints:constraintsV];
    
    [viewController setPort:_port];
    [viewController setBuffer:buffer];
    [viewController setParagraphIndex:paragraphIndex];
    
    self.title = buffer.filePath;
    [_titlebar setTitle:self.title];
}

- (void)select {
    [self.view.window makeFirstResponder:_sourceViewController];
    [self.view setStyle:kCEEViewStyleSelected | kCEEViewStyleActived];
}

- (void)deselect {
    [self.view clearStyle:kCEEViewStyleSelected];
}

- (void)setTitle:(NSString *)title {
    [super setTitle:title];
    [_titlebar setTitle:self.title];
}

- (void)textBufferStateChangedResponse:(NSNotification*)notification {
    CEESourceBuffer* buffer = [[_port currentReference] buffer];
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
