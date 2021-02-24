//
//  CEENetwork.m
//  Cymbols
//
//  Created by qing on 2020/9/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEENetwork.h"
#import "AppDelegate.h"
#import "cee_datetime.h"
#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/network/IOEthernetInterface.h>
#import <IOKit/network/IONetworkInterface.h>
#import <IOKit/network/IOEthernetController.h>

NSNotificationName CEENotificationCymbolsUpdateNotify = @"CEENotificationCymbolsUpdateNotify";
NSNotificationName CEENotificationCymbolsUpdateConfirm = @"CEENotificationCymbolsUpdateConfirm";

static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices);
static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize);

// Returns an iterator containing the primary (built-in) Ethernet interface. The caller is responsible for
// releasing the iterator after the caller is done with it.
static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
{
    kern_return_t kernResult;
    CFMutableDictionaryRef matchingDict;
    CFMutableDictionaryRef propertyMatchDict;
    
    // Ethernet interfaces are instances of class kIOEthernetInterfaceClass. 
    // IOServiceMatching is a convenience function to create a dictionary with the key kIOProviderClassKey and 
    // the specified value.
    matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);

    // Note that another option here would be:
    // matchingDict = IOBSDMatching("en0");
    // but en0: isn't necessarily the primary interface, especially on systems with multiple Ethernet ports.
        
    if (NULL == matchingDict) {
        printf("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
        // Each IONetworkInterface object has a Boolean property with the key kIOPrimaryInterface. Only the
        // primary (built-in) interface has this property set to TRUE.
        
        // IOServiceGetMatchingServices uses the default matching criteria defined by IOService. This considers
        // only the following properties plus any family-specific matching in this order of precedence 
        // (see IOService::passiveMatch):
        //
        // kIOProviderClassKey (IOServiceMatching)
        // kIONameMatchKey (IOServiceNameMatching)
        // kIOPropertyMatchKey
        // kIOPathMatchKey
        // kIOMatchedServiceCountKey
        // family-specific matching
        // kIOBSDNameKey (IOBSDNameMatching)
        // kIOLocationMatchKey
        
        // The IONetworkingFamily does not define any family-specific matching. This means that in            
        // order to have IOServiceGetMatchingServices consider the kIOPrimaryInterface property, we must
        // add that property to a separate dictionary and then add that to our matching dictionary
        // specifying kIOPropertyMatchKey.
            
        propertyMatchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                      &kCFTypeDictionaryKeyCallBacks,
                                                      &kCFTypeDictionaryValueCallBacks);
    
        if (NULL == propertyMatchDict) {
            printf("CFDictionaryCreateMutable returned a NULL dictionary.\n");
        }
        else {
            // Set the value in the dictionary of the property with the given key, or add the key 
            // to the dictionary if it doesn't exist. This call retains the value object passed in.
            CFDictionarySetValue(propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue); 
            
            // Now add the dictionary containing the matching value for kIOPrimaryInterface to our main
            // matching dictionary. This call will retain propertyMatchDict, so we can release our reference 
            // on propertyMatchDict after adding it to matchingDict.
            CFDictionarySetValue(matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
            CFRelease(propertyMatchDict);
        }
    }
    
    // IOServiceGetMatchingServices retains the returned iterator, so release the iterator when we're done with it.
    // IOServiceGetMatchingServices also consumes a reference on the matching dictionary so we don't need to release
    // the dictionary explicitly.
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, matchingServices);    
    if (KERN_SUCCESS != kernResult) {
        printf("IOServiceGetMatchingServices returned 0x%08x\n", kernResult);
    }
    
    return kernResult;
}
    
