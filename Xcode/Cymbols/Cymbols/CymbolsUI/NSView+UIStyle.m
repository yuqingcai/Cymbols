//
//  NSView+UIStyle.m
//  Cymbols
//
//  Created by caiyuqing on 2019/9/26.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "NSView+UIStyle.h"
#include "CEEIdentifier.h"

@implementation CEEUserInterfaceStyleScheme
@end

@implementation NSView(UIStyle)

- (BOOL)styleSet:(CEEViewStyle)style {
    return NO;
}

- (CEEViewStyle)style {
    return kCEEViewStyleInit;
}

- (void)setStyle:(CEEViewStyle)style {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view setStyle:style];
}

- (void)clearStyle:(CEEViewStyle)style {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view clearStyle:style];
}

- (void)resetStyle:(CEEViewStyle)style {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view resetStyle:style];
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    [self updateUserInterface];
    [self setNeedsDisplay:YES];
    for (NSView* view in self.subviews)
        [view setStyleConfiguration:configuration];
}


- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return nil;
}

- (NSString*)getSchemeIdentifier {
    NSString* identifier = nil;
    
    // try to get the view identifier
    if ([self conformsToProtocol:@protocol(NSUserInterfaceItemIdentification)]) {
        identifier = [(id<NSUserInterfaceItemIdentification>)self identifier];
        if (identifier) {
            // it's a reserved Identifier, ignore
            if ([[identifier substringWithRange:NSMakeRange(0, 1)] isEqualToString:@"_"])
                identifier = nil;
            else
                identifier = [identifier stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
        }
    }
    return identifier;
}


- (NSString*)getSchemeClassName {
    NSString* className = nil;
    className = [self className];
    if ([[className substringWithRange:NSMakeRange(0, 3)] isEqualToString:@"CEE"])
        className = [className stringByReplacingCharactersInRange:NSMakeRange(0, 3) withString:@"ID"];
    return className;
}

- (NSString*)createComponentIdentifier:(NSString*)identifier {
    NSString* componentIdentifier = nil;
    NSString* mainIdentifier = nil;
    if (!self.identifier || IdentifierIsReserved(self.identifier))
        mainIdentifier = CreateClassIdentifier([self className]);
    else
        mainIdentifier = self.identifier;
    
    componentIdentifier = [mainIdentifier stringByAppendingFormat:@".%@", identifier];
    componentIdentifier = CreateUnifiedIdentifierWithPrefix(componentIdentifier);
    
    return componentIdentifier;
}

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    
}

@end
