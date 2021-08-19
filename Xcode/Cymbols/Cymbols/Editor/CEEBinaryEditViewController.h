//
//  CEEEditViewController.h
//  Cymbols
//
//  Created by caiyuqing on 2018/11/29.
//  Copyright © 2018 caiyuqing. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "cee_lib.h"
#import "CEEProject.h"
#import "CEEEditViewController.h"
#import "CEEBinaryView.h"
#import "CEEASCIIView.h"

@interface CEEBinaryEditViewController : CEEEditViewController <CEEBinaryViewDelegate, CEEASCIIViewDelegate>
@end