// Given an iterator across a set of Ethernet interfaces, return the MAC address of the last one.
// If no interfaces are found the MAC address is set to an empty string.
// In this sample the iterator should contain just the primary interface.
static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize)
{
    io_object_t intfService;
    io_object_t controllerService;
    kern_return_t kernResult = KERN_FAILURE;
    
    // Make sure the caller provided enough buffer space. Protect against buffer overflow problems.
    if (bufferSize < kIOEthernetAddressSize) {
        return kernResult;
    }
    
    // Initialize the returned address
    bzero(MACAddress, bufferSize);
    
    // IOIteratorNext retains the returned object, so release it when we're done with it.
    while ((intfService = IOIteratorNext(intfIterator)))
    {
        CFTypeRef    MACAddressAsCFData;        

        // IONetworkControllers can't be found directly by the IOServiceGetMatchingServices call, 
        // since they are hardware nubs and do not participate in driver matching. In other words,
        // registerService() is never called on them. So we've found the IONetworkInterface and will 
        // get its parent controller by asking for it specifically.
        
        // IORegistryEntryGetParentEntry retains the returned object, so release it when we're done with it.
        kernResult = IORegistryEntryGetParentEntry(intfService,
                                                   kIOServicePlane,
                                                   &controllerService);
        
        if (KERN_SUCCESS != kernResult) {
            printf("IORegistryEntryGetParentEntry returned 0x%08x\n", kernResult);
        }
        else {
            // Retrieve the MAC address property from the I/O Registry in the form of a CFData
            MACAddressAsCFData = IORegistryEntryCreateCFProperty(controllerService,
                                                                 CFSTR(kIOMACAddress),
                                                                 kCFAllocatorDefault,
                                                                 0);
            if (MACAddressAsCFData) {
                CFShow(MACAddressAsCFData); // for display purposes only; output goes to stderr
                
                // Get the raw bytes of the MAC address from the CFData
                CFDataGetBytes(MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress);
                CFRelease(MACAddressAsCFData);
            }
                
            // Done with the parent Ethernet controller object so we release it.
            (void) IOObjectRelease(controllerService);
        }
        
        // Done with the Ethernet interface object so we release it.
        (void) IOObjectRelease(intfService);
    }
        
    return kernResult;
}

#define TRIGGER_SECOND 60

@interface CEENetwork() {
    UInt8 _MACAddress[kIOEthernetAddressSize];
}

@property NSInteger beatCount;
@property BOOL isMACAddressFound;
@property BOOL isSendingUserInfo;
@property BOOL isAcquiringUpdateInfo;
@property NSInteger notifyInterval;
@end

@implementation CEENetwork

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self getMACAddress];
    _isSendingUserInfo = NO;
    _isAcquiringUpdateInfo = NO;
    _notifyInterval = (NSInteger)(TRIGGER_SECOND / CEE_APP_HEART_BEAT_INTERVAL);
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(heartBeatResponse:) name:CEENotificationHeartBeat object:nil];
    
    return self;
}

- (void)getMACAddress {
    kern_return_t kernResult = KERN_SUCCESS;
    io_iterator_t intfIterator;
    _isMACAddressFound = NO;
    
    kernResult = FindEthernetInterfaces(&intfIterator);
       
    if (KERN_SUCCESS != kernResult) {
        fprintf(stdout, "FindEthernetInterfaces returned 0x%08x\n", kernResult);
    }
    else {
        kernResult = GetMACAddress(intfIterator, _MACAddress, sizeof(_MACAddress));
        
        if (KERN_SUCCESS != kernResult) {
            fprintf(stdout, "GetMACAddress returned 0x%08x\n", kernResult);
        }
        else {
            _isMACAddressFound = YES;
            printf("This system's built-in MAC address is %02x:%02x:%02x:%02x:%02x:%02x.\n",
                    _MACAddress[0], _MACAddress[1], _MACAddress[2], 
                   _MACAddress[3], _MACAddress[4], _MACAddress[5]);
        }
    }
       
    (void) IOObjectRelease(intfIterator); // Release the iterator.
}

- (void)heartBeatResponse:(NSNotification*)notification {
    _beatCount ++;
    
    if (_beatCount % _notifyInterval)
        return;
    
    [self sendUserInfo];
    [self scanUpdateInfo];
}

- (void)sendUserInfo {
    if (!_isMACAddressFound || _isSendingUserInfo)
        return;
    
    _isSendingUserInfo = YES;
    
    NSString* MACAddressString = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x",
                                  _MACAddress[0], _MACAddress[1], _MACAddress[2],
                                  _MACAddress[3], _MACAddress[4], _MACAddress[5]];
    
    NSString* URLString = [NSString stringWithFormat:@"%@?version=%%22%@%%22&mac_address=%%22%@%%22",
                           @"https://cymbols.io/cgi-bin/cymbols_user_notify",
                           @"0.5",
                           MACAddressString];
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:URLString] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:20.0];
    [request setHTTPMethod:@"GET"];
    
    NSURLSession *session = [NSURLSession sharedSession];
    NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
        self->_isSendingUserInfo = NO;
        NSString* string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSLog(@"%@", string);
        
        if(httpResponse.statusCode == 200) {
        }
        else {
        }
    }];
    [dataTask resume];
}

- (void)scanUpdateInfo {
    if (_isAcquiringUpdateInfo)
        return;
    
    _isAcquiringUpdateInfo = YES;
    
    NSString* URLString = @"https://cymbols.io/cymbols_update_info";
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:URLString] cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:20.0];
    [request setHTTPMethod:@"GET"];
    
    NSURLSession *session = [NSURLSession sharedSession];
    NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
        self->_isAcquiringUpdateInfo = NO;
        
        NSString* update = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        
        if(httpResponse.statusCode == 200) {
            
            dispatch_sync(dispatch_get_main_queue(), ^{
                AppDelegate* delegate = [NSApp delegate];
                NSString* filePath = [[delegate supportDirectory] stringByAppendingPathComponent:@"UpdateInfo.cfg"];
                
                if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                    NSString* origin = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
                    
                    if ([self compareUpdateInfo:origin with:update]) {
                        [update writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
                        [self setUpdateFlag];
                    }
                }
                else {
                    [update writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
                    [self setUpdateFlag];
                }
                
            });
        }
        
    }];
    [dataTask resume];
}

