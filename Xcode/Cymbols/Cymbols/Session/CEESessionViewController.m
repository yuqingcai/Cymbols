//
//  CEESessionViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionViewController.h"
#import "CEESessionSpliter0.h"
#import "CEESessionToolbar.h"
#import "CEEProjectCreatorController.h"
#import "CEEProjectAddFileController.h"
#import "CEEProjectRemoveFileController.h"
#import "AppDelegate.h"
#import "CEESessionView.h"
#import "CEESessionSourceController.h"
#import "CEESessionStatusBar.h"
#import "CEEProjectParseViewController.h"
#import "CEEProjectContextViewController.h"
#import "CEEProjectCleanViewController.h"
#import "CEEProjectSearchViewController.h"
#import "CEETitlebarButton.h"

@interface CEESessionViewController ()

@property (strong) IBOutlet CEESessionView *sessionView;
@property (strong) CEESessionToolbar* toolbar;
@property (weak) IBOutlet CEESessionStatusBar *statusBar;
@property (weak) IBOutlet CEEView *containerView;
@property (weak) CEESessionSpliter0* spliter;
@property CGFloat titleHeight;
@property (strong) NSWindowController* projectParseWindowController;
@property (strong) NSWindowController* projectCleanWindowController;
@property (strong) NSWindowController* projectSearchWindowController;
@property (strong) NSWindowController* contextWindowController;
@property (strong) NSWindowController* userInterfaceStyleSelectionWindowController;
@property (strong) NSWindowController* textHighlightStyleSelectionWindowController;
@property (weak) IBOutlet CEETitlebarButton *messageButton;

@end

@implementation CEESessionViewController

@synthesize sheetOffset = _sheetOffset;
@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_containerView setTranslatesAutoresizingMaskIntoConstraints:NO];
    _titleHeight = 26;
    [_messageButton setEnabled:NO];
    [_messageButton setHidden:YES];
    
    _toolbar = [[NSStoryboard storyboardWithName:@"Session" bundle:nil] instantiateControllerWithIdentifier:@"IDSessionToolbar"];
    [self addChildViewController:_toolbar];
    [self showToolbar:YES];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activeSourceBufferResponse:) name:CEENotificationSessionPortActiveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activePortResponse:) name:CEENotificationSessionActivePort object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setPortDescriptorResponse:) name:CEENotificationSessionPortSetDescriptor object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferReloadResponse:) name:CEENotificationSourceBufferReload object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferChangeStateResponse:) name:CEENotificationSourceBufferChangeState object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sourceBufferCloseResponse:) name:CEENotificationSessionPortCloseSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(requestJumpSourcePointSelectionResponse:) name:CEENotificationSessionPortRequestJumpSourcePointSelection object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(searchReferenceResponse:) name:CEENotificationSessionPortSearchReference object:nil];
}

- (CGFloat)sheetOffset {
    if (_toolbar.shown)
        return _titleHeight + _toolbar.view.frame.size.height + 1;
    else
        return _titleHeight + 1;
}

