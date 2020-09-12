//
//  CEESplitView.h
//  Cymbols
//
//  Created by Qing on 2018/9/13.
//  Copyright © 2018年 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NSSplitView+UIStyle.h"
#import "CEESerialization.h"

NS_ASSUME_NONNULL_BEGIN

@interface CEESplitView : NSSplitView {
    CEEUserInterfaceStyleConfiguration* _styleConfiguration;
    CEEViewStyleState _styleState;
}
@property NSPointerArray* userInterfaceStyles;
@property (strong) NSColor* dividerColorVertical;
@property (strong) NSColor* dividerColorHorizontal;
@property (strong) NSColor* borderColor;
@property CGFloat borderWidth;
@property CGFloat cornerRadius;

@end

NS_ASSUME_NONNULL_END
