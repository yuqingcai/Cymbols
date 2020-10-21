//
//  CEENetwork.m
//  Cymbols
//
//  Created by qing on 2020/9/16.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#import "CEENetwork.h"
#import "AppDelegate.h"
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOEthernetController.h>


static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices);
static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize);

// Returns an iterator containing the primary (built-in) Ethernet interface. The caller is responsible for
// releasing the iterator after the caller is done with it.
static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
{
    kern_return_t           kernResult; 
    CFMutableDictionaryRef    matchingDict;
    CFMutableDictionaryRef    propertyMatchDict;
    
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
    io_object_t     intfService;
    io_object_t     controllerService;
    kern_return_t   kernResult = KERN_FAILURE;
    
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


@interface CEENetwork() {
    UInt8 _MACAddress[kIOEthernetAddressSize];
}

@property NSInteger beatCount;
@property BOOL isMACAddressFound;
@property BOOL isRequesting;
@property NSInteger notifyInterval;
@end

@implementation CEENetwork

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self getMACAddress];
    _isRequesting = NO;
    _notifyInterval = (NSInteger)(60.0 / CEE_APP_HEART_BEAT_INTERVAL); //60s
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
    if (_beatCount % _notifyInterval || !_isMACAddressFound || _isRequesting)
        return;
    
    _isRequesting = YES;
    
    NSString* MACAddressString = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x", 
                                  _MACAddress[0], _MACAddress[1], _MACAddress[2], _MACAddress[3], _MACAddress[4], _MACAddress[5]];
    
    NSString* URLString = [NSString stringWithFormat:@"%@?version=%%22%@%%22&mac_address=%%22%@%%22",
                           @"https://cymbols.io/cgi-bin/cymbols_user_notify",
                           @"0.5",
                           MACAddressString];
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:URLString] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:20.0];
    [request setHTTPMethod:@"GET"];
    
    NSURLSession *session = [NSURLSession sharedSession];
    NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
        self->_isRequesting = NO;
        NSString* string = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSLog(@"%@", string);
        if(httpResponse.statusCode == 200) {
        }
        else {
        }
    }];
    [dataTask resume];
    
}

@end