- (void)showToolbar:(BOOL)shown {
    [_toolbar setShown:shown];
    
    [_containerView removeFromSuperview];
    [_toolbar.view removeFromSuperview];
    [_statusBar removeFromSuperview];
    
    NSDictionary* views = @{
        @"toolbarView" : _toolbar.view,
        @"containerView" : _containerView,
        @"statusBar": _statusBar,
    };
    
    NSDictionary* metrics = @{
        @"titleHeight" : @(_titleHeight),
        @"toolbarHeight" : @(_toolbar.view.frame.size.height),
        @"interval0" : @(_sessionView.seperateLineSize),
        @"interval1" : @(_sessionView.seperateLineSize + _titleHeight)
    };
    
    NSArray* constraints;
    if (shown) {
        [self.view addSubview:_toolbar.view];
        [self.view addSubview:_containerView];
        [self.view addSubview:_statusBar];
        
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"H:|-0-[toolbarView]-0-|" options:0 metrics:nil views:views];
        [self.view addConstraints:constraints];
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"H:|-0-[containerView]-0-|" options:0 metrics:nil views:views];
        [self.view addConstraints:constraints];
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"H:|-0-[statusBar]-0-|" options:0 metrics:nil views:views];
        [self.view addConstraints:constraints];
        
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"V:|-titleHeight-[toolbarView(==toolbarHeight)]-interval0-[containerView]-0-[statusBar]-0-|" options:0 metrics:metrics views:views];
        [self.view addConstraints:constraints];
        _sessionView.seperateLineTopSpace = _titleHeight + _toolbar.view.frame.size.height;
    }
    else {
        [self.view addSubview:_containerView];
        [self.view addSubview:_statusBar];
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"H:|-0-[containerView]-0-|" options:0 metrics:nil views:views];
        [self.view addConstraints:constraints];
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"H:|-0-[statusBar]-0-|" options:0 metrics:nil views:views];
        [self.view addConstraints:constraints];
        
        constraints = [NSLayoutConstraint constraintsWithVisualFormat: @"V:|-interval1-[containerView]-0-[statusBar]-0-|" options:0 metrics:metrics views:views];
        [self.view addConstraints:constraints];
        _sessionView.seperateLineTopSpace = _titleHeight;
    }
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {
    if ([[segue identifier] caseInsensitiveCompare:@"IDSessionViewEmbedSegue"] == NSOrderedSame)
        _spliter = [segue destinationController];
    
    if ([[segue identifier] caseInsensitiveCompare:@"IDProjectCreatorSegue"] == NSOrderedSame) {
        CEEProjectCreatorController* projectCreatorController = (CEEProjectCreatorController*)[segue destinationController];
        [projectCreatorController setSession:self.session];
    }
    
    if ([[segue identifier] caseInsensitiveCompare:@"IDProjectAddFileSegue"] == NSOrderedSame) {
        CEEProjectAddFileController* projectAddFileController = (CEEProjectAddFileController*)[segue destinationController];
        [projectAddFileController setSession:self.session];
    }
    if ([[segue identifier] caseInsensitiveCompare:@"IDProjectRemoveFileSegue"] == NSOrderedSame) {
        CEEProjectRemoveFileController* projectRemoveFileController = (CEEProjectRemoveFileController*)[segue destinationController];
        [projectRemoveFileController setSession:self.session];
    }
}

- (IBAction)toggleToolbar:(id)sender {
    if (_toolbar.shown)
        [self showToolbar:NO];
    else
        [self showToolbar:YES];
}

- (IBAction)toggleSymbolView:(id)sender {
    [_spliter toggleSymbolView];
}

- (IBAction)toggleReferenceView:(id)sender {
    [_spliter toggleReferenceView];
}

- (IBAction)togglePathView:(id)sender {
    [_spliter togglePathView];
}

