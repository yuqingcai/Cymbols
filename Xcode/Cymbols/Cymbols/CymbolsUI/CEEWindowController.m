//
//  CEEWindowController.m
//  Cymbols
//
//  Created by yuqingcai on 2018/10/16.
//  Copyright © 2018 Lazycatdesign. All rights reserved.
//

#import "CEEStyleManager.h"
#import "CEEWindowController.h"
#import "CEEViewController.h"
#import "CEEView.h"
#import "CEEIdentifier.h"

@implementation CEEWindowController

- (void)initProperties {
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateUserInterfaceStyle:) name:CEENotificationUserInterfaceStyleUpdate object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateUserInterfaceStyleComplete:) name:CEENotificationUserInterfaceStyleUpdateComplete object:nil];
}

- (void)updateUserInterfaceStyle:(NSNotification*)notification {
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [self.contentViewController setViewStyleConfiguration:[styleManager userInterfaceConfiguration]];
}

- (void)updateUserInterfaceStyleComplete:(NSNotification*)notification {
    [self.contentViewController setViewStyleConfigurationComplete];
}


- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithWindow:(NSWindow *)window {
    self = [super initWithWindow:window];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithWindowNibName:(NSNibName)windowNibName owner:(id)owner {
    self = [super initWithWindowNibName:windowNibName owner:owner];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithWindowNibPath:(NSString *)windowNibPath owner:(id)owner {
    self = [super initWithWindowNibPath:windowNibPath owner:owner];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)windowDidBecomeMain:(NSNotification *)notification {
    [self.contentViewController setViewStyleState:kCEEViewStyleStateActived];
}

- (void)windowDidResignMain:(NSNotification *)notification {
    [self.contentViewController setViewStyleState:kCEEViewStyleStateDeactived];
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\": ", self.window.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (BOOL)deserialize:(NSDictionary*)dict {
    return YES;
}

@end
