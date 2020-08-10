#ifndef __CEE_GEOMETRY_H__
#define __CEE_GEOMETRY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_list.h"

typedef struct _CEEPoint {
    cee_float x;
    cee_float y;
} CEEPoint;

typedef struct _CEESize {
    cee_float width;
    cee_float height;
} CEESize;

typedef struct _CEERect {
    CEEPoint origin;
    CEESize size;
} CEERect;

CEEPoint cee_point_make(cee_float x,
                        cee_float y);
CEEPoint* cee_point_create(cee_float x,
                           cee_float y);
void cee_point_free(cee_pointer data);
CEESize cee_size_make(cee_float width,
                      cee_float height);
CEESize* cee_size_create(cee_float width,
                         cee_float height);
void cee_size_free(cee_pointer data);
CEERect cee_rect_make(cee_float x,
                      cee_float y,
                      cee_float width,
                      cee_float height);
CEERect* cee_rect_create(cee_float x,
                         cee_float y,
                         cee_float width,
                         cee_float height);
void cee_rect_free(cee_pointer data);
cee_boolean cee_point_in_rect(CEEPoint point,
                              CEERect rect);
CEEList* cee_rects_outline_create(CEEList* rects);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_GEOMETRY_H__ */
