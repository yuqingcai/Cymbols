//
//  CEEScroller.m
//
//  Created by caiyuqing on 2018/12/3.
//  Copyright © 2018 caiyuqing. All rights reserved.
//

#import "CEEScroller.h"


@interface CEEScrollerStyleItem : NSObject
@property (strong) NSColor* knobColor;
@property (strong) NSColor* backgroundColor;
@end

@implementation CEEScrollerStyleItem
@end


@interface CEEScroller()
@property CGFloat currentValue;
@property (readonly) CGFloat knobSize;
@property NSPoint scrollerdragPoint;
@property BOOL shouldValidateScrollerDragPoint;
@end

@implementation CEEScroller

@synthesize knobProportion = _knobProportion;
@synthesize knobSize = _knobSize;

- (void)initProperties {
    _knobColor = [NSColor grayColor];
    _backgroundColor = [NSColor clearColor];
    _knobProportion = 1.0;
    _currentValue = 0.0;
    _knobSize = 0.0;
    _scrollerdragPoint = NSMakePoint(0.0, 0.0);
    _shouldValidateScrollerDragPoint = FALSE;
    _borderWidth = 0.0;
    _cornerRadius = 0.0;
}

- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (BOOL)isFlipped {
    return YES;
}

+ (CGFloat)scrollerWidthForControlSize:(NSControlSize)controlSize scrollerStyle:(NSScrollerStyle)scrollerStyle {
    return [NSScroller scrollerWidthForControlSize:controlSize scrollerStyle:scrollerStyle];
}

- (void)drawRect:(NSRect)dirtyRect {
    [_backgroundColor set];
        
    NSSize frameSize = self.frame.size;
    
    NSRect rect = NSMakeRect((_borderWidth / 2.0),
                             (_borderWidth / 2.0),
                             frameSize.width - _borderWidth,
                             frameSize.height - _borderWidth);
    NSBezierPath* path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:_cornerRadius yRadius:_cornerRadius];

    if (_backgroundColor) {
        [_backgroundColor setFill];
        [path fill];
    }
    [self drawKnob];
}

- (NSRect)rectForPart:(NSScrollerPart)partCode {
    if (partCode == NSScrollerKnob) {
        NSRect knobRect;
        if (_direction == kCEEScrollerDirectionVertical) {
            knobRect.origin.x = 0.0;
            knobRect.origin.y = _currentValue * (self.frame.size.height - self.knobSize);
            knobRect.size.width = self.frame.size.width;
            knobRect.size.height = self.knobSize;
        }
        else {
            knobRect.origin.x = _currentValue * (self.frame.size.width - self.knobSize);
            knobRect.origin.y = 0.0;
            knobRect.size.width = self.knobSize;
            knobRect.size.height = self.frame.size.height;
        }
        return  knobRect;
    }
    
    return NSMakeRect(0.0, 0.0, 0.0, 0.0);
}

- (void)setKnobProportion:(CGFloat)knobProportion {
    if (knobProportion > 1.0)
        knobProportion = 1.0;
    
    _knobProportion = knobProportion;
    
    [self setNeedsDisplay:YES];
}

- (CGFloat)knobProportion {
    return _knobProportion;
}

- (CGFloat)knobSize {
    CGFloat total = 0.0;
    if (_direction == kCEEScrollerDirectionVertical)
        total = self.frame.size.height;
    else
        total = self.frame.size.width;
    
    if (total * _knobProportion < 20.0)
        return 20.0;
    
    return total * _knobProportion;
}

- (void)drawKnob {
    if (fabs(1.0 - self.knobProportion) < 0.0001)
        return;

    NSRect knobRect = [self rectForPart:NSScrollerKnob];
    knobRect.origin.x += 4.0;
    knobRect.origin.y += 4.0;
    knobRect.size.height -= 8.0;
    knobRect.size.width -= 8.0;
    NSBezierPath* roundRect = [NSBezierPath bezierPathWithRoundedRect:knobRect xRadius:4.0 yRadius:4.0];
    [_knobColor setFill];
    [roundRect fill];
}

- (void)setFloatValue:(float)floatValue {
    _currentValue = floatValue;
    
    if (_currentValue < 0.0)
        _currentValue = 0.0;
    
    if (_currentValue > 1.0)
        _currentValue = 1.0;
    
    [self setNeedsDisplay:YES];
}

- (float)floatValue {
    return _currentValue;
}

- (void)valueByPoint:(NSPoint)point {
    NSRect knobRect = [self rectForPart:NSScrollerKnob];
    CGFloat knobHalfSize = self.knobSize / 2.0;
    if (_direction == kCEEScrollerDirectionVertical) {
        if (point.y < knobRect.origin.y) {
            if (point.y - knobHalfSize < 0.0)
                knobRect.origin.y = 0.0;
            else
                knobRect.origin.y = point.y - knobHalfSize;
        }
        else if (point.y > knobRect.origin.y + knobRect.size.height) {
            if (point.y + knobHalfSize > self.frame.size.height)
                knobRect.origin.y = self.frame.size.height - knobRect.size.height;
            else
                knobRect.origin.y = point.y - knobHalfSize;
        }
        _currentValue = knobRect.origin.y / 
            (self.frame.size.height - self.knobSize);
    }
    else if (_direction == kCEEScrollerDirectionHorizontal) {
        if (point.x < knobRect.origin.x) {
            if (point.x - knobHalfSize < 0.0)
                knobRect.origin.x = 0.0;
            else
                knobRect.origin.x = point.x - knobHalfSize;
        }
        else if (point.x > knobRect.origin.x + knobRect.size.width) {
            if (point.x + knobHalfSize > self.frame.size.width)
                knobRect.origin.x = self.frame.size.width - knobRect.size.width;
            else
                knobRect.origin.x = point.x - knobHalfSize;
        }
        _currentValue = knobRect.origin.x / 
            (self.frame.size.width - self.knobSize);
    }
}

