//
//  CEESessionSpliter0.h
//  Cymbols
//
//  Created by qing on 2020/7/1.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEESplitViewController.h"
#import "CEEProject.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionSpliter0 : CEESplitViewController
@property (weak) CEESession* session;
- (void)toggleSymbolView;
- (void)toggleReferenceView;
- (void)togglePathView;
- (void)showSymbolView:(BOOL)shown;
- (void)showReferenceView:(BOOL)shown;
- (void)showPathView:(BOOL)shown;
@end

NS_ASSUME_NONNULL_END
