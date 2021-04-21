//
//  CEECheckboxTableCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEETableCellView.h"
#import "CEETableCellViewTextLabel.h"
#import "CEECheckBox.h"

NS_ASSUME_NONNULL_BEGIN

@protocol CEECheckboxTableCellViewDelegate <NSObject>
@required
- (void)select:(NSString*)identifier;
- (void)deselect:(NSString*)identifier;
@end

@interface CEECheckboxTableCellView : CEETableCellView
@property (weak) IBOutlet CEECheckBox* checkbox;
@property (weak) IBOutlet CEETableCellViewTextLabel *text;

@property (weak) id<CEECheckboxTableCellViewDelegate> delegate;
@property (strong) NSString* selectedIdentifier;

@end

NS_ASSUME_NONNULL_END
