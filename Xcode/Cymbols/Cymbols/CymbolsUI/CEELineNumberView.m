//
//  CEELineNumberView.m
//  Cymbols
//
//  Created by caiyuqing on 2018/12/3.
//  Copyright © 2018 caiyuqing. All rights reserved.
//

#import "CEELineNumberView.h"
#import <cjson/cJSON.h>
#import "cee_regex.h"


@interface CEELineNumberView()
@end

@implementation CEELineNumberView

@synthesize lineNumberTags = _lineNumberTags;

- (void)initProperties {
    [super initProperties];
}

- (BOOL)isFlipped {
    return NO;
}

- (void)setLineNumberTags:(NSArray *)lineNumberTags {
    _lineNumberTags = lineNumberTags;
    [self setNeedsDisplay:YES];
}

- (NSArray*)lineNumberTags {
    return _lineNumberTags;
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return nil;
}

- (void)updateUserInterface {
}

- (void)setTextAttributesDescriptor:(NSString*)descriptor {
    cJSON *descriptor_object = cJSON_Parse((const char*)[descriptor UTF8String]);
    if (!descriptor_object) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stderr, "is_valid_text_platform_descriptor Error before: %s\n", error_ptr);;
    }
    
    const cJSON *background_color_item = cJSON_GetObjectItemCaseSensitive(descriptor_object, "backgroundcolor");
    if (background_color_item)
        self.backgroundColor = [CEEUserInterfaceStyleConfiguration createColorFromString:[NSString stringWithUTF8String:background_color_item->valuestring]];
        
    const cJSON *rules = cJSON_GetObjectItemCaseSensitive(descriptor_object, "rules");
    if (rules) {
        const cJSON *rule = NULL;
        
        cJSON_ArrayForEach(rule, rules) {
            cJSON* name_item = cJSON_GetObjectItemCaseSensitive(rule, "name");
            if (!strcmp(name_item->valuestring, "line_number")) {
                cJSON* font_item = cJSON_GetObjectItemCaseSensitive(rule, "font");
                cJSON* forecolor_item = cJSON_GetObjectItemCaseSensitive(rule, "forecolor");
                
                self.font = [CEEUserInterfaceStyleConfiguration createFontFromString:[NSString stringWithUTF8String:font_item->valuestring]];
                self.textColor = [CEEUserInterfaceStyleConfiguration createColorFromString:[NSString stringWithUTF8String:forecolor_item->valuestring]];
                break;
            }
        }
    }
    
    cJSON_Delete(descriptor_object);
    
    [self setNeedsDisplay:YES];
}


- (void)drawRect:(NSRect)dirtyRect {
    
    NSSize frameSize = self.frame.size;
    NSRect contentRect = NSMakeRect(0.0,
                                    0.0,
                                    frameSize.width,
                                    frameSize.height);
    
    NSBezierPath* contentPath = [NSBezierPath bezierPathWithRoundedRect:contentRect xRadius:self.cornerRadius yRadius:self.cornerRadius];
    
    if (self.backgroundColor) {
        [self.backgroundColor setFill];
        [contentPath fill];
    }
    
    NSDictionary* attribtues = @{
                                 NSFontAttributeName : self.font,
                                 NSForegroundColorAttributeName : self.textColor,
                                 };
    if (!_lineNumberTags)
        return;
    
    for (NSDictionary* tag in _lineNumberTags) {
        NSString* label = tag[@"number"];
        NSRect bounds = NSRectFromString(tag[@"bounds"]);
        NSAttributedString* attributedLabel = [[NSAttributedString alloc] initWithString:label attributes: attribtues];
        
        NSRect rect = NSMakeRect((self.frame.size.width - [attributedLabel size].width),
                                 self.frame.size.height - (bounds.origin.y + bounds.size.height),
                                 self.frame.size.width,
                                 bounds.size.height);
        
        //[[NSColor redColor] setStroke];
        //NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:1.0 yRadius:1.0];
        //[path setLineWidth:1.0];
        //[path stroke];
        
        [attributedLabel drawAtPoint:rect.origin];
    }
}

@end
