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

@interface CEEProjectParseViewController ()
@property (strong) CEEProject* project;
@property (weak) IBOutlet NSProgressIndicator *progressBar;
@property (weak) IBOutlet CEETextLabel *label;
@property cee_boolean cancel;
@end

@implementation CEEProjectParseViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEProjectController* projectController = [NSDocumentController sharedDocumentController];
    _project = [projectController currentDocument];
}

- (void)viewDidAppear {
    if (!_project || !_project.database)
        [NSApp stopModalWithCode:NSModalResponseCancel];
    
    cee_database_symbols_clean(_project.database);
    
    __block NSArray* filePaths = [_project getFilePathsWithCondition:nil];
    
    if (filePaths) {
        [_progressBar setMinValue:0.0];
        [_progressBar setMaxValue:filePaths.count];
        
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
                    [self->_progressBar setDoubleValue:(double)i];
                });
                
                if (self->_cancel)
                    break;
            }
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                [NSApp stopModalWithCode:NSModalResponseOK];
            });
            
        });
        
    }
}

- (IBAction)cancel:(id)sender {
    _cancel = TRUE;
    [NSApp stopModalWithCode:NSModalResponseStop];
}

@end