- (IBAction)toggleContextView:(id)sender {
    CEESessionSourceController* controller = (CEESessionSourceController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceController"];
    [controller toggleContextView];
}

- (IBAction)splitHorizontally:(id)sender {
    CEESessionSourceController* controller = (CEESessionSourceController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceController"];
    [controller splitViewHorizontally];
}

- (IBAction)splitVertically:(id)sender {
    CEESessionSourceController* controller = (CEESessionSourceController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceController"];
    [controller splitViewVertically];
}

- (IBAction)toggleUserInterfaceStyleSelection:(id)sender {
    if (!_userInterfaceStyleSelectionWindowController)
        _userInterfaceStyleSelectionWindowController = [[NSStoryboard storyboardWithName:@"Theme" bundle:nil] instantiateControllerWithIdentifier:@"IDUserInterfaceStyleSelectionWindowController"];
    NSModalResponse responese = [NSApp runModalForWindow:_userInterfaceStyleSelectionWindowController.window];
    [_userInterfaceStyleSelectionWindowController close];
}

- (IBAction)toggleTextHighlightStyleSelection:(id)sender {
    if (!_textHighlightStyleSelectionWindowController)
        _textHighlightStyleSelectionWindowController = [[NSStoryboard storyboardWithName:@"Theme" bundle:nil] instantiateControllerWithIdentifier:@"IDTextHighlightStyleSelectionWindowController"];
    NSModalResponse responese = [NSApp runModalForWindow:_textHighlightStyleSelectionWindowController.window];
    [_textHighlightStyleSelectionWindowController close];
}

- (IBAction)newWindow:(id)sender {
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    [projectController createProjectFromSetting:nil];
    CEEWindowController* windowController = projectController.currentDocument.windowControllers[0];
    NSRect frame = windowController.window.frame;
    NSPoint origin = self.view.window.frame.origin;
    frame.origin = origin;
    frame.origin.x += 60.0;
    frame.origin.y -= 60.0;
    [windowController.window setFrame:frame display:YES];
}

- (IBAction)listWindows:(id)sender {
    [_session.project showWindows];
}

- (IBAction)newFile:(id)sender {
    if (!_session.activedPort)
        [_session setActivedPort:[_session createPort]];
    [_session.activedPort openUntitledSourceBuffer];
}

- (IBAction)newProject:(id)sender {
    [self performSegueWithIdentifier:@"IDProjectCreatorSegue" sender:self];
}

- (NSArray*)filePathsFromOpenPanel {
    __block NSMutableArray* filePaths = [[NSMutableArray alloc] init];
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    NSURL* URL = [[NSURL alloc] initFileURLWithPath:NSHomeDirectory() isDirectory:YES];
    [openPanel setDirectoryURL:URL];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setAllowsMultipleSelection:YES];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanCreateDirectories:NO];
    [openPanel setDelegate:self];
    [openPanel beginSheetModalForWindow:self.view.window completionHandler: (^(NSInteger result){
        [NSApp stopModalWithCode:result];
        if (result == NSModalResponseOK) {
            for (NSURL* URL in [openPanel URLs])
                [filePaths addObject:[URL path]];
        }
    })];
    [NSApp runModalForWindow:[self.view window]];
    return filePaths;
}

- (NSString*)filePathFromSavePanelWithFileName:(NSString*)fileName {
    __block NSString* savePath = nil;
    NSSavePanel* savePanel = [NSSavePanel savePanel];
    NSURL* URL = [[NSURL alloc] initFileURLWithPath:NSHomeDirectory() isDirectory:YES];
    [savePanel setDirectoryURL:URL];
    [savePanel setCanCreateDirectories:YES];
    [savePanel setDelegate:self];
    if (fileName)
        [savePanel setNameFieldStringValue:fileName];
    else
        [savePanel setNameFieldStringValue:@"Untitled"];
        
    [savePanel beginSheetModalForWindow:self.view.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:result];
        if (result == NSModalResponseOK) {
            savePath = [[savePanel URL] path];
        }
    })];
    [NSApp runModalForWindow:[self.view window]];
    return savePath;
}

- (IBAction)openFiles:(id)sender {
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    NSArray* filePaths = [self filePathsFromOpenPanel];
    CEEProject* project = nil;
    for (NSString* filePath in filePaths) {
        NSURL* url = [NSURL fileURLWithPath:filePath];
        NSString* typeName = [projectController typeForContentsOfURL:url error:nil];
        if ([typeName isEqualToString:@"com.lazycatdesign.cymbols.cymd"]) {
            project = [projectController documentForURL:url];
            if (project) {
                [project showWindows];
            }
            else {
                [projectController openProjectFromURL:url];
            }
        }
        else {
            [_session.project addSecurityBookmarksWithFilePaths:@[filePath]];
            [_session.activedPort openSourceBuffersWithFilePaths:@[filePath]];
        }
    }
}

- (IBAction)save:(id)sender {
    CEESourceBuffer* buffer = [_session.activedPort activedSourceBuffer];
    if ([buffer stateSet:kCEESourceBufferStateFileTemporary]) {
        NSString* fileName = [buffer.filePath lastPathComponent];
        NSString* savePath = [self filePathFromSavePanelWithFileName:fileName];
        if (savePath)
            [_session.activedPort saveSourceBuffer:buffer atFilePath:savePath];
    }
    else {
        if ([buffer stateSet:kCEESourceBufferStateModified]) {
            [_session.activedPort saveSourceBuffer:buffer atFilePath:buffer.filePath];
        }
    }
}

- (IBAction)saveAs:(id)sender {
    CEESourceBuffer* buffer = [_session.activedPort activedSourceBuffer];
    NSString* fileName = [buffer.filePath lastPathComponent];
    NSString* savePath = [self filePathFromSavePanelWithFileName:fileName];
    if (savePath) {
        [_session.activedPort saveSourceBuffer:buffer atFilePath:savePath];
    }
}

