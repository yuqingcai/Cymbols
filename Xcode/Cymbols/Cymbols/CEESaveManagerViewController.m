//
//  CEESaveManagerViewController.m
//  Cymbols
//
//  Created by qing on 2020/7/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESaveManagerViewController.h"
#import "CEEStyleManager.h"
#import "AppDelegate.h"
#import "CEESourceBuffer.h"
#import "CEEEditViewController.h"

@interface CEESaveManagerViewController ()
@property (weak) IBOutlet CEETableView *sourceBufferTable;
@property (strong) CEEEditViewController *editViewController;
@property (weak) IBOutlet CEEView *sourceContentView;
@property (strong) NSMutableArray* selectedSourceBufferFilePaths;
@property (strong) NSString* directory;
@property (weak) IBOutlet CEECheckBox *selectAllButton;
@end

@implementation CEESaveManagerViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [_sourceBufferTable setNibNameOfCellView:@"TableCellViews"];
    [_sourceBufferTable setDelegate:self];
    [_sourceBufferTable setDataSource:self];
    [_sourceBufferTable setNumberOfColumns:1];
    [_sourceBufferTable setTarget:self];
    [_sourceBufferTable setAction:@selector(selectRow:)];
    [_sourceBufferTable setAllowsMultipleSelection:YES];
    [_sourceBufferTable setEnableDrawHeader:NO];
    [_sourceBufferTable setSelectable:YES];
    
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
    [_editViewController setWrap:YES];
    
    _directory = nil;
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    if (!_modifiedSourceBuffers || !_modifiedSourceBuffers.count)
        return;
    
    [_sourceBufferTable reloadData];
    
    if (!_sourceBufferTable.selectedRowIndexes ||
        _sourceBufferTable.selectedRow == -1)
        return;
    
    CEESourceBuffer* buffer = _modifiedSourceBuffers[_sourceBufferTable.selectedRow];
    [_editViewController setBuffer:buffer];
    
    _selectedSourceBufferFilePaths = [[NSMutableArray alloc] init];
    [_selectAllButton setState:NSControlStateValueOn];
    [_selectAllButton sendAction:_selectAllButton.action to:_selectAllButton.target];
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    return [_modifiedSourceBuffers count];
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FilePath";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CEECheckboxTableCellView *cellView = nil;
    CEESourceBuffer* buffer = _modifiedSourceBuffers[row];
    cellView = [_sourceBufferTable makeViewWithIdentifier:@"IDCheckboxTableCellView"];
    if (cellView) {
        if (!cellView.delegate)
            [cellView setDelegate:self];
        
        cellView.text.stringValue = [buffer.filePath lastPathComponent];
        cellView.selectedIdentifier = buffer.filePath;
        if ([self sourceBufferIsSelected:buffer.filePath])
            [cellView.checkbox setState:NSControlStateValueOn];
        else
            [cellView.checkbox setState:NSControlStateValueOff];
            
    }
    return cellView;
}

- (IBAction)cancel:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseCancel];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (CEESourceBuffer*)modifiedSourceBufferWithFilePath:(NSString*)filePath {
    for (CEESourceBuffer* buffer in _modifiedSourceBuffers) {
        if ([buffer.filePath isEqualToString:filePath])
            return buffer;
    }
    return nil;
}

- (IBAction)save:(id)sender {
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    CEEProject* project = [projectController currentDocument];
    AppDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* sourceBufferManager = [delegate sourceBufferManager];
    
    for (NSString* filePath in _selectedSourceBufferFilePaths) {
        NSString* fileName = [filePath lastPathComponent];
        NSSavePanel* savePanel = [NSSavePanel savePanel];
        NSString * savePath = nil;
        NSURL* URL = nil;
        CEESourceBuffer* buffer = nil;
        NSModalResponse responese = NSModalResponseCancel;
        
        buffer = [self modifiedSourceBufferWithFilePath:filePath];
        if (!buffer)
            continue;
    
        if ([buffer stateSet:kCEESourceBufferStateFileTemporary]) {
            if (!_directory)
                URL = [[NSURL alloc] initFileURLWithPath:NSHomeDirectory() isDirectory:YES];
            else
                URL = [[NSURL alloc] initFileURLWithPath:_directory isDirectory:YES];

            NSString* filePathBeforeSave = buffer.filePath;
            [savePanel setDirectoryURL:URL];
            [savePanel setCanCreateDirectories:YES];
            [savePanel setDelegate:self];
            [savePanel setNameFieldStringValue:fileName];
            responese = [savePanel runModal];
            
            if (responese == NSModalResponseOK) {
                savePath = [[savePanel URL] path];
                if (savePath) {
                    BOOL ret = [sourceBufferManager saveSourceBuffer:buffer atFilePath:savePath];
                    if (!ret) {
                        NSLog(@"Save Source Buffer Failed!");
                        continue;
                    }
                    CEEProjectController* controller = (CEEProjectController*)[NSDocumentController sharedDocumentController];
                    [controller replaceSourceBufferReferenceFilePath:filePathBeforeSave to:savePath];
                    _directory = [savePath stringByDeletingLastPathComponent];
                }
            }
        }
        else {
            [sourceBufferManager saveSourceBuffer:buffer atFilePath:buffer.filePath];
        }
        
        [project syncSourceSymbols:buffer];
    }
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)discardAll:(id)sender {
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)selectAll:(id)sender {
    _selectedSourceBufferFilePaths = nil;
    if (_selectAllButton.state == NSControlStateValueOn) {
        _selectedSourceBufferFilePaths = [[NSMutableArray alloc] init];
        for (CEESourceBuffer* buffer in _modifiedSourceBuffers)
            [_selectedSourceBufferFilePaths addObject:buffer.filePath];
    }
    
    [_sourceBufferTable reloadData];
}

- (BOOL)sourceBufferIsSelected:(NSString*)filePath {
    for (NSInteger i = 0; i < _selectedSourceBufferFilePaths.count; i ++) {
        if ([_selectedSourceBufferFilePaths[i] compare:filePath options:NSLiteralSearch] == NSOrderedSame)
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

#pragma mark - protocol CEEFilePathConfirmCellDelegate
- (void)select:(NSString*)filePath {
    if (![self sourceBufferIsSelected:filePath])
        [_selectedSourceBufferFilePaths addObject:filePath];
}

- (void)deselect:(NSString*)filePath {
    if ([self sourceBufferIsSelected:filePath])
        [_selectedSourceBufferFilePaths removeObject:filePath];
}

@end
