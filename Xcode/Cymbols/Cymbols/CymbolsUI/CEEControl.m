//
//  CEEControl.m
//  Cymbols
//
//  Created by yuqingcai on 2019/7/12.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#import "CEEControl.h"
#include<stdio.h>

#define MAXCOLOR 256
#define RED 2
#define GREEN 1
#define BLUE 0

/**********************************************************************
 C Implementation of Wu's Color Quantizer (v. 2)
 (see Graphics Gems vol. II, pp. 126-133)
 
 Author: Xiaolin Wu
 Dept. of Computer Science
 Univ. of Western Ontario
 London, Ontario N6A 5B7
 wu@csd.uwo.ca
 
 Algorithm: Greedy orthogonal bipartition of RGB space for variance
 minimization aided by inclusion-exclusion tricks.
 For speed no nearest neighbor search is done. Slightly
 better performance can be expected by more sophisticated
 but more expensive versions.
 
 The author thanks Tom Lane at Tom_Lane@G.GP.CS.CMU.EDU for much of
 additional documentation and a cure to a previous bug.
 
 Free to distribute, comments and suggestions are appreciated.
 **********************************************************************/

struct box {
    int r0;             /* min value, exclusive */
    int r1;             /* max value, inclusive */
    int g0;
    int g1;
    int b0;
    int b1;
    int vol;
};

/* Histogram is in elements 1..HISTSIZE along each axis,
 * element 0 is for base or marginal value
 * NB: these must start out 0!
 */

float m2[33][33][33];
long int wt[33][33][33];
long int mr[33][33][33];
long int mg[33][33][33];
long int mb[33][33][33];
unsigned char *Ir;
unsigned char *Ig;
unsigned char *Ib;
int image_size; /*image size*/
long int K; /*color look-up table size*/
unsigned char tag[33][33][33];

int Hist3d(long int *vwt,
           long int *vmr,
           long int *vmg,
           long int *vmb,
           float *m2)
/* build 3-D color histogram of counts, r/g/b, c^2 */
{
    int ind, r, g, b;
    int inr, ing, inb, table[256];
    int i;
    
    for (i = 0; i < 256; i ++) 
        table[i] = i*i;
        
    for(i = 0; i < image_size; i ++) {
        r = Ir[i];
        g = Ig[i];
        b = Ib[i];
        inr = (r >> 3) + 1;
        ing = (g >> 3) + 1;
        inb = (b >> 3) + 1;
        ind = (inr << 10) + (inr << 6) + inr + (ing << 5) + ing + inb;
        /*[inr][ing][inb]*/
        ++ vwt[ind];
        vmr[ind] += r;
        vmg[ind] += g;
        vmb[ind] += b;
        m2[ind] += (float)(table[r] + table[g] + table[b]);
    }
    
    return 0;
}

/* At conclusion of the histogram step, we can interpret
 *   wt[r][g][b] = sum over voxel of P(c)
 *   mr[r][g][b] = sum over voxel of r*P(c)  ,  similarly for mg, mb
 *   m2[r][g][b] = sum over voxel of c^2*P(c)
 * Actually each of these should be divided by 'size' to give the usual
 * interpretation of P() as ranging from 0 to 1, but we needn't do that here.
 */

/* We now convert histogram into moments so that we can rapidly calculate
 * the sums of the above quantities over any desired box.
 */