- (IBAction)saveAll:(id)sender {
    for (CEESessionPort* port in _session.ports) {
        for (CEESourceBuffer* buffer in [port openedSourceBuffers]) {
            if ([buffer stateSet:kCEESourceBufferStateFileTemporary]) {
                NSString* fileName = [buffer.filePath lastPathComponent];
                NSString* savePath = [self filePathFromSavePanelWithFileName:fileName];
                if (savePath)
                    [port saveSourceBuffer:buffer atFilePath:savePath];
            }
            else {
                if ([buffer stateSet:kCEESourceBufferStateModified]) {
                    [port saveSourceBuffer:buffer atFilePath:buffer.filePath];
                }
            }
        }
    }
}

- (void)setSession:(CEESession *)session {
    _session = session;
    [_toolbar setSession:session];
    if (_spliter)
        [_spliter setSession:session];
}

- (CEESession*)session {
    return _session;
}

- (IBAction)projectAddFile:(id)sender {
    if (_session.project.database)
        [self performSegueWithIdentifier:@"IDProjectAddFileSegue" sender:self]; 
}

- (IBAction)projectRemoveFile:(id)sender {
    if (_session.project.database)
        [self performSegueWithIdentifier:@"IDProjectRemoveFileSegue" sender:self];
}

- (void)setFrameAttachable:(BOOL)enable {
    CEESessionSourceController* controller = (CEESessionSourceController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceController"];
    [controller setFrameAttachable:enable];
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.view.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    serializing = [serializing stringByAppendingFormat:@"\"subviews\": ["];
    
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"%@", [_toolbar serialize]];
    serializing = [serializing stringByAppendingFormat:@"}"];
    serializing = [serializing stringByAppendingFormat:@","];
    
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"%@", [_spliter serialize]];
    serializing = [serializing stringByAppendingFormat:@"}"];
    
    serializing = [serializing stringByAppendingFormat:@"]"];
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    NSArray* subviews = dict[@"subviews"];
    for (NSDictionary* subview in subviews) {
        if (subview[@"IDToolbarView"])
            [_toolbar deserialize:subview[@"IDToolbarView"]];
        else if (subview[@"IDSessionSplit0View"])
            [_spliter deserialize:subview[@"IDSessionSplit0View"]];
    }
    
    if ([_toolbar shown])
        [self showToolbar:YES];
    else
        [self showToolbar:NO];
}

- (void)updateStatusBar {
    CEESessionPort* port = [_session activedPort];
    if (!port)
        return;
    
    [_statusBar setContent:[port descriptor]];
}

- (void)activeSourceBufferResponse:(NSNotification*)notification {
    if (notification.object != _session.activedPort)
        return;
    [self updateStatusBar];
}

- (void)openSourceBufferResponse:(NSNotification*)notification {
    if (notification.object != _session.activedPort)
        return;
    [self updateStatusBar];
    [self autoShowReferenceListWhenOpenedSourceBufferIsMultiple];
}

- (void)autoShowReferenceListWhenOpenedSourceBufferIsMultiple {
    if (_session.project.database ||
        _session.activedPort.openedSourceBuffers.count <= 1)
        return;
        
    AppDelegate* delegate = [NSApp delegate];
    NSDictionary* configurations = delegate.configurations;
    if (configurations[@"show_opened_files_list"]) {
        NSString* configuration = configurations[@"show_opened_files_list"];
        if ([configuration compare:@"auto" options:NSCaseInsensitiveSearch] == NSOrderedSame) {
            [_spliter showReferenceView:YES];
        }
    }
}

- (void)activePortResponse:(NSNotification*)notification {
    [self updateStatusBar];
}

- (void)setPortDescriptorResponse:(NSNotification*)notification {
    if (notification.object != _session.activedPort)
        return;
    [self updateStatusBar];
}

- (void)sourceBufferReloadResponse:(NSNotification*)notification {
    if (notification.object != _session.activedPort.activedSourceBuffer)
        return;
    [self updateStatusBar];
}

