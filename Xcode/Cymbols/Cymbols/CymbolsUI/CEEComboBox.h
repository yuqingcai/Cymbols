//
//  CEEComboBox.h
//  Cymbols
//
//  Created by qing on 2020/7/8.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CEEControl.h"
#import "CEEView.h"
#import "CEEPopupPanel.h"
#import "CEETableView.h"

NS_ASSUME_NONNULL_BEGIN


@protocol CEEComboBoxDelegate;

@interface CEEComboBox : CEEControl <CEEPopupPanelDelegate, CEETableViewDelegate, CEETableViewDataSource>
@property (strong) IBInspectable NSColor* backgroundColor;
@property (strong) IBInspectable NSColor* borderColor;
@property (strong) IBInspectable NSColor* textColor;
@property (strong) IBInspectable NSShadow* textShadow;
@property (strong) IBInspectable NSGradient* gradient;
@property (strong) IBInspectable NSString* borders;
@property (strong) IBInspectable NSString* title;
@property (strong) IBInspectable NSImage* icon;
@property (strong) IBInspectable NSColor* iconColor;
@property IBInspectable CGFloat gradientAngle;
@property IBInspectable CGFloat borderWidth;
@property IBInspectable CGFloat cornerRadius;
@property NSControlStateValue state;
@property NSInteger indexOfSelectedItem;

@property (nullable, weak) id<CEEComboBoxDelegate> delegate;
@property (strong) NSString* nibNameOfCellView;
@property BOOL isEnabled;
- (void)addItem:(NSString*)item;
- (NSString*)itemAtIndex:(NSInteger)index;
- (void)removeAllItems;
@end

@protocol CEEComboBoxDelegate <NSObject>
- (void)selectedIndexChangedOfComboBox:(CEEComboBox*)comboBox;
@end

NS_ASSUME_NONNULL_END