void M3d(long int *vwt,
         long int *vmr,
         long int *vmg,
         long int *vmb,
         float *m2) /* compute cumulative moments. */
{
    unsigned int ind1, ind2;
    unsigned char i, r, g, b;
    int line, line_r, line_g, line_b, 
        area[33], area_r[33], area_g[33],
        area_b[33];
    float line2, area2[33];
    
    for (r = 1; r <= 32; r ++) {
        for (i = 0; i <= 32; i ++)
            area2[i] = area[i] = area_r[i] = area_g[i] = area_b[i] = 0;
        
        for (g = 1; g <= 32; g ++) {
            line2 = line = line_r = line_g = line_b = 0;
            
            for (b = 1; b <= 32; b ++) {
                ind1 = (r << 10) + (r << 6) + r + (g << 5) + g + b; /* [r][g][b] */
                line += vwt[ind1];
                line_r += vmr[ind1];
                line_g += vmg[ind1];
                line_b += vmb[ind1];
                line2 += m2[ind1];
                area[b] += line;
                area_r[b] += line_r;
                area_g[b] += line_g;
                area_b[b] += line_b;
                area2[b] += line2;
                ind2 = ind1 - 1089; /* [r-1][g][b] */
                vwt[ind1] = vwt[ind2] + area[b];
                vmr[ind1] = vmr[ind2] + area_r[b];
                vmg[ind1] = vmg[ind2] + area_g[b];
                vmb[ind1] = vmb[ind2] + area_b[b];
                m2[ind1] = m2[ind2] + area2[b];
            }
            
        }
    }
}


long int Vol(struct box *cube,
             long int mmt[33][33][33])
/* Compute sum over a box of any given statistic */

{
    return(mmt[cube->r1][cube->g1][cube->b1]
           -mmt[cube->r1][cube->g1][cube->b0]
           -mmt[cube->r1][cube->g0][cube->b1]
           +mmt[cube->r1][cube->g0][cube->b0]
           -mmt[cube->r0][cube->g1][cube->b1]
           +mmt[cube->r0][cube->g1][cube->b0]
           +mmt[cube->r0][cube->g0][cube->b1]
           -mmt[cube->r0][cube->g0][cube->b0]);
}

/* The next two routines allow a slightly more efficient calculation
 * of Vol() for a proposed subbox of a given box.  The sum of Top()
 * and Bottom() is the Vol() of a subbox split in the given direction
 * and with the specified new upper bound.
 */

long int Bottom(struct box *cube,
                unsigned char dir,
                long int mmt[33][33][33])
/* Compute part of Vol(cube, mmt) that doesn't depend on r1, g1, or b1 */
/* (depending on dir) */

{
    switch(dir){
        case RED:
            return (-mmt[cube->r0][cube->g1][cube->b1]
                    +mmt[cube->r0][cube->g1][cube->b0]
                    +mmt[cube->r0][cube->g0][cube->b1]
                    -mmt[cube->r0][cube->g0][cube->b0]);
            break;
        case GREEN:
            return (-mmt[cube->r1][cube->g0][cube->b1]
                    +mmt[cube->r1][cube->g0][cube->b0]
                    +mmt[cube->r0][cube->g0][cube->b1]
                    -mmt[cube->r0][cube->g0][cube->b0]);
            break;
        case BLUE:
            return (-mmt[cube->r1][cube->g1][cube->b0]
                    +mmt[cube->r1][cube->g0][cube->b0]
                    +mmt[cube->r0][cube->g1][cube->b0]
                    -mmt[cube->r0][cube->g0][cube->b0]);
            break;
    }
    return 0;
}


long int Top(struct box *cube,
             unsigned char dir,
             int pos,
             long int mmt[33][33][33])
/* Compute remainder of Vol(cube, mmt), substituting pos for */
/* r1, g1, or b1 (depending on dir) */
{
    switch(dir){
        case RED:
            return (mmt[pos][cube->g1][cube->b1]
                    -mmt[pos][cube->g1][cube->b0]
                    -mmt[pos][cube->g0][cube->b1]
                    +mmt[pos][cube->g0][cube->b0]);
            break;
        case GREEN:
            return (mmt[cube->r1][pos][cube->b1]
                    -mmt[cube->r1][pos][cube->b0]
                    -mmt[cube->r0][pos][cube->b1]
                    +mmt[cube->r0][pos][cube->b0]);
            break;
        case BLUE:
            return (mmt[cube->r1][cube->g1][pos]
                    -mmt[cube->r1][cube->g0][pos]
                    -mmt[cube->r0][cube->g1][pos]
                    +mmt[cube->r0][cube->g0][pos]);
            break;
    }
    return 0;
}


