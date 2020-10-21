//
//  CEEViewController.m
//  Cymbols
//
//  Created by 蔡于清 on 2018/8/15.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import "CEEViewController.h"
#import "CEESplitViewController.h"
#import "CEEIdentifier.h"
#import "CEEStyleManager.h"

@implementation CEEViewController

- (void)initProperties {
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

- (instancetype)initWithNibName:(NSNibName)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    [self.view setStyleConfiguration:[styleManager userInterfaceConfiguration]];
    [self.view setStyleState:kCEEViewStyleStateActived];
}

- (NSString*)serialize {
    NSString* serializing = [NSString stringWithFormat:@"\"%@\": ", self.view.identifier];
    serializing = [serializing stringByAppendingFormat:@"{"];
    serializing = [serializing stringByAppendingFormat:@"\"size\":\"%@\",", NSStringFromSize(self.view.frame.size)];
    serializing = [serializing stringByAppendingString:@"\"subviews\":["];
    
    serializing = [serializing stringByAppendingFormat:@"]"];
    serializing = [serializing stringByAppendingFormat:@"}"];
    return serializing;
}

- (void)deserialize:(NSDictionary*)dict {
    [self.view setFrameSize:NSSizeFromString(dict[@"size"])];
}
@end
