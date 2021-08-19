//
//  CEEStorePayment.h
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/10.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>


NS_ASSUME_NONNULL_BEGIN

extern NSNotificationName CEENotificationPaymentSelection;
extern NSNotificationName CEENotificationPurchaseError;
extern NSNotificationName CEENotificationPurchaseSuccessful;
extern NSNotificationName CEENotificationPurchaseNoProductRestored;
extern NSNotificationName CEENotificationPurchaseExpiresed;

typedef NS_ENUM(NSUInteger, CEEPurchaseState) {
    kCEEPurchaseStateSuccessful,
    kCEEPurchaseStateCancel,
    kCEEPurchaseStateError,
};

@interface CEEProductDescriptor : NSObject
@property (strong) NSString* title;
@property (strong) NSString* details;
@property (strong) NSString* price;
@end

@interface CEEStorePayment : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver, NSURLSessionDelegate, SKRequestDelegate>
@property NSArray* productDescriptors;
+ (CEEStorePayment *)sharedInstance;
- (BOOL)verify;
- (void)purchaseProductByIndex:(NSInteger)index;
- (void)restore;
@end

NS_ASSUME_NONNULL_END
