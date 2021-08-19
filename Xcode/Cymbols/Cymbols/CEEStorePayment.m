//
//  CEEStorePayment.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/8/10.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEStorePayment.h"

// Receipt verify URLs
NSString* productionURLString = @"https://buy.itunes.apple.com/verifyReceipt";
NSString* sandboxURLString = @"https://sandbox.itunes.apple.com/verifyReceipt";

// Shared Secret, generate URL: https://appstoreconnect.apple.com/access/shared-secret
NSString* sharedSecret = @"c1377177fac14906935fcaa1ac55a8a4";

// Products
NSString* subscriptionYearlyWithIntroductory = @"com.cymbols.subscription.yearly.introductory";
NSString* subscriptionMonthlyWithIntroductory = @"com.cymbols.subscription.monthly.introductory";
NSString* subscriptionYearly = @"com.cymbols.subscription.yearly";
NSString* subscriptionMonthly = @"com.cymbols.subscription.monthly";


NSNotificationName CEENotificationPaymentSelection = @"CEENotificationPaymentSelection";
NSNotificationName CEENotificationPurchaseError = @"CEENotificationPurchaseError";
NSNotificationName CEENotificationPurchaseSuccessful = @"CEENotificationPurchaseSuccessful";
NSNotificationName CEENotificationPaymentVirifyError = @"CEENotificationPaymentVirifyError";
NSNotificationName CEENotificationPurchaseNoProductRestored = @"CEENotificationPurchaseNoProductRestored";

typedef NS_ENUM(NSUInteger, CEEStorePaymentType) {
    kCEEStorePaymentTypeNone,
    kCEEStorePaymentTypeIntroductoryOffer,
    kCEEStorePaymentTypeRegularSubscription,
};

@implementation CEEProductDescriptor
@end

@interface CEEStorePayment()
@property BOOL verified;
@property (strong) NSArray* products;
@property dispatch_semaphore_t semaphore_products;
@property NSDictionary *receiptVerified;
@property NSMutableArray* transcationsRestored;
@end

@implementation CEEStorePayment

+ (CEEStorePayment *)sharedInstance {
    static dispatch_once_t onceToken;
    static CEEStorePayment * storePaymentSharedInstance;
    
    dispatch_once(&onceToken, ^{
        storePaymentSharedInstance = [[CEEStorePayment alloc] init];
        storePaymentSharedInstance.verified = NO;
        storePaymentSharedInstance.semaphore_products = dispatch_semaphore_create(0);
    });
    return storePaymentSharedInstance;
}

- (BOOL)verify {
    
    if (_verified)
        return YES;
    
    NSData *receipt = [NSData dataWithContentsOfURL:[NSBundle mainBundle].appStoreReceiptURL];
    
    if (!receipt) {
        exit(173);
    }
    else {
        NSURL *productionURL = [NSURL URLWithString:productionURLString];
        NSURL *sandBoxURL = [NSURL URLWithString:sandboxURLString];
        
        NSError* error = nil;
        NSData* data = nil;
        
        [self verifyReceipt:receipt fromStoreURL:sandBoxURL data:&data error:&error];
        if (error) {
            if (error.code == 21007) {
                [self verifyReceipt:receipt fromStoreURL:productionURL data:&data error:&error];
                if (error) {
                    NSLog(@"verify receipt error: %ld", error.code);
                    return NO;
                }
            }
            else {
                NSLog(@"verify receipt error: %ld", error.code);
                return NO;
            }
        }
        
        if (data) {
            _receiptVerified = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
            CEEStorePaymentType paymentType = [self detectPaymentType:_receiptVerified];
            [self makePaymentProcess:paymentType];
        }
        else {
            NSLog(@"verify receipt error: no data");
            return NO;
        }
    }
    
    _verified = YES;
    return YES;
}

- (void)restore {
    _transcationsRestored = [[NSMutableArray alloc] init];
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}


- (void)verifyReceipt:(NSData*)receipt fromStoreURL:(NSURL*)url data:(NSData**)receiveData error:(NSError**)receiveError {
    NSString *encodedReceipt = [receipt base64EncodedStringWithOptions:0];
    NSDictionary *contents = @{
        @"receipt-data": encodedReceipt,
        @"password": sharedSecret,
    };
    NSData *body = [NSJSONSerialization dataWithJSONObject:contents options:0 error:nil];
    NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:url];
    [request setHTTPMethod:@"POST"];
    [request setHTTPBody:body];
    
    NSURLSessionConfiguration * configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
    configuration.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    NSURLSession * session = [NSURLSession sessionWithConfiguration:configuration];
    
    __block dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    __block NSError* error0 = nil;
    __block NSData* data0 = nil;
    NSURLSessionDataTask * dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        error0 = error;
        data0 = data;
        dispatch_semaphore_signal(sem);
    }];
    [dataTask resume];
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
    
    *receiveData = data0;
    *receiveError = error0;
}