- (BOOL)compareUpdateInfo:(NSString*)origin with:(NSString*)update {
    NSString* time0 = nil;
    NSString* time1 = nil;
    NSString* patternString = @"\\[(.*)\\]\\s*:\\s*.*";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = nil;
    NSTextCheckingResult *match = nil;
    
    matches = [regex matchesInString:origin options:0 range:NSMakeRange(0, [origin length])];
    if (matches.count) {
        match = matches[0];
        time0 = [origin substringWithRange:[match rangeAtIndex:1]];
    }
    
    matches = [regex matchesInString:update options:0 range:NSMakeRange(0, [update length])];
    if (matches.count) {
        match = matches[0];
        time1 = [update substringWithRange:[match rangeAtIndex:1]];
    }
    
    if (!time0 || !time1)
        return NO;
    
    time_t t0 = cee_time_from_iso8601([time0 UTF8String]);
    time_t t1 = cee_time_from_iso8601([time1 UTF8String]);
    
    if (cee_time_compare(t0, t1) == -1)
        return YES;
    
    return NO;
}

- (void)setUpdateFlag {
    AppDelegate* delegate = [NSApp delegate];
    NSString* filePath = [[delegate supportDirectory] stringByAppendingPathComponent:@"UpdateInfo.cfg"];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return;
    
    NSString* string = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
    string = [self appendUpdateFlagInString:string];
    [string writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationCymbolsUpdateNotify object:self];
}

- (void)cleanUpdateFlag {
    AppDelegate* delegate = [NSApp delegate];
    NSString* filePath = [[delegate supportDirectory] stringByAppendingPathComponent:@"UpdateInfo.cfg"];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return;
    
    NSString* string = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
    string = [self cleanUpdateFlagInString:string];
    [string writeToURL:[NSURL fileURLWithPath:filePath] atomically:NO encoding:NSUTF8StringEncoding error:nil];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:CEENotificationCymbolsUpdateConfirm object:self];
}

- (BOOL)updateFlagSet {
    AppDelegate* delegate = [NSApp delegate];
    NSString* filePath = [[delegate supportDirectory] stringByAppendingPathComponent:@"UpdateInfo.cfg"];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return NO;
    
    NSString* string = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
    return [self updateFlagSetInString:string];
}

- (NSString*)updateInfoString {
    AppDelegate* delegate = [NSApp delegate];
    NSString* filePath = [[delegate supportDirectory] stringByAppendingPathComponent:@"UpdateInfo.cfg"];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
        return nil;
    
    NSString* string = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil];
    NSString* patternString = @"\\[.*\\]\\s*:\\s*(.*)";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = nil;
    NSString* infoString = nil;
    matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    if (matches.count) {
        NSTextCheckingResult *match = matches[0]; // first line in  update info string
        NSRange range = [match rangeAtIndex:1];
        infoString = [string substringWithRange:range];
        infoString = [infoString stringByReplacingOccurrencesOfString:@"*" withString:@""];
    }
    return infoString;
}

- (NSString*)appendUpdateFlagInString:(NSString*)string {
    NSString* patternString = @"\\[.*\\]\\s*:\\s*.*";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = nil;
    
    matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    if (matches.count) {
        NSTextCheckingResult *match = matches[0]; // first line in  update info string
        NSRange range = [match rangeAtIndex:0];
        NSString* str = [string substringWithRange:range];
        str = [str stringByAppendingFormat:@"*", nil];
        string = [string stringByReplacingCharactersInRange:range withString:str];
    }
    
    return string;
}

- (NSString*)cleanUpdateFlagInString:(NSString*)string {
    return [string stringByReplacingOccurrencesOfString:@"*" withString:@""];
}

- (BOOL)updateFlagSetInString:(NSString*)string {
    NSString* patternString = @"\\[.*\\]\\s*:\\s*.*";
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = nil;
    
    matches = [regex matchesInString:string options:0 range:NSMakeRange(0, [string length])];
    if (matches.count) {
        NSTextCheckingResult *match = matches[0]; // first line in  update info string
        NSRange range = [match rangeAtIndex:0];
        NSString* str = [string substringWithRange:range];
        return [str containsString:@"*"];
    }
    
    return NO;
}

@end