- (void)sourceBufferChangeStateResponse:(NSNotification*)notification {
    if (notification.object != _session.activedPort.activedSourceBuffer)
        return;
    [self updateStatusBar];
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    if (notification.object != _session.project)
        return;
    [self updateStatusBar];
    [self autoShowReferenceListWhenOpenedSourceBufferIsMultiple];
}

- (void)sourceBufferCloseResponse:(NSNotification*)notification {
    if (notification.object != _session.activedPort)
        return;
    [self updateStatusBar];
}

- (IBAction)buildProject:(id)sender {
    if (!_projectParseWindowController)
        _projectParseWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectParseWindowController"];
    CEEProjectParseViewController* controller = (CEEProjectParseViewController*)_projectParseWindowController.contentViewController;
    controller.sync = NO;
    [self.view.window beginSheet:_projectParseWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:result];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)syncProject:(id)sender {
    if (!_projectParseWindowController)
        _projectParseWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectParseWindowController"];
    CEEProjectParseViewController* controller = (CEEProjectParseViewController*)_projectParseWindowController.contentViewController;
    controller.sync = YES;
    [self.view.window beginSheet:_projectParseWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:result];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)cleanProject:(id)sender {
    if (!_projectCleanWindowController)
        _projectCleanWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectCleanWindowController"];
    
    [self.view.window beginSheet:_projectCleanWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:result];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (void)requestJumpSourcePointSelectionResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
     
    if (!_contextWindowController)
        _contextWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectContextWindowController"];
    NSModalResponse responese = [NSApp runModalForWindow:_contextWindowController.window];
    if (responese == NSModalResponseOK) {
        CEEProjectContextViewController* contextViewController = (CEEProjectContextViewController*)_contextWindowController.contentViewController;
        if (contextViewController.selectedSymbol) {
            CEESourceSymbol* symbol = contextViewController.selectedSymbol;
            CEESourcePoint* sourcePoint = [[CEESourcePoint alloc] initWithFilePath:[NSString stringWithUTF8String:symbol->filepath] andLocations:[NSString stringWithUTF8String:symbol->locations]];
            [port jumpToSourcePoint:sourcePoint];
        }
    }
    [_contextWindowController close];
}

- (void)searchReferenceResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
        
    if (!_projectSearchWindowController)
        _projectSearchWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectSearchWindowController"];
    
    CEEProjectSearchViewController* projectSearchViewController = (CEEProjectSearchViewController*)_projectSearchWindowController.contentViewController;
    
    [projectSearchViewController setAutoStart:YES];
    NSModalResponse responese = [NSApp runModalForWindow:_projectSearchWindowController.window];
    if (responese == NSModalResponseOK) {
        if (projectSearchViewController.selectedResult) {
            CEESearchResult* result = projectSearchViewController.selectedResult;
            CEESourcePoint* sourcePoint = [[CEESourcePoint alloc] initWithFilePath:result.filePath andLocations:result.locations];
            [port jumpToSourcePoint:sourcePoint];
        }
    }
    [projectSearchViewController setAutoStart:NO];
    [_projectSearchWindowController close];
}

- (IBAction)searchInProject:(id)sender {
    CEESessionPort* port = _session.activedPort;
    if (!port)
        return;
    
    if (!_projectSearchWindowController)
        _projectSearchWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectSearchWindowController"];
    
    CEEProjectSearchViewController* projectSearchViewController = (CEEProjectSearchViewController*)_projectSearchWindowController.contentViewController;
    
    NSModalResponse responese = [NSApp runModalForWindow:_projectSearchWindowController.window];
    if (responese == NSModalResponseOK) {
        if (projectSearchViewController.selectedResult) {
            CEESearchResult* result = projectSearchViewController.selectedResult;
            CEESourcePoint* sourcePoint = [[CEESourcePoint alloc] initWithFilePath:result.filePath andLocations:result.locations];
            [port jumpToSourcePoint:sourcePoint];
        }
    }
    [_projectSearchWindowController close];    
}

- (IBAction)showHelp:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://cymbols.io/manual.html"]];
}

- (IBAction)setPreferences:(id)sender {
    AppDelegate* delegate = [NSApp delegate];
    CEESessionPort* port = self.session.activedPort;
    [port openSourceBuffersWithFilePaths:@[[delegate configurationFilePath]]];
}

@end
