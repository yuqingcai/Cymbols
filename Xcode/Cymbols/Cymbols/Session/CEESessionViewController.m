//
//  CEESessionViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/4.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEESessionViewController.h"
#import "CEEStyleManager.h"
#import "CEESessionSpliter0.h"
#import "CEESessionToolbar.h"
#import "CEEProjectCreatorController.h"
#import "AppDelegate.h"
#import "CEESessionView.h"
#import "CEESessionSourceViewController.h"
#import "CEESessionStatusBar.h"
#import "CEEProjectParseViewController.h"
#import "CEEProjectContextViewController.h"
#import "CEEProjectCleanViewController.h"
#import "CEEProjectSearchViewController.h"
#import "CEESaveManagerViewController.h"
#import "CEEUpdateInfoViewController.h"
#import "CEETitlebarButton.h"
#import "CEETextTitle.h"

@interface CEESessionViewController ()

@property (strong) IBOutlet CEESessionView *sessionView;
@property (strong) CEESessionToolbar* toolbar;
@property (weak) IBOutlet CEESessionStatusBar *statusBar;
@property (weak) IBOutlet CEEView *containerView;
@property (weak) CEESessionSpliter0* spliter;
@property CGFloat titleHeight;
@property (strong) NSWindowController* projectCreatorWindowController;
@property (strong) NSWindowController* projectParseWindowController;
@property (strong) NSWindowController* projectCleanWindowController;
@property (strong) NSWindowController* projectSearchWindowController;
@property (strong) NSWindowController* timeFreezerWindowController;
@property (strong) NSWindowController* contextWindowController;
@property (strong) NSWindowController* addReferenceWindowController;
@property (strong) NSWindowController* removeReferenceWindowController;
@property (strong) NSWindowController* referenceRootScannerWindowController;
@property (strong) NSWindowController* updateInfoWindowController;
@property (strong) CEEWindowController* preferenceWindowController;
@property (weak) IBOutlet CEEToolbarButton *messageButton;
@property (weak) IBOutlet CEETextTitle *trialVersionTitle;

@end

@implementation CEESessionViewController

@synthesize sheetOffset = _sheetOffset;
@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_containerView setTranslatesAutoresizingMaskIntoConstraints:NO];
    _titleHeight = 26;
    [self checkUpdate];
    
    [_trialVersionTitle setHidden:YES];
#ifdef TRIAL_VERSION
    [_trialVersionTitle setHidden:NO];
#endif
    
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
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(searchReferenceRequestResponse:) name:CEENotificationSessionPortSearchReferenceRequest object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(cymbolsUpdateNotifyResponse:) name:CEENotificationCymbolsUpdateNotify object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(cymbolsUpdateConfirmResponse:) name:CEENotificationCymbolsUpdateConfirm object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(jumpToSymbolRequestResponse:) name:CEENotificationSessionPortJumpToSymbolRequest object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(timeFreezeResponse:) name:CEENotificationTimeFreeze object:nil];
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

- (IBAction)toggleSourceBufferView:(id)sender {
    [_spliter toggleSourceBufferView];
}

- (IBAction)toggleFileReferenceView:(id)sender {
    [_spliter toggleFileReferenceView];
}

