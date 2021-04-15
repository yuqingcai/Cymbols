//
//  CEEPopupPanel.h
//  Cymbols
//
//  Created by qing on 2020/7/8.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN


@protocol CEEPopupPanelDelegate;

extern const CGFloat CEEPopupPanelSizeDefaultWidth;
extern const CGFloat CEEPopupPanelSizeDefaultHeight;
extern const CGFloat CEEPopupPanelSizeMinimumWidth;
extern const CGFloat CEEPopupPanelSizeMinimumHeight;

@interface CEEPopupPanel : NSPanel
@property BOOL exclusived;
@property (weak) id<CEEPopupPanelDelegate> popupDelegate;
- (CGFloat)expactedWidth;
- (CGFloat)expactedHeight;
- (CGFloat)minimumWidth;
- (CGFloat)minimumHeight;
@end

@protocol CEEPopupPanelDelegate <NSObject>
- (void)closePopupPanel:(CEEPopupPanel*)popupPanel;
@end

NS_ASSUME_NONNULL_END