- (CEEStorePaymentType)detectPaymentType:(NSDictionary*)userInfo {
    NSLog(@"%@", userInfo);
    
    NSArray* latest_receipt_info = userInfo[@"latest_receipt_info"];
    if (!latest_receipt_info || !latest_receipt_info.count)
        return kCEEStorePaymentTypeIntroductoryOffer;
        
    NSDictionary* latest = userInfo[@"latest_receipt_info"][0];
    NSString* expires_date_string = latest[@"expires_date"];
    
    //NSLog(@"%@", userInfo[@"latest_receipt_info"]);
    //NSLog(@"%@", userInfo[@"pending_renewal_info"]);
    //NSString* expires_date_string_local = [self getLocalDateFormateUTCDate:expires_date_string];
    //NSLog(@"expiresed date: %@", expires_date_string_local);
    
    NSDate* expires_date = [self getUTCDate:expires_date_string];
    NSDate* current_date = [NSDate date];
    
    if ([expires_date compare:current_date] == NSOrderedAscending) {
        NSLog(@"expiresed");
        return kCEEStorePaymentTypeRegularSubscription;
    }
    
    return kCEEStorePaymentTypeNone;
}

- (NSString *)getLocalDateFormateUTCDate:(NSString *)utcStr {
    NSDateFormatter *format = [[NSDateFormatter alloc] init];
    format.timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    format.dateFormat = @"yyyy-MM-dd HH:mm:ss VV";
    NSDate *utcDate = [format dateFromString:utcStr];
    format.timeZone = [NSTimeZone localTimeZone];
    NSString *dateString = [format stringFromDate:utcDate];
    return dateString;
}

- (NSDate *)getUTCDate:(NSString *)utcStr {
    NSDateFormatter *format = [[NSDateFormatter alloc] init];
    format.timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    format.dateFormat = @"yyyy-MM-dd HH:mm:ss VV";
    NSDate *utcDate = [format dateFromString:utcStr];
    return utcDate;
}

- (void)makePaymentProcess:(CEEStorePaymentType)paymentType {
    if (paymentType == kCEEStorePaymentTypeNone)
        return;
    
    NSSet* productIdentifiers = nil;
    
    if (paymentType == kCEEStorePaymentTypeIntroductoryOffer)
        productIdentifiers = [NSSet setWithArray:@[
            subscriptionYearlyWithIntroductory,
            subscriptionMonthlyWithIntroductory,
        ]];
    else if (paymentType == kCEEStorePaymentTypeRegularSubscription)
        productIdentifiers = [NSSet setWithArray:@[
            subscriptionYearly,
            subscriptionMonthly,
        ]];
    
    SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    request.delegate = self;
    [request start];
    dispatch_semaphore_wait(_semaphore_products, DISPATCH_TIME_FOREVER);
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationPaymentSelection object:self];
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    
    NSMutableArray* descriptors = [[NSMutableArray alloc] init];
    _products = response.products;
    
    for (SKProduct* product in _products) {
        CEEProductDescriptor* descriptor = [[CEEProductDescriptor alloc] init];
        
        NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFormatter setLocale:product.priceLocale];
        NSString *priceFormattedString = [numberFormatter stringFromNumber:product.price];
        
        NSLog(@"%@", product.localizedTitle);
        NSLog(@"%@", product.localizedDescription);
        
        descriptor.title = product.localizedTitle;
        descriptor.details = product.localizedDescription;
        descriptor.price = priceFormattedString;
        
        [descriptors addObject:descriptor];
        
        if ([descriptor.title rangeOfString:@"short-term member ship" options:NSCaseInsensitiveSearch].location != NSNotFound)
            descriptor.title = [descriptor.title stringByAppendingFormat:@" (%@/month)", descriptor.price];
        else if ([descriptor.title rangeOfString:@"long-term member ship" options:NSCaseInsensitiveSearch].location != NSNotFound)
            descriptor.title = [descriptor.title stringByAppendingFormat:@" (%@/year)", descriptor.price];
    }
    _productDescriptors = descriptors;
    
    dispatch_semaphore_signal(_semaphore_products);
}

- (void)purchaseProductByIndex:(NSInteger)index {
    if (!_products || index >= _products.count)
        return;
    
    SKPayment* payment = [SKPayment paymentWithProduct:_products[index]];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction*> *)transactions {
    for (SKPaymentTransaction* transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchased:
                [self handleTranscactionPurchased:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self handleTranscactionFailed:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self handleTranscationRestored:transaction];
                break;
            default:
                break;
        }
    }
    
    if (_transcationsRestored)
        [self restoredApplicationFeatures];
}

- (void)handleTranscactionPurchased:(SKPaymentTransaction*)transaction {
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    dispatch_async(dispatch_get_main_queue(), ^(void){
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationPurchaseSuccessful object:self];
    });
}

- (void)handleTranscactionFailed:(SKPaymentTransaction*)transaction {
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    dispatch_async(dispatch_get_main_queue(), ^(void){
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationPurchaseError object:self];
    });
}

- (void)handleTranscationRestored:(SKPaymentTransaction*)transaction {
    if (_transcationsRestored)
        [_transcationsRestored addObject:transaction];
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

- (void)restoredApplicationFeatures {
    dispatch_async(dispatch_get_main_queue(), ^(void){
        [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationPurchaseNoProductRestored object:self];
    });
}

@end
