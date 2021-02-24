//
//  CEESessionSourceViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2019/8/1.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "AppDelegate.h"
#import "CEESessionSourceViewController.h"
#import "CEESessionFrameManager.h"

@interface CEESessionSourceViewController ()
@property CEESessionSpliter1* spliter;
@end

@implementation CEESessionSourceViewController

@synthesize session = _session;

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {
    if ([[segue identifier] caseInsensitiveCompare:@"IDSessionSourceEmbedSegue"] == NSOrderedSame)
        _spliter = [segue destinationController];
}


- (void)setSession:(CEESession *)session {
    _session = session;
    if (_spliter)
        [_spliter setSession:session];
}

- (CEESession*)session {
    return _session;
}

- (void)toggleContextView {
    [_spliter toggleViewByIdentifier:@"IDSessionContextViewController"];
}

- (void)showContextView:(BOOL)shown {
    [_spliter showView:shown byIdentifier:@"IDSessionContextViewController"];
}

- (void)splitViewHorizontally {
    if (!_spliter)
        return;
    
    CEESessionFrameManager* frameManager = (CEESessionFrameManager*)[_spliter viewControllerByIdentifier:@"IDSessionFrameManager"];
    [frameManager split:kCEESplitFrameDirectionHorizontal];
}

- (void)splitViewVertically {
    if (!_spliter)
        return;
    
    CEESessionFrameManager* frameManager = (CEESessionFrameManager*)[_spliter viewControllerByIdentifier:@"IDSessionFrameManager"];
    [frameManager split:kCEESplitFrameDirectionVertical];
}

- (void)setFrameAttachable:(BOOL)enable {
    CEESessionFrameManager* frameManager = (CEESessionFrameManager*)[_spliter viewControllerByIdentifier:@"IDSessionFrameManager"];
    [frameManager setFrameAttachable:enable];
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\":", self.view.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    serializing = [serializing stringByAppendingFormat:@"\"subviews\": ["];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"%@", [_spliter serialize]];
    serializing = [serializing stringByAppendingFormat:@"}"];
    serializing = [serializing stringByAppendingFormat:@"]"];
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    [self.view setFrameSize:NSSizeFromString(dict[@"size"])];
    NSArray* descriptors = dict[@"subviews"];
    for (NSDictionary* descriptor in descriptors) {
        if (descriptor[@"IDSessionSplit1View"])
            [_spliter deserialize:descriptor[@"IDSessionSplit1View"]];
    }
}

@end