- (void)valueByDiff:(NSPoint)point {
    NSRect knobRect = [self rectForPart:NSScrollerKnob];
    if (_direction == kCEEScrollerDirectionVertical) {
        knobRect.origin.y += point.y;
        if (knobRect.origin.y < 0.0) {
            knobRect.origin.y = 0.0;
            _shouldValidateScrollerDragPoint = YES;
        }
        
        if (knobRect.origin.y + knobRect.size.height > self.frame.size.height) {
            knobRect.origin.y = self.frame.size.height - knobRect.size.height;
            _shouldValidateScrollerDragPoint = YES;
        }
        
        _currentValue = knobRect.origin.y / (self.frame.size.height - self.knobSize);
    }
    else if (_direction == kCEEScrollerDirectionHorizontal){
        
        knobRect.origin.x += point.x;
        
        if (knobRect.origin.x < 0.0) {
            knobRect.origin.x = 0.0;
            _shouldValidateScrollerDragPoint = YES;
        }
        
        if (knobRect.origin.x + knobRect.size.width > self.frame.size.width) {
            knobRect.origin.x = self.frame.size.width - knobRect.size.width;
            _shouldValidateScrollerDragPoint = YES;
        }
        
        _currentValue = knobRect.origin.x / (self.frame.size.width - self.knobSize);
    }
}

- (BOOL)scrollable {
    if (fabs(_knobProportion - 1.0) < 0.0001)
        return NO;
    
    NSRect knobRect = [self rectForPart:NSScrollerKnob];
    
    if (_direction == kCEEScrollerDirectionVertical) {
        if (knobRect.origin.y < 0.0 ||
            knobRect.origin.y + knobRect.size.height > self.frame.size.height)
            return NO;
    }
    else if (_direction == kCEEScrollerDirectionHorizontal) {
        if (knobRect.origin.x < 0.0 ||
            knobRect.origin.x + knobRect.size.width > self.frame.size.width)
            return NO;
    }
    
    return YES;
}

- (void)logScrollerDragPoint:(NSPoint)point {
    NSRect knobRect = [self rectForPart:NSScrollerKnob];
    _scrollerdragPoint = NSMakePoint(point.x - knobRect.origin.x,
                                     point.y - knobRect.origin.y);
}

- (void)traceScrollerDragPoint:(NSPoint)point {
    NSRect knobRect = [self rectForPart:NSScrollerKnob];
    NSPoint pointInScroller = NSMakePoint(point.x - knobRect.origin.x,
                                          point.y - knobRect.origin.y);
    if (_direction == kCEEScrollerDirectionVertical) {
        if (fabs(pointInScroller.y - _scrollerdragPoint.y) < 10.0)
            _shouldValidateScrollerDragPoint = FALSE;
        
    }
    else if (_direction == kCEEScrollerDirectionHorizontal) {
        if (fabs(pointInScroller.x - _scrollerdragPoint.x) < 10.0)
            _shouldValidateScrollerDragPoint = FALSE;
    }
}

- (void)mouseDown:(NSEvent *)event {
    NSPoint point0;
    NSPoint point1;
    NSPoint diff;
    
    if (![self scrollable])
        return;
    
    point0 = [self convertPoint:[event locationInWindow] fromView:nil];
    [self valueByPoint:point0];
    [self logScrollerDragPoint:point0];
    [self setNeedsDisplay:YES];
    
    if (self.action)
        [self sendAction:self.action to:self.target];
    
    BOOL keepOn = YES;
    while (keepOn) {
        NSEventMask eventMask = NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged;
        event = [[self window] nextEventMatchingMask:eventMask];
        point1 = [self convertPoint:[event locationInWindow] fromView:nil];
        diff = NSMakePoint(point1.x - point0.x, point1.y - point0.y);
        point0 = point1;
        
        switch ([event type]) {
            case NSEventTypeLeftMouseDragged:
                
                if (_shouldValidateScrollerDragPoint) {
                    [self traceScrollerDragPoint:point1];
                    continue;
                }
                
                [self valueByDiff:diff];
                [self setNeedsDisplay:YES];
                
                if (self.action)
                    [self sendAction:self.action to:self.target];
            
                break;

            case NSEventTypeLeftMouseUp:
                keepOn = NO;
                break;

            default:
                break;
        }
    }
}

- (void)resetCursorRects {
    [super resetCursorRects];
    NSRect rect = NSMakeRect(0.0, 
                             0.0, 
                             self.bounds.size.width, 
                             self.bounds.size.height);
    [self addCursorRect:rect cursor:[NSCursor arrowCursor]];
}

- (void)updateUserInterface {
    CEEUserInterfaceStyle* current = (CEEUserInterfaceStyle*)[self.userInterfaceStyles pointerAtIndex:self.styleState];
    if (!current)
        return;
        
    if (current.knobColor)
        self.knobColor = current.knobColor;
    
    if (current.backgroundColor)
        self.backgroundColor = current.backgroundColor;
}

@end
