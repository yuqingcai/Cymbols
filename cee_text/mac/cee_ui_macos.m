//
//  cee_ui_macos.c
//  Cymbols
//
//  Created by qing on 2019/11/21.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <cjson/cJSON.h>
#include "cee_lib.h"
#include "cee_regex.h"
#include "cee_ui_macos.h"

static void hsb2rgb(cee_float hue,
                    cee_float saturation,
                    cee_float brightness,
                    cee_float *red,
                    cee_float *green,
                    cee_float *blue);


cee_boolean cee_color_component_from_rgb_descriptor(const char* descriptor,
                                                    CGFloat components[4])
{
    const cee_char* pattern = "(rgba)\\s*\\(\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*\\)";
    NSString* contentString = [NSString stringWithUTF8String:descriptor];
    NSString* patternString = [NSString stringWithUTF8String:pattern];
    cee_boolean ret = FALSE;
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:contentString options:0 range:NSMakeRange(0, [contentString length])];
    if (matches.count) {
        ret = TRUE;
        NSTextCheckingResult* match = matches[0];
        
        cee_char* type = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:1]] UTF8String]);
        cee_char* str0 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:2]] UTF8String]);
        cee_char* str1 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:3]] UTF8String]);
        cee_char* str2 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:4]] UTF8String]);
        cee_char* str3 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:5]] UTF8String]);
        
        components[0] = atof(str0) / 256.0;
        components[1] = atof(str1) / 256.0;
        components[2] = atof(str2) / 256.0;
        components[3] = atof(str3) / 100.0;
        
        cee_free(str3);
        cee_free(str2);
        cee_free(str1);
        cee_free(str0);
        cee_free(type);
    }
    
    return ret;
}

cee_boolean cee_color_component_from_hsb_descriptor(const char* descriptor,
                                                    CGFloat components[4])
{
    const cee_char* pattern = "(hsba)\\s*\\(\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*,\\s*([0-9.]+)\\s*\\)";
    NSString* contentString = [NSString stringWithUTF8String:descriptor];
    NSString* patternString = [NSString stringWithUTF8String:pattern];
    cee_boolean ret = FALSE;
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:contentString options:0 range:NSMakeRange(0, [contentString length])];
    if (matches.count) {
        ret = TRUE;
        NSTextCheckingResult* match = matches[0];

        cee_char* type = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:1]] UTF8String]);
        cee_char* str0 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:2]] UTF8String]);
        cee_char* str1 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:3]] UTF8String]);
        cee_char* str2 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:4]] UTF8String]);
        cee_char* str3 = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:5]] UTF8String]);
        
        float component0 = atof(str0);
        float component1 = atof(str1);
        float component2 = atof(str2);
        float component3 = atof(str3);

        cee_float red = 0.0;
        cee_float green = 0.0;
        cee_float blue = 0.0;
        cee_float alpha = 0.0;
        
        /**
         *  the platform color space is SRGB, we need to convert
         * HSB components to RGB components
         */
        hsb2rgb(component0, component1, component2, &red, &green, &blue);
        
        alpha = component3 / 100.0;
        components[0] = red;
        components[1] = green;
        components[2] = blue;
        components[3] = alpha;
        
        cee_free(str3);
        cee_free(str2);
        cee_free(str1);
        cee_free(str0);
        cee_free(type);
    }
    
    return ret;
}


cee_boolean cee_color_component_from_hex(const char* descriptor,
                                         CGFloat components[4])
{
    const cee_char* pattern = "#([0-9a-fA-F]+)";
    NSString* contentString = [NSString stringWithUTF8String:descriptor];
    NSString* patternString = [NSString stringWithUTF8String:pattern];
    cee_boolean ret = FALSE;
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:contentString options:0 range:NSMakeRange(0, [contentString length])];
    if (matches.count) {
        ret = TRUE;
        NSTextCheckingResult* match = matches[0];
        NSString* valueString = [contentString substringWithRange:[match rangeAtIndex:1]];
        long int hexValue = strtol([valueString UTF8String], NULL, 16);
        components[0] = (CGFloat)(((unsigned char)(hexValue >> 16)) / 256.0);
        components[1] = (CGFloat)(((unsigned char)(hexValue >> 8)) / 256.0);
        components[2] = (CGFloat)(((unsigned char)(hexValue)) / 256.0);
        components[3] = 1.0;
    }
    
    return ret;
}

static void hsb2rgb(cee_float hue,
                    cee_float saturation,
                    cee_float brightness,
                    cee_float *red,
                    cee_float *green,
                    cee_float *blue)
{
    int i;
    float f, p, q, t;
    
    saturation /= 100.0;
    brightness /= 100.0;
        
    if(saturation < FLT_EPSILON) {
        // achromatic (grey)
        *red = *green = *blue = brightness;
        return;
    }
    
    hue /= 60; // sector 0 to 5
    i = (int)floorf(hue);
    f = hue - i; // factorial part of h
    p = brightness * (1 - saturation);
    q = brightness * (1 - (saturation * f));
    t = brightness * (1 - (saturation * (1 - f)));
    
    switch (i) {
        case 0:
            *red = brightness;
            *green = t;
            *blue = p;
            break;
            
        case 1:
            *red = q;
            *green = brightness;
            *blue = p;
            break;
            
        case 2:
            *red = p;
            *green = brightness;
            *blue = t;
            break;
            
        case 3:
            *red = p;
            *green = q;
            *blue = brightness;
            break;
            
        case 4:
            *red = t;
            *green = p;
            *blue = brightness;
            break;
            
        default: // case 5:
            *red = brightness;
            *green = p;
            *blue = q;
            break;
    }
    return ;
}

void cee_font_attributes_from_descriptor(const cee_char* descriptor,
                                         cee_char** family,
                                         cee_char** style,
                                         cee_char** size)
{
    const cee_char* pattern = "font\\s*\\(\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*,\\s*\\\"(.*)\\\"\\s*,\\s*([0-9.]+)\\s*\\)";
    NSString* contentString = [NSString stringWithUTF8String:descriptor];
    NSString* patternString = [NSString stringWithUTF8String:pattern];
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:patternString options:NSRegularExpressionCaseInsensitive error:NULL];
    NSArray *matches = [regex matchesInString:contentString options:0 range:NSMakeRange(0, [contentString length])];
    if (matches.count) {
        NSTextCheckingResult* match = matches[0];
        if (family)
            *family = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:1]] UTF8String]);
        if (size)
            *size = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:2]] UTF8String]);
        if (style)
            *style = cee_strdup([[contentString substringWithRange:[match rangeAtIndex:3]] UTF8String]);
    }
}