float Var(struct box *cube)
/* Compute the weighted variance of a box */
/* NB: as with the raw statistics, this is really the variance * size */
{
    float dr, dg, db, xx;
    
    dr = Vol(cube, mr);
    dg = Vol(cube, mg);
    db = Vol(cube, mb);
    xx = m2[cube->r1][cube->g1][cube->b1]
        -m2[cube->r1][cube->g1][cube->b0]
        -m2[cube->r1][cube->g0][cube->b1]
        +m2[cube->r1][cube->g0][cube->b0]
        -m2[cube->r0][cube->g1][cube->b1]
        +m2[cube->r0][cube->g1][cube->b0]
        +m2[cube->r0][cube->g0][cube->b1]
        -m2[cube->r0][cube->g0][cube->b0];
    
    return (xx - ((dr * dr) + (dg * dg) + (db * db)) / (float)Vol(cube, wt));
}

/* We want to minimize the sum of the variances of two subboxes.
 * The sum(c^2) terms can be ignored since their sum over both subboxes
 * is the same (the sum for the whole box) no matter where we split.
 * The remaining terms have a minus sign in the variance formula,
 * so we drop the minus sign and MAXIMIZE the sum of the two terms.
 */
float Maximize(struct box *cube,
               unsigned char dir,
               int first,
               int last,
               int *cut,
               long int whole_r,
               long int whole_g,
               long int whole_b,
               long int whole_w)

{
    long int half_r, half_g, half_b, half_w;
    long int base_r, base_g, base_b, base_w;
    int i;
    float temp, max;
    
    base_r = Bottom(cube, dir, mr);
    base_g = Bottom(cube, dir, mg);
    base_b = Bottom(cube, dir, mb);
    base_w = Bottom(cube, dir, wt);
    max = 0.0;
    *cut = -1;
    for (i = first; i < last; i ++){
        half_r = base_r + Top(cube, dir, i, mr);
        half_g = base_g + Top(cube, dir, i, mg);
        half_b = base_b + Top(cube, dir, i, mb);
        half_w = base_w + Top(cube, dir, i, wt);
        /* now half_x is sum over lower half of box, if split at i */
        if (half_w == 0) {      /* subbox could be empty of pixels! */
            continue;             /* never split into an empty box */
        } else {
            temp = ((float)half_r*half_r + (float)half_g*half_g +
                    (float)half_b*half_b)/half_w;
        }
        
        half_r = whole_r - half_r;
        half_g = whole_g - half_g;
        half_b = whole_b - half_b;
        half_w = whole_w - half_w;
        if (half_w == 0) {      /* subbox could be empty of pixels! */
            continue;             /* never split into an empty box */
        } else {
            temp += ((float)half_r*half_r + (float)half_g*half_g +
                     (float)half_b*half_b)/half_w;
        }
        
        if (temp > max) {
            max=temp; 
            *cut=i;
        }
    }
    return(max);
}

int Cut(struct box *set1, 
        struct box *set2)
{
    unsigned char dir;
    int cutr, cutg, cutb;
    float maxr, maxg, maxb;
    long int whole_r, whole_g, whole_b, whole_w;
    
    whole_r = Vol(set1, mr);
    whole_g = Vol(set1, mg);
    whole_b = Vol(set1, mb);
    whole_w = Vol(set1, wt);
    
    maxr = Maximize(set1, RED, set1->r0+1, set1->r1, &cutr,
                    whole_r, whole_g, whole_b, whole_w);
    maxg = Maximize(set1, GREEN, set1->g0+1, set1->g1, &cutg,
                    whole_r, whole_g, whole_b, whole_w);
    maxb = Maximize(set1, BLUE, set1->b0+1, set1->b1, &cutb,
                    whole_r, whole_g, whole_b, whole_w);
    
    if( (maxr>=maxg)&&(maxr>=maxb) ) {
        dir = RED;
        if (cutr < 0) return 0; /* can't split the box */
    }
    else
        if( (maxg>=maxr)&&(maxg>=maxb) )
            dir = GREEN;
        else
            dir = BLUE;
    
    set2->r1 = set1->r1;
    set2->g1 = set1->g1;
    set2->b1 = set1->b1;
    
