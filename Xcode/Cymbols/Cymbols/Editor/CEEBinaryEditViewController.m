//
//  CEEBinaryEditViewController.m
//  Cymbols
//
//  Created by caiyuqing on 2018/11/29.
//  Copyright © 2018 caiyuqing. All rights reserved.
//
#import "AppDelegate.h"
#import "CEEBinaryEditViewController.h"
#import "CEETitleView.h"
#import "CEEScroller.h"
#import "CEEBinaryView.h"

@interface CEEBinaryEditViewController()
@property (weak) IBOutlet CEEBinaryView *binaryView;

@end

@implementation CEEBinaryEditViewController

- (void)viewDidAppear {    
    [self.view.window makeFirstResponder:self.view];
}

- (void)setBuffer:(CEESourceBuffer*)buffer {
    (void)self.view;
    [super setBuffer:buffer];
    [self.port setDescriptor:[self createPortDescriptor]];
}

- (NSString*)createPortDescriptor {
    return @"Binary Preview";
}

@end