- (IBAction)toggleContextView:(id)sender {
    CEESessionSourceViewController* controller = (CEESessionSourceViewController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceViewController"];
    [controller toggleContextView];
}

- (IBAction)splitHorizontally:(id)sender {
    CEESessionSourceViewController* controller = (CEESessionSourceViewController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceViewController"];
    [controller splitViewHorizontally];
}

- (IBAction)splitVertically:(id)sender {
    CEESessionSourceViewController* controller = (CEESessionSourceViewController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceViewController"];
    [controller splitViewVertically];
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

- (IBAction)closeFile:(id)sender {
    if (!_session.activedPort ||
        !_session.activedPort.activedSourceBuffer)
        return;
    
    CEESessionPort* port = _session.activedPort;
    CEESourceBuffer* buffer = [port activedSourceBuffer];
    NSWindowController* saveManagerWindowController = nil;
    CEESaveManagerViewController* controller = nil;
    if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile]) {
        saveManagerWindowController = [[NSStoryboard storyboardWithName:@"SaveManager" bundle:nil] instantiateControllerWithIdentifier:@"IDSaveManagerWindowController"];;
        controller = (CEESaveManagerViewController*)saveManagerWindowController.contentViewController;
        [controller setModifiedSourceBuffers:@[buffer]];
        
        [self.view.window beginSheet:saveManagerWindowController.window completionHandler:(^(NSInteger result) {
            if (result == NSModalResponseOK)
                [port closeSourceBuffers:@[buffer]];
            [controller setModifiedSourceBuffers:nil];
            [NSApp stopModalWithCode:NSModalResponseOK];
        })];
        [NSApp runModalForWindow:self.view.window];
    }
    else {
        [port closeSourceBuffers:@[buffer]];
    }
}

- (IBAction)closeWindow:(id)sender {
    if ([self.view.window.delegate windowShouldClose:self.view.window])
        [self.view.window close];
}

- (IBAction)newProject:(id)sender {
    if (!_projectCreatorWindowController)
        _projectCreatorWindowController = [[NSStoryboard storyboardWithName:@"ProjectCreator" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectCreatorWindowController"];
    
    [self.view.window beginSheet:_projectCreatorWindowController.window completionHandler:(^(NSInteger result) {
    })];
}

- (IBAction)closeProject:(id)sender {
    [_session.project close];
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
    __block CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    NSURL* URL = [[NSURL alloc] initFileURLWithPath:NSHomeDirectory() isDirectory:YES];
    NSOpenPanel* openPanel = [NSOpenPanel openPanel];
    [openPanel setDirectoryURL:URL];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setAllowsMultipleSelection:YES];
    [openPanel setCanChooseFiles:YES];
    [openPanel setCanCreateDirectories:NO];
    [openPanel setDelegate:self];
    [openPanel beginSheetModalForWindow:self.view.window completionHandler: (^(NSInteger result) {
        if (result == NSModalResponseOK) {
            for (NSURL* URL in [openPanel URLs]) {
                NSString* typeName = [projectController typeForContentsOfURL:URL error:nil];
                
                if ([typeName isEqualToString:@"com.lazycatdesign.cymbols.cymd"]) {
                    CEEProject* project = [projectController documentForURL:URL];
                    if (project)
                        [project showWindows];
                    else
                        [projectController openProjectFromURL:URL];
                }
                else {
                    [self->_session.activedPort openSourceBufferWithFilePath:[URL path]];
                }
            }
        }
    })];
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
        if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile]) {
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
                if ([buffer stateSet:kCEESourceBufferStateShouldSyncToFile]) {
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

- (IBAction)addReferencesToProject:(id)sender {
    if (!_addReferenceWindowController)
        _addReferenceWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDAddReferenceWindowController"];
    [self.view.window beginSheet:_addReferenceWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)removeReferencesFromProject:(id)sender {
    if (!_removeReferenceWindowController)
        _removeReferenceWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDRemoveReferenceWindowController"];
    [self.view.window beginSheet:_removeReferenceWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)scanReferenceRoots:(id)sender {
    if (!_referenceRootScannerWindowController)
        _referenceRootScannerWindowController = [[NSStoryboard storyboardWithName:@"ReferenceManager" bundle:nil] instantiateControllerWithIdentifier:@"IDReferenceRootScannerWindowController"];
    [self.view.window beginSheet:_referenceRootScannerWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (void)setFrameAttachable:(BOOL)enable {
    CEESessionSourceViewController* controller = (CEESessionSourceViewController*)[_spliter viewControllerByIdentifier:@"IDSessionSourceViewController"];
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
    NSDictionary* configuration = delegate.configuration;
    if (configuration[@"show_opened_files"]) {
        NSString* value = configuration[@"show_opened_files"];
        if ([value compare:@"auto" options:NSCaseInsensitiveSearch] == NSOrderedSame) {
            [_spliter showSourceBufferView:YES];
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

- (void)jumpToSymbolRequestResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    
    if (port.session != _session ||
        !_session.sourceContext || !_session.sourceContext->symbols)
        return;
    
    if (cee_list_length(_session.sourceContext->symbols) == 1) {
        [_session jumpToSymbol:cee_list_nth_data(_session.sourceContext->symbols, 0) inPort:_session.pinnedPort];
        return;
    }
    
    if (!_contextWindowController)
        _contextWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectContextWindowController"];
    NSModalResponse responese = [NSApp runModalForWindow:_contextWindowController.window];
    if (responese == NSModalResponseOK) {
        CEEProjectContextViewController* contextViewController = (CEEProjectContextViewController*)_contextWindowController.contentViewController;
        if (contextViewController.selectedSymbolRef)
            [port.session jumpToSymbol:contextViewController.selectedSymbolRef inPort:_session.pinnedPort];
    }
    [_contextWindowController close];
}

- (void)searchReferenceRequestResponse:(NSNotification*)notification {
    CEESessionPort* port = notification.object;
    if (port.session != _session)
        return;
    
    if (!_projectSearchWindowController)
        _projectSearchWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectSearchWindowController"];
    
    CEEProjectSearchViewController* projectSearchViewController = (CEEProjectSearchViewController*)_projectSearchWindowController.contentViewController;
    
    [projectSearchViewController setAutoStart:YES];
    NSModalResponse responese = [NSApp runModalForWindow:_projectSearchWindowController.window];
    if (responese == NSModalResponseOK) {
        if (projectSearchViewController.selectedResult)
            [port.session jumpToSourcePoint:projectSearchViewController.selectedResult inPort:_session.pinnedPort];
    }
    [projectSearchViewController setAutoStart:NO];
    [_projectSearchWindowController close];
}

- (void)timeFreezeResponse:(NSNotification*)notification {
    [self showTrialWelcome];
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
        if (projectSearchViewController.selectedResult)
            [port.session jumpToSourcePoint:projectSearchViewController.selectedResult inPort:_session.pinnedPort];
    }
    [_projectSearchWindowController close];
}

- (IBAction)buildProject:(id)sender {
    if (!_projectParseWindowController)
        _projectParseWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectParseWindowController"];
    CEEProjectParseViewController* controller = (CEEProjectParseViewController*)_projectParseWindowController.contentViewController;
    controller.sync = NO;
    [self.view.window beginSheet:_projectParseWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)syncProject:(id)sender {
    if (!_projectParseWindowController)
        _projectParseWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectParseWindowController"];
    CEEProjectParseViewController* controller = (CEEProjectParseViewController*)_projectParseWindowController.contentViewController;
    controller.sync = YES;
    [self.view.window beginSheet:_projectParseWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)cleanProject:(id)sender {
    if (!_projectCleanWindowController)
        _projectCleanWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectCleanWindowController"];
    
    [self.view.window beginSheet:_projectCleanWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
}

- (IBAction)getHelp:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://www.cymbols.io/help"]];
}

- (IBAction)getManual:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://www.cymbols.io/manual"]];
}

- (IBAction)reportBug:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://www.cymbols.io/bug"]];
}

- (IBAction)requestNewFeature:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://www.cymbols.io/feature"]];
}


- (void)cymbolsUpdateNotifyResponse:(id)sender {
    [_messageButton setEnabled:YES];
    [_messageButton setHidden:NO];
}

- (void)cymbolsUpdateConfirmResponse:(id)sender {
    [_messageButton setEnabled:NO];
    [_messageButton setHidden:YES];
}

- (void)checkUpdate {
    AppDelegate* delegate = [NSApp delegate];
    if ([delegate.network updateFlagSet]) {
        [_messageButton setEnabled:YES];
        [_messageButton setHidden:NO];
    }
    else {
        [_messageButton setEnabled:NO];
        [_messageButton setHidden:YES];
    }
}

- (IBAction)update:(id)sender {
    if (!_updateInfoWindowController)
        _updateInfoWindowController = [[NSStoryboard storyboardWithName:@"UpdateInfo" bundle:nil] instantiateControllerWithIdentifier:@"IDUpdateInfoWindowController"];
    
    AppDelegate* delegate = [NSApp delegate];
    CEEUpdateInfoViewController* viewController = (CEEUpdateInfoViewController*)_updateInfoWindowController.contentViewController;
    viewController.infoString = [delegate.network updateInfoString];
    [self.view.window beginSheet:_updateInfoWindowController.window completionHandler:(^(NSInteger result) {
        [NSApp stopModalWithCode:NSModalResponseOK];
    })];
    [NSApp runModalForWindow:self.view.window];
    [delegate.network cleanUpdateFlag];
}

- (IBAction)setPreferences:(id)sender {
    if (!_preferenceWindowController)
        _preferenceWindowController = [[NSStoryboard storyboardWithName:@"Preferences" bundle:nil] instantiateControllerWithIdentifier:@"IDPreferencesWindowController"];
    [NSApp runModalForWindow:_preferenceWindowController.window];
    [_preferenceWindowController close];
}

- (void)showTrialWelcome {
    if (!_timeFreezerWindowController)
        _timeFreezerWindowController = [[NSStoryboard storyboardWithName:@"TimeFreezer" bundle:nil] instantiateControllerWithIdentifier:@"IDTimeFreezerWindowController"];
    if ([self.view.window isMainWindow]) {
        [self.view.window beginSheet:_timeFreezerWindowController.window completionHandler:(^(NSInteger result) {
            [NSApp stopModalWithCode:NSModalResponseOK];
        })];
        [NSApp runModalForWindow:self.view.window];
    }
}

@end
