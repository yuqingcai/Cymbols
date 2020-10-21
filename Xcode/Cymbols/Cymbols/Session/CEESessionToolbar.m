//
//  CEESessionToolbar.m
//  Cymbols
//
//  Created by caiyuqing on 2019/7/12.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//
#import "CEESessionToolbar.h"
#import "CEEButton.h"
#import "CEEToolbarButton.h"

@interface CEESessionToolbar ()
@property (weak) IBOutlet CEEToolbarButton *createFileButton;
@property (weak) IBOutlet CEEToolbarButton *createProjectButton;
@property (weak) IBOutlet CEEToolbarButton *openFileButton;
@property (weak) IBOutlet CEEToolbarButton *saveFileButton;
@property (weak) IBOutlet CEEToolbarButton *saveAllFileButton;
@property (weak) IBOutlet CEEToolbarButton *dumplicateButton;
@property (weak) IBOutlet CEEToolbarButton *cutButton;
@property (weak) IBOutlet CEEToolbarButton *pasteButton;
@property (weak) IBOutlet CEEToolbarButton *searchButton;
@property (weak) IBOutlet CEEToolbarButton *undoButton;
@property (weak) IBOutlet CEEToolbarButton *redoButton;
@property (weak) IBOutlet CEEToolbarButton *buildProjectButton;
@property (weak) IBOutlet CEEToolbarButton *cleanProjectButton;
@property (weak) IBOutlet CEEToolbarButton *syncProjectButton;
@property (weak) IBOutlet CEEToolbarButton *searchInProjectButton;
@property (weak) IBOutlet CEEToolbarButton *createWindwoButton;
@property (weak) IBOutlet CEEToolbarButton *listWindowButton;
@property (weak) IBOutlet CEEToolbarButton *splitHorizontalButton;
@property (weak) IBOutlet CEEToolbarButton *splitVerticalButton;
@property (weak) IBOutlet CEEToolbarButton *toggleToolbarButton;
@property (weak) IBOutlet CEEToolbarButton *toggleContextButton;
@property (weak) IBOutlet CEEToolbarButton *toggleSymbolButton;
@property (weak) IBOutlet CEEToolbarButton *toggleReferenceButton;
@property (weak) IBOutlet CEEToolbarButton *toggleFilePathButton;

@end

@implementation CEESessionToolbar

@synthesize shown = _shown;

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self setShown:YES];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionPresentResponse:) name:CEENotificationSessionPresent object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(projectSettingPropertiesResponse:) name:CEENotificationProjectSettingProperties object:nil];
}

- (void)setShown:(BOOL)shown {
    _shown = shown;
}

- (BOOL)shown {
    return _shown;
}

- (NSString*)serialize {
    // serialize view controller start
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.view.identifier];
    serializing = [serializing stringByAppendingString:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    
    if (_shown)
        serializing = [serializing stringByAppendingString:@"\"shown\":\"YES\""];
    else
        serializing = [serializing stringByAppendingString:@"\"shown\":\"NO\""];
    
    serializing = [serializing stringByAppendingString:@"}"];
    // serialize view controller end
    
    return serializing;
    
}

- (void)deserialize:(NSDictionary*)dict {
    if ([dict[@"shown"] caseInsensitiveCompare:@"YES"] == NSOrderedSame)
        _shown = YES;
    else
        _shown = NO;
}

- (void)sessionPresentResponse:(NSNotification*)notification {
    BOOL hidden = YES;
    if (_session.project.database)
        hidden = NO;
    
    
    //[_buildProjectButton setEnabled:enable];
    //[_cleanProjectButton setEnabled:enable];
    //[_syncProjectButton setEnabled:enable];
    //[_searchInProjectButton setEnabled:enable];
    
    [_buildProjectButton setHidden:hidden];
    [_cleanProjectButton setHidden:hidden];
    [_syncProjectButton setHidden:hidden];
    [_searchInProjectButton setHidden:hidden];
}

- (void)projectSettingPropertiesResponse:(NSNotification*)notification {
    BOOL hidden = YES;
    if (_session.project.database)
        hidden = NO;
    
    //[_buildProjectButton setEnabled:enable];
    //[_cleanProjectButton setEnabled:enable];
    //[_syncProjectButton setEnabled:enable];
    //[_searchInProjectButton setEnabled:enable];
    
    [_buildProjectButton setHidden:hidden];
    [_cleanProjectButton setHidden:hidden];
    [_syncProjectButton setHidden:hidden];
    [_searchInProjectButton setHidden:hidden];
}

@end
