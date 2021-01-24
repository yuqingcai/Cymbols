//
//  CEESessionReferenceTableView.h
//  Cymbols
//
//  Created by qing on 2020/9/17.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableView.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESessionReferenceTableView : CEETableView
- (void)highlightRowRect:(NSInteger)rowIndex enable:(BOOL)flag;
@end

NS_ASSUME_NONNULL_END
