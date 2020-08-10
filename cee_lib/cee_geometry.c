#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <float.h>


#include "cee_types.h"
#include "cee_geometry.h"
#include "cee_mem.h"

CEEPoint cee_point_make(cee_float x,
                        cee_float y)
{
    CEEPoint point;
    point.x = x;
    point.y = y;
    return point;
}

CEEPoint* cee_point_create(cee_float x,
                           cee_float y)
{
    CEEPoint* point = (CEEPoint*)cee_malloc(sizeof(CEEPoint));
    point->x = x;
    point->y = y;
    return point;
}

void cee_point_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_free(data);
}

CEESize cee_size_make(cee_float width,
                      cee_float height)
{
    CEESize size;
    size.width = width;
    size.height = height;
    return size;
}

CEESize* cee_size_create(cee_float width,
                         cee_float height)
{
    CEESize* size = (CEESize*)cee_malloc(sizeof(CEESize));
    size->width = width;
    size->height = height;
    return size;
}
void cee_size_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_free(data);
}

CEERect cee_rect_make(cee_float x,
                      cee_float y,
                      cee_float width,
                      cee_float height)
{
    CEERect rect;
    rect.origin.x = x;
    rect.origin.y = y;
    rect.size.width = width;
    rect.size.height = height;
    return rect;
}

CEERect* cee_rect_create(cee_float x,
                         cee_float y,
                         cee_float width,
                         cee_float height)
{
    CEERect* rect = (CEERect*)cee_malloc(sizeof(CEERect));
    rect->origin.x = x;
    rect->origin.y = y;
    rect->size.width = width;
    rect->size.height = height;
    return rect;
}

void cee_rect_free(cee_pointer data)
{
    if (!data)
        return;
    
    cee_free(data);
}

cee_boolean cee_point_in_rect(CEEPoint point,
                              CEERect rect)
{
    return (point.x >= rect.origin.x &&
            point.x < rect.origin.x + rect.size.width &&
            point.y >= rect.origin.y &&
            point.y < rect.origin.y + rect.size.height);
}

static cee_int rect_tail_compare(CEERect* r0, CEERect* r1)
{
    cee_float x0 = r0->origin.x + r0->size.width;
    cee_float x1 = r1->origin.x + r1->size.width;
    
    if (fabs(x0 - x1) < FLT_EPSILON)
        return 0;
    if (x0 < x1)
        return -1;
    if (x0 > x1)
        return 1;
    
    return 0;
}

static cee_int rect_origin_compare(CEERect* r0, CEERect* r1)
{
    cee_float x0 = r0->origin.x;
    cee_float x1 = r1->origin.x;
    
    if (fabs(x0 - x1) < FLT_EPSILON)
        return 0;
    if (x0 < x1)
        return -1;
    if (x0 > x1)
        return 1;
    
    return 0;
}

static cee_float rect_tail_diff(CEERect* r0, CEERect* r1)
{
    cee_float x0 = r0->origin.x + r0->size.width;
    cee_float x1 = r1->origin.x + r1->size.width;
    return fabs(x0 - x1);
}

static cee_float rect_origin_diff(CEERect* r0, CEERect* r1)
{
    cee_float x0 = r0->origin.x;
    cee_float x1 = r1->origin.x;
    return fabs(x0 - x1);
}