    switch (dir){
        case RED:
            set2->r0 = set1->r1 = cutr;
            set2->g0 = set1->g0;
            set2->b0 = set1->b0;
            break;
        case GREEN:
            set2->g0 = set1->g1 = cutg;
            set2->r0 = set1->r0;
            set2->b0 = set1->b0;
            break;
        case BLUE:
            set2->b0 = set1->b1 = cutb;
            set2->r0 = set1->r0;
            set2->g0 = set1->g0;
            break;
    }
    set1->vol=(set1->r1-set1->r0)*(set1->g1-set1->g0)*(set1->b1-set1->b0);
    set2->vol=(set2->r1-set2->r0)*(set2->g1-set2->g0)*(set2->b1-set2->b0);
    return 1;
}


void Mark(struct box *cube,
     int label,
     unsigned char *tag)
{
    int r, g, b;
    
    for (r = cube->r0 + 1; r <= cube->r1; r ++)
        for (g = cube->g0 + 1; g <= cube->g1; g ++)
            for (b = cube->b0 + 1; b <= cube->b1; b ++)
                tag[(r<<10) + (r<<6) + r + (g<<5) + g + b] = label;
}

@implementation CEEControl

- (void)initProperties {
    _styleState = kCEEViewStyleStateActived;
    [self setEnabled:YES];
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    self = [super initWithCoder:decoder];
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


- (instancetype)init {
    self = [super init];
    if (!self)
        return nil;
    
    [self initProperties];
    
    return self;
}

- (NSImage*)tintedImage:(NSImage*)image withColor:(NSColor *)tint {
    NSImage *tintedImage = [image copy];
    if (tint) {
        [tintedImage lockFocus];
        [tint set];
        NSRect imageRect = { NSZeroPoint, [tintedImage size] };
        NSRectFillUsingOperation(imageRect, NSCompositingOperationSourceAtop);
        [tintedImage unlockFocus];
    }
    return tintedImage;
}

- (NSArray*)schemeColorsFromImage:(NSImage*)image {
    if (!image)
        return nil;
    
    NSSize imageSize = image.size;
    NSRect imageRect = NSMakeRect(0, 0, imageSize.width, imageSize.height);

    // Create a context to hold the image data
    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    CGContextRef ctx = CGBitmapContextCreate(NULL,
                                             imageSize.width,
                                             imageSize.height,
                                             8,
                                             0,
                                             colorSpace,
                                             kCGImageAlphaPremultipliedLast);
    // Wrap graphics context
    NSGraphicsContext* gctx = [NSGraphicsContext graphicsContextWithCGContext:ctx flipped:NO];

    // Make our bitmap context current and render the NSImage into it
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:gctx];
    [image drawInRect:imageRect];
    
    size_t width = CGBitmapContextGetWidth(ctx);
    size_t height = CGBitmapContextGetHeight(ctx);
    uint32_t* pixel = (uint32_t*)CGBitmapContextGetData(ctx);
    
    struct box cube[MAXCOLOR];
    unsigned char lut_r[MAXCOLOR];
    unsigned char lut_g[MAXCOLOR];
    unsigned char lut_b[MAXCOLOR];
    int next;
    long int i;
    long int weight;
    int k;
    float vv[MAXCOLOR];
    float temp;
    
    image_size = (int)(width * height);
    Ir = (unsigned char*)malloc(sizeof(unsigned char)* width * height);
    Ig = (unsigned char*)malloc(sizeof(unsigned char)* width * height);
    Ib = (unsigned char*)malloc(sizeof(unsigned char)* width * height);
    
    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            uint32_t rgba = *pixel;
            
            // Extract colour components
            uint8_t red = (rgba & 0x000000ff) >> 0;
            uint8_t green = (rgba & 0x0000ff00) >> 8;
            uint8_t blue = (rgba & 0x00ff0000) >> 16;
            
            Ir[y*width + x] = red;
            Ig[y*width + x] = green;
            Ib[y*width + x] = blue;
            
            // Next pixel!
            pixel++;
        }
    }
    
    
    K = 2; // 2 colors in scheme
    memset(wt, 0, sizeof(long int) * (33*33*33));
    memset(mr, 0, sizeof(long int) * (33*33*33));
    memset(mg, 0, sizeof(long int) * (33*33*33));
    memset(mb, 0, sizeof(long int) * (33*33*33));
    memset(m2, 0, sizeof(float) * (33*33*33));
    
    Hist3d(wt, mr, mg, mb, m2);
    free(Ig); free(Ib); free(Ir);
    M3d(wt, mr, mg, mb, m2);
    
    cube[0].r0 = cube[0].g0 = cube[0].b0 = 0;
    cube[0].r1 = cube[0].g1 = cube[0].b1 = 32;
    next = 0;
    for(i = 1; i < K; i ++) {
        if (Cut(&cube[next], &cube[i])) {
            // volume test ensures we won't try to cut one-cell box
            vv[next] = (cube[next].vol>1) ? Var(&cube[next]) : 0.0;
            vv[i] = (cube[i].vol>1) ? Var(&cube[i]) : 0.0;
        } else {
            vv[next] = 0.0; // don't try to split this box again
            i--;            // didn't create box i
        }
        next = 0; temp = vv[0];
        for(k=1; k<=i; ++k)
            if (vv[k] > temp) {
                temp = vv[k]; next = k;
            }
        if (temp <= 0.0) {
            K = i + 1;
            break;
        }
    }
    
    // the space for array m2 can be freed now
    for(k = 0; k < K; k ++){
        Mark(&cube[k], k, tag);
        weight = Vol(&cube[k], wt);
        if (weight) {
            lut_r[k] = Vol(&cube[k], mr) / weight;
            lut_g[k] = Vol(&cube[k], mg) / weight;
            lut_b[k] = Vol(&cube[k], mb) / weight;
        }
        else{
            lut_r[k] = lut_g[k] = lut_b[k] = 0;
        }
    }    
    
    // Clean up
    CGColorSpaceRelease(colorSpace);
    [NSGraphicsContext setCurrentContext:nil];
    [NSGraphicsContext restoreGraphicsState];
    CGContextRelease(ctx);
    
    NSMutableArray* colors = [[NSMutableArray alloc] init];
    for (int i = 0; i < K; i ++) {
        NSColor* color = [NSColor colorWithCalibratedRed:lut_r[i] / 256.0 green:lut_g[i] / 256.0 blue:lut_b[i] / 256.0 alpha:1];
        CGFloat hue = [color hueComponent];
        CGFloat saturation = [color saturationComponent];
        CGFloat brightness = [color brightnessComponent];
        
        if (brightness > 0.6)
            brightness -= 0.05;
        else
            brightness += 0.05;
        
        color = [NSColor colorWithColorSpace:[NSColorSpace sRGBColorSpace] hue:hue saturation:saturation brightness:brightness alpha:1.0];
        [colors addObject:color];
    }    
    return colors;
}

- (void)setStyleState:(CEEViewStyleState)state {  
    _styleState = state;
    [super setStyleState:state];
}

- (CEEViewStyleState)styleState {
    return _styleState;
}

- (void)setStyleConfiguration:(CEEUserInterfaceStyleConfiguration*)configuration {
    _styleConfiguration = configuration;
    [configuration configureView:self];
    [super setStyleConfiguration:configuration];
}

- (CEEUserInterfaceStyleConfiguration*)styleConfiguration {
    return _styleConfiguration;
}

- (void)updateUserInterface {
    
}

- (void)setSytleSchemes:(NSArray*)schemes {
    self.userInterfaceStyles = [[NSPointerArray alloc] init];
        
    for (NSUInteger i = 0; i < kCEEViewStyleStateMax; i ++)
        [self.userInterfaceStyles addPointer:NULL];
    
    for (CEEUserInterfaceStyleScheme* scheme in schemes) {
        CEEUserInterfaceStyle* style = [[CEEUserInterfaceStyle alloc] initWithScheme:scheme];
        [self.userInterfaceStyles replacePointerAtIndex:scheme.styleState withPointer:(void*)style];
    }
}

@end
