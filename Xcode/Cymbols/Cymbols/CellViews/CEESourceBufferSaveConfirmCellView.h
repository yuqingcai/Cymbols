//
//  CEESourceBufferSaveConfirmCellView.h
//  Cymbols
//
//  Created by qing on 2020/8/6.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEEView.h"
#import "CEETableCellViewTextLabel.h"
#import "CEECheckBox.h"

NS_ASSUME_NONNULL_BEGIN

@protocol CEESourceBufferSaveConfirmCellDelegate <NSObject>
@required
- (void)sourceBufferSelect:(NSString*)identifier;
- (void)sourceBufferDeselect:(NSString*)identifier;
@end

@interface CEESourceBufferSaveConfirmCellView : CEEView
@property (weak) IBOutlet CEETableCellViewTextLabel *title;
@property (weak) IBOutlet CEECheckBox* check;
@property (weak) id<CEESourceBufferSaveConfirmCellDelegate> delegate;
@property (strong) NSString* sourceBufferIdentifier;

@end

NS_ASSUME_NONNULL_END