CEEList* cee_rects_outline_create(CEEList* rects)
{
    CEEList* points = NULL;
    cee_float default_radius = 5.0;
    cee_float radius = 0.0;
    cee_float diff = 0.0;
    CEEPoint* p1 = NULL;
    CEEPoint* p2 = NULL;
    CEEPoint* c1 = NULL;
    CEEPoint* c2 = NULL;
    
    CEEList* p = rects;
    while (p) {
        CEERect* prev = NULL;
        CEERect* current = NULL;
        CEERect* next = NULL;
        
        if (p->prev)
            prev = p->prev->data;
        
        current = p->data;
        
        if (p->next)
            next = p->next->data;
        
        radius = default_radius;
        if (prev) {
            diff = rect_tail_diff(current, prev);
            radius =  diff >= default_radius ? default_radius : diff;
        }
        
        if (!prev || rect_tail_compare(current, prev) == 1) {
            p1 = cee_point_create(current->origin.x + current->size.width - radius, current->origin.y);
            p2 = cee_point_create(current->origin.x + current->size.width, current->origin.y + radius);
            c1 = cee_point_create(current->origin.x + current->size.width, current->origin.y);
            c2 = cee_point_create(current->origin.x + current->size.width, current->origin.y);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (prev && rect_tail_compare(current, prev) == 0) {
            p1 = cee_point_create(current->origin.x + current->size.width, current->origin.y);
            p2 = cee_point_create(current->origin.x + current->size.width, current->origin.y);
            c1 = NULL;
            c2 = NULL;
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (prev && rect_tail_compare(current, prev) == -1) {
            p1 = cee_point_create(current->origin.x + current->size.width + radius, current->origin.y);
            p2 = cee_point_create(current->origin.x + current->size.width, current->origin.y + radius);
            c1 = cee_point_create(current->origin.x + current->size.width, current->origin.y);
            c2 = cee_point_create(current->origin.x + current->size.width, current->origin.y);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        
        
        radius = default_radius;
        if (next) {
            diff = rect_tail_diff(current, next);
            radius =  diff >= default_radius ? default_radius : diff;
        }
        if (!next || rect_tail_compare(current, next) == 1) {
            p1 = cee_point_create(current->origin.x + current->size.width,
                                  current->origin.y + current->size.height - radius);
            p2 = cee_point_create(current->origin.x + current->size.width - radius, 
                                  current->origin.y + current->size.height);
            c1 = cee_point_create(current->origin.x + current->size.width, 
                                  current->origin.y + current->size.height);
            c2 = cee_point_create(current->origin.x + current->size.width, 
                                  current->origin.y + current->size.height);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (next && rect_tail_compare(current, next) == 0) {
            p1 = cee_point_create(current->origin.x + current->size.width, 
                                  current->origin.y + current->size.height);
            p2 = cee_point_create(current->origin.x + current->size.width, 
                                  current->origin.y + current->size.height);
            c1 = NULL;
            c2 = NULL;
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (next && rect_tail_compare(current, next) == -1) {
            p1 = cee_point_create(current->origin.x + current->size.width,
                                  current->origin.y + current->size.height - radius);
            p2 = cee_point_create(current->origin.x + current->size.width + radius, 
                                  current->origin.y + current->size.height);
            c1 = cee_point_create(current->origin.x + current->size.width, 
                                  current->origin.y + current->size.height);
            c2 = cee_point_create(current->origin.x + current->size.width, 
                                  current->origin.y + current->size.height);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        
        p = p->next;
    }
    
    p = cee_list_last(rects);
    while (p) {
        CEERect* prev = NULL;
        CEERect* current = NULL;
        CEERect* next = NULL;
        
        if (p->prev)
            prev = p->prev->data;
        
        current = p->data;
        
        if (p->next)
            next = p->next->data;
        
        
        
        radius = default_radius;
        if (next) {
            diff = rect_origin_diff(current, next);
            radius = diff >= default_radius ? default_radius : diff;
        }
            
        if (!next || rect_origin_compare(current, next) == -1) {
            p1 = cee_point_create(current->origin.x + radius, current->origin.y + current->size.height);
            p2 = cee_point_create(current->origin.x, current->origin.y + current->size.height - radius);
            c1 = cee_point_create(current->origin.x, current->origin.y + current->size.height);
            c2 = cee_point_create(current->origin.x, current->origin.y + current->size.height);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (next && rect_origin_compare(current, next) == 0) {
            p1 = cee_point_create(current->origin.x, current->origin.y + current->size.height);
            p2 = cee_point_create(current->origin.x, current->origin.y + current->size.height);
            c1 = NULL;
            c2 = NULL;
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (next && rect_origin_compare(current, next) == 1) {
            p1 = cee_point_create(current->origin.x - radius, current->origin.y + current->size.height);
            p2 = cee_point_create(current->origin.x, current->origin.y + current->size.height - radius);
            c1 = cee_point_create(current->origin.x, current->origin.y + current->size.height);
            c2 = cee_point_create(current->origin.x, current->origin.y + current->size.height);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        
        radius = default_radius;
        if (prev) {
            diff = rect_origin_diff(current, prev);
            radius = diff >= default_radius ? default_radius : diff;
        }
        else {
            radius = default_radius;
        }
        
        if (prev && rect_origin_compare(current, prev) == -1) {
            p1 = cee_point_create(current->origin.x, current->origin.y + radius);
            p2 = cee_point_create(current->origin.x + radius, current->origin.y);
            c1 = cee_point_create(current->origin.x, current->origin.y);
            c2 = cee_point_create(current->origin.x, current->origin.y);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (prev && rect_origin_compare(current, prev) == 0) {
            p1 = cee_point_create(current->origin.x, current->origin.y);
            p2 = cee_point_create(current->origin.x, current->origin.y);
            c1 = NULL;
            c2 = NULL;
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (prev && rect_origin_compare(current, prev) == 1) {
            p1 = cee_point_create(current->origin.x, current->origin.y + radius);
            p2 = cee_point_create(current->origin.x - radius, current->origin.y);
            c1 = cee_point_create(current->origin.x, current->origin.y);
            c2 = cee_point_create(current->origin.x, current->origin.y);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        else if (!prev) {
            p1 = cee_point_create(current->origin.x, current->origin.y + radius);
            p2 = cee_point_create(current->origin.x + radius, current->origin.y);
            c1 = cee_point_create(current->origin.x, current->origin.y);
            c2 = cee_point_create(current->origin.x, current->origin.y);
            points = cee_list_append(points, p1);
            points = cee_list_append(points, p2);
            points = cee_list_append(points, c1);
            points = cee_list_append(points, c2);
        }
        
        p = p->prev;
    }
    
    return points;
}
