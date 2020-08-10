//
//  CEESourceBufferManagerViewController.m
//  Cymbols
//
//  Created by qing on 2020/7/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESourceBufferManagerViewController.h"
#import "CEEStyleManager.h"
#import "CymbolsDelegate.h"
#import "CEESourceBuffer.h"
#import "CEESourceBufferSaveConfirmCellView.h"
#import "CEESourceViewController.h"

@interface CEESourceBufferManagerViewController ()
@property (weak) IBOutlet CEETableView *sourceBufferTableView;
@property (strong) CEESourceViewController *sourceViewController;
@property (weak) IBOutlet CEEView *sourceContentView;
@end

@implementation CEESourceBufferManagerViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_sourceBufferTableView setDelegate:self];
    [_sourceBufferTableView setDataSource:self];
    [_sourceBufferTableView setColumns:1];
    [_sourceBufferTableView reloadData];
    [_sourceBufferTableView setAllowsMultipleSelection:YES];
    
    _sourceViewController =  [[NSStoryboard storyboardWithName:@"SourceViewController" bundle:nil] instantiateControllerWithIdentifier:@"IDTextEditViewController"];
    [_sourceViewController.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [_sourceContentView addSubview:_sourceViewController.view];
    NSDictionary *views = @{
        @"textEditView" : _sourceViewController.view,
    };
    NSArray *constraintsH = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[textEditView]-1-|" options:0 metrics:nil views:views];
    NSArray *constraintsV = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-1-[textEditView]-1-|" options:0 metrics:nil views:views];
    [_sourceContentView addConstraints:constraintsH];
    [_sourceContentView addConstraints:constraintsV];
    
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    CymbolsDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* bufferManager = [delegate sourceBufferManager];
    return [[bufferManager buffers] count];
}

- (NSString *)tableView:(CEETableView *)tableView titleForColumn:(NSInteger)column {
    return @"FilePath";
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    CymbolsDelegate* delegate = [NSApp delegate];
    CEESourceBufferManager* bufferManager = [delegate sourceBufferManager];
    NSArray* buffers = [bufferManager buffers];
    CEESourceBufferSaveConfirmCellView *cellView = nil;
    CEESourceBuffer* buffer = buffers[row];
    cellView = [_sourceBufferTableView makeViewWithIdentifier:@"IDSourceBufferSaveConfirmCellView"];
    cellView.title.stringValue = [buffer.filePath lastPathComponent];
    return cellView;
}

- (CEEView*)tableView:(CEETableView*)tableView viewWithIdentifier:(NSString*)identifier {
    CymbolsDelegate* delegate = [NSApp delegate];
    return (CEEView*)[delegate nibObjectWithIdentifier:identifier fromNibNamed:@"TableCellViews"];
}

- (IBAction)cancel:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseCancel];
}

- (IBAction)saveAll:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseOK];
}

- (IBAction)discardAll:(id)sender {
    [NSApp stopModalWithCode:NSModalResponseOK];
}


@end
