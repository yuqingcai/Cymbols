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

@interface CEESessionViewController ()
@property (strong) IBOutlet CEESessionView *sessionView;
@property (strong) CEESessionToolbar* toolbar;
@property (weak) IBOutlet CEESessionStatusBar *statusBar;
@property (weak) IBOutlet CEEView *containerView;
@property (weak) CEESessionSpliter0* spliter;
@property CGFloat titleHeight;
@end

@implementation CEESessionViewController

@synthesize sheetOffset = _sheetOffset;
@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
    [_containerView setTranslatesAutoresizingMaskIntoConstraints:NO];
    _titleHeight = 23;
    
    _toolbar = [[NSStoryboard storyboardWithName:@"Session" bundle:nil] instantiateControllerWithIdentifier:@"IDSessionToolbar"];
    [self addChildViewController:_toolbar];
    [self showToolbar:YES];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortActiveSourceBufferResponse:) name:CEENotificationSessionPortActiveSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPortOpenSourceBufferResponse:) name:CEENotificationSessionPortOpenSourceBuffer object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionActivePortResponse:) name:CEENotificationSessionActivePort object:nil];
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

- (IBAction)toggleUserInterfaceStyleSelectionController:(id)sender {
    [self performSegueWithIdentifier:@"IDUserInterfaceStyleSelectionSegue" sender:self];
}

- (IBAction)toggleTextHighlightStyleSelectionController:(id)sender {
    [self performSegueWithIdentifier:@"IDTextHighlightStyleSelectionSegue" sender:self];
}

- (IBAction)newWindow:(id)sender {
    CEEProjectController* controller = [NSDocumentController sharedDocumentController];
    [controller createProjectFromSetting:nil];
}

- (IBAction)listWindows:(id)sender {
    [_session.project showWindows];
}

- (IBAction)newFile:(id)sender {
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
            if (project)
                [project showWindows];
            else
                [projectController openProjectFromURL:url];
        }
        else {
            [_session.activedPort openSourceBufferWithFilePath:filePath];
        }
    }
}

- (IBAction)save:(id)sender {
    CEESourceBuffer* buffer = [[_session.activedPort currentBufferReference] buffer];
    if ([buffer stateSet:kCEESourceBufferStateFileUntitled]) {
        NSString* fileName = [buffer.filePath lastPathComponent];
        NSString* savePath = [self filePathFromSavePanelWithFileName:fileName];
        if (savePath)
            [_session.activedPort saveSourceBuffer:buffer atPath:savePath];
    }
    else {
        if ([buffer stateSet:kCEESourceBufferStateModified]) {
            [_session.activedPort saveSourceBuffer:buffer atPath:buffer.filePath];
        }
    }
}

- (IBAction)saveAs:(id)sender {
    CEESourceBuffer* buffer = [[_session.activedPort currentBufferReference] buffer];
    NSString* fileName = [buffer.filePath lastPathComponent];
    NSString* savePath = [self filePathFromSavePanelWithFileName:fileName];
    if (savePath)
        [_session.activedPort saveSourceBuffer:buffer atPath:savePath];
}

- (IBAction)saveAll:(id)sender {
    for (CEESourceBuffer* buffer in [_session registeredSourceBuffers]) {
        if ([buffer stateSet:kCEESourceBufferStateFileUntitled]) {
            NSString* fileName = [buffer.filePath lastPathComponent];
            NSString* savePath = [self filePathFromSavePanelWithFileName:fileName];
            if (savePath)
                [_session.activedPort saveSourceBuffer:buffer atPath:savePath];
        }
        else {
            if ([buffer stateSet:kCEESourceBufferStateModified]) {
                [_session.activedPort saveSourceBuffer:buffer atPath:buffer.filePath];
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

- (void)sessionPortActiveSourceBufferResponse:(NSNotification*)notification {
    //CEESessionPort* port = notification.object;
    //if (port.session != _session)
    //    return;
    //CEEBufferReference* reference = port.currentReference;
    //CEESourceBuffer* sourceBuffer = reference.buffer;
    //CEETextStorageRef storage = sourceBuffer.storage;
    //cee_ulong size = cee_text_storage_size_get(storage);
    //[_statusBar setContent:[NSString stringWithFormat:@"Ln 1, Col1    UTF-8    LF    Space: 4    Size: %lu    Plain Text", size]];
}

- (void)sessionPortOpenSourceBufferResponse:(NSNotification*)notification {
    //CEESessionPort* port = notification.object;
    //if (port.session != _session)
    //    return;
    //CEEBufferReference* reference = port.currentReference;
    //CEESourceBuffer* sourceBuffer = reference.buffer;
    //CEETextStorageRef storage = sourceBuffer.storage;
    //cee_ulong size = cee_text_storage_size_get(storage);
    //[_statusBar setContent:[NSString stringWithFormat:@"Ln 1, Col1    UTF-8    LF    Space: 4    Size: %lu    Plain Text", size]];
}

- (void)sessionActivePortResponse:(NSNotification*)notification {
    //CEESession* session = notification.object;
    //if (session != _session)
    //    return;
    //CEESessionPort* port = session.activedPort;
    //CEEBufferReference* reference = port.currentReference;
    //CEESourceBuffer* sourceBuffer = reference.buffer;
    //CEETextStorageRef storage = sourceBuffer.storage;
    //cee_ulong size = cee_text_storage_size_get(storage);
    //[_statusBar setContent:[NSString stringWithFormat:@"Ln 1, Col1    UTF-8    LF    Space: 4    Size: %lu    Plain Text", size]];
}

- (IBAction)buildProject:(id)sender {
    NSWindowController* projectParseWindowController = [[NSStoryboard storyboardWithName:@"ProjectProcess" bundle:nil] instantiateControllerWithIdentifier:@"IDProjectParseWindowController"];
    NSModalResponse responese = [NSApp runModalForWindow:projectParseWindowController.window];
    [projectParseWindowController close];
}

- (IBAction)cleanProject:(id)sender {
    cee_database_symbols_clean(_session.project.database);
}

- (IBAction)syncProject:(id)sender {
    NSLog(@"sync project");
}

- (IBAction)searchInProject:(id)sender {
    NSLog(@"search project");
}

@end
