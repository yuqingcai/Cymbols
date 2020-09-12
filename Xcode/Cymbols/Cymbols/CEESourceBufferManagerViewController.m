//
//  CEESourceBufferManagerViewController.m
//  Cymbols
//
//  Created by qing on 2020/7/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESourceBufferManagerViewController.h"
#import "CEEStyleManager.h"
#import "AppDelegate.h"
#import "CEESourceBuffer.h"
#import "CEEEditViewController.h"

@interface CEESourceBufferManagerViewController ()
@property (weak) IBOutlet CEETableView *sourceBufferTable;
@property (strong) CEEEditViewController *editViewController;
@property (weak) IBOutlet CEEView *sourceContentView;
@property (strong) NSMutableArray* modifiedSourceBuffers;
@property (strong) NSMutableArray* selectedSourceBufferIdentifiers;
@property (strong) NSString* directory;
@property (weak) IBOutlet CEECheckBox *selectAllButton;

@end

@implementation CEESourceBufferManagerViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_sourceBufferTable setDelegate:self];
    [_sourceBufferTable setDataSource:self];
    [_sourceBufferTable setColumns:1];
    [_sourceBufferTable setTarget:self];
    [_sourceBufferTable setAction:@selector(selectRow:)];
    [_sourceBufferTable setAllowsMultipleSelection:YES];
    [_sourceBufferTable setEnableDrawHeader:NO];
    
    _editViewController =  [[NSStoryboard storyboardWithName:@"Editor" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_editViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceContentView addSubview:_editViewController.view];
    NSDictionary *views = @{
        @"textEditView" : _editViewController.view,
    };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[textEditView]-0-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[textEditView]-0-|" options:0 metrics:nil views:views];
    [_sourceContentView addConstraints:constraintsH];
    [_sourceContentView addConstraints:constraintsV];
    [_editViewController setEditable:NO];
    [_editViewController setIntelligence:NO];
    _directory = nil;
}

- (void)viewDidAppear {
    [super viewDidAppear];
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* bufferManager = [delegate sourceBufferManager];
    NSArray* buffers = [bufferManager buffers];
    
    _modifiedSourceBuffers = [[NSMutableArray alloc] init];
    for (CEESourceBuffer* buffer in buffers) {
        if (buffer.state & kCEESourceBufferStateModified)
            [_modifiedSourceBuffers addObject:buffer];
    }
    
    _selectedSourceBufferIdentifiers = [[NSMutableArray alloc] init];
    [_sourceBufferTable reloadData];
    
    if (!_sourceBufferTable.selectedRowIndexes ||
        _sourceBufferTable.selectedRow == -1)
        return;    
    CEESourceBuffer* buffer = _modifiedSourceBuffers[_sourceBufferTable.selectedRow];
    [_editViewController setBuffer:buffer];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    return [_modifiedSourceBuffers count];
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FilePath";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEESourceBufferSaveConfirmCellView *cellView = nil;
    CEESourceBuffer* buffer = _modifiedSourceBuffers[row];
    cellView = [_sourceBufferTable makeViewWithIdentifier:@"IDSourceBufferSaveConfirmCellView"];
    if (cellView) {
        if (!cellView.delegate)
            [cellView setDelegate:self];
        
        cellView.title.stringValue = [buffer.filePath lastPathComponent];
        cellView.sourceBufferIdentifier = buffer.filePath;
        
        if ([self sourceBufferIsSelectedWithIdentifier:buffer.filePath])
            [cellView.check setState:NSControlStateValueOn];
        else
            [cellView.check setState:NSControlStateValueOff];
            
    }
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    AppDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (IBAction)cancel:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (IBAction)save:(id)sender {
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    
    for (NSString* identifier in _selectedSourceBufferIdentifiers) {
        NSString* fileName = [identifier lastPathComponent];
        NSSavePanel* savePanel = [NSSavePanel savePanel];
        NSString * savePath = nil;
        NSURL* URL = nil;
        CEESourceBuffer* sourceBuffer = nil;
        NSModalResponse responese = NSModalResponseCancel;
        
        if ([sourceBufferManager fileExistsAtPath:identifier]) {
            URL = [[NSURL alloc] initFileURLWithPath:[identifier stringByDeletingLastPathComponent] isDirectory:YES];
        }
        else {
            if (!_directory)
                URL = [[NSURL alloc] initFileURLWithPath:NSHomeDirectory() isDirectory:YES];
            else
                URL = [[NSURL alloc] initFileURLWithPath:_directory isDirectory:YES];
        }
        
        sourceBuffer = [sourceBufferManager getSourceBufferWithFilePath:identifier];
        if (!sourceBuffer)
            continue;
        
        [savePanel setDirectoryURL:URL];
        [savePanel setCanCreateDirectories:YES];
        [savePanel setDelegate:self];
        if (fileName)
            [savePanel setNameFieldStringValue:fileName];
        else
            [savePanel setNameFieldStringValue:@"Untitled"];
        
        responese = [savePanel runModal];
        if (responese == NSModalResponseOK) {
            savePath = [[savePanel URL] path];
            [sourceBufferManager saveSourceBuffer:sourceBuffer atPath:savePath];
            _directory = [savePath stringByDeletingLastPathComponent];
        }
    }
    
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)discardAll:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)selectAll:(id)sender {
    _selectedSourceBufferIdentifiers = nil;
    if (_selectAllButton.state == NSOnState) {
        _selectedSourceBufferIdentifiers = [[NSMutableArray alloc] init];
        for (CEESourceBuffer* buffer in _modifiedSourceBuffers)
            [_selectedSourceBufferIdentifiers addObject:buffer.filePath];
    }
    
    [_sourceBufferTable reloadData];
}

- (BOOL)sourceBufferIsSelectedWithIdentifier:(NSString*)identifier {
    for (NSInteger i = 0; i < _selectedSourceBufferIdentifiers.count; i ++) {
        if ([_selectedSourceBufferIdentifiers[i] compare:identifier options:NSLiteralSearch] == NSOrderedSame)
            return YES;
    }
    return NO;
}

- (IBAction)selectRow:sender {
    if (!_sourceBufferTable.selectedRowIndexes ||
        _sourceBufferTable.selectedRow == -1)
        return;    
    CEESourceBuffer* buffer = _modifiedSourceBuffers[_sourceBufferTable.selectedRow];
    [_editViewController setBuffer:buffer];
}

#pragma mark - protocol CEESourceBufferSaveConfirmCellDelegate
- (void)sourceBufferSelect:(NSString*)identifier {
    if (![self sourceBufferIsSelectedWithIdentifier:identifier])
        [_selectedSourceBufferIdentifiers addObject:identifier];
}

- (void)sourceBufferDeselect:(NSString*)identifier {
    if ([self sourceBufferIsSelectedWithIdentifier:identifier])
        [_selectedSourceBufferIdentifiers removeObject:identifier];
}

@end
