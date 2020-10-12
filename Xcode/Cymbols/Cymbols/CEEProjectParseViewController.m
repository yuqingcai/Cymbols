//
//  CEEProjectParseViewController.m
//  Cymbols
//
//  Created by qing on 2020/8/25.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEEProjectParseViewController.h"
#import "CEETextLabel.h"
#import "cee_backend.h"
#import "CEEButton.h"

@interface CEEProjectParseViewController ()
@property (strong) CEEProject* project;
@property (weak) IBOutlet NSProgressIndicator *progressBar;
@property (weak) IBOutlet CEETextLabel *label;
@property (weak) IBOutlet CEEButton *button;
@property BOOL cancel;

@end

@implementation CEEProjectParseViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)viewDidAppear {
    _cancel = NO;
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    _project = [projectController currentDocument];
    __block NSArray* filePaths = [_project getFilePathsWithCondition:nil];
    if (!filePaths) {
        [self->_label setStringValue:@"No Source should be Parsed."];
        [self->_button setTitle:@"Complete"];
        [_progressBar setMinValue:0.0];
        [_progressBar setMaxValue:1.0];
        [_progressBar setDoubleValue:1.0];
        return;
    }
    
    cee_database_symbols_clean(_project.database);
    [_progressBar setMinValue:0.0];
    [_progressBar setMaxValue:filePaths.count];
    [_progressBar setDoubleValue:0.0];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        for (int i = 0; i < filePaths.count; i ++) {
            NSString* filePath = filePaths[i];
            CEEList* list = NULL;
            CEESourceBuffer* buffer = [[CEESourceBuffer alloc] initWithFilePath:filePath];
            
            cee_source_buffer_parse(buffer, 0);
            cee_source_fregment_symbol_tree_dump_to_list(buffer.prep_directive_symbol_tree, &list);
            cee_source_fregment_symbol_tree_dump_to_list(buffer.statement_symbol_tree, &list);
            
            if (list) {
                cee_database_symbols_write(self->_project.database, list);
                cee_list_free(list);
            }
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self->_label setStringValue:[filePath lastPathComponent]];
                [self->_progressBar setDoubleValue:(double)i+1];
            });
            
            if (self->_cancel)
                break;
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            filePaths = nil;
            [self->_button setTitle:@"Complete"];
        });
    });
}

- (IBAction)cancel:(id)sender {
    _cancel = YES;
    
    if (self.view.window.sheetParent)
        [self.view.window.sheetParent endSheet:self.view.window returnCode:NSModalResponseOK];
    
    if ([self.view.window isModalPanel])
        [NSApp stopModalWithCode:NSModalResponseOK];
}

@end
