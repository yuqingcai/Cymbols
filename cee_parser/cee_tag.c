#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cee_tag.h"
#include "cee_range.h"
#include "cee_datetime.h"

void cee_tag_free(cee_pointer data)
{
    if (!data)
        return;
    
    CEETag* tag = (CEETag*)data;
    
    cee_free(tag);
}

CEETag* cee_tag_create(CEETagType type,
                       CEERange range)
{
    CEETag* tag = (CEETag*)cee_malloc0(sizeof(CEETag));
    if (!tag)
        return NULL;
    
    tag->type = type;
    tag->range = range;
    return tag;
}

cee_int cee_tag_compare(const cee_pointer a,
                        const cee_pointer b)
{
    CEETag* tag0 = (CEETag*)a;
    CEETag* tag1 = (CEETag*)b;
    
    if (tag0->range.location < tag1->range.location)
        return -1;
    else if (tag0->range.location > tag1->range.location)
        return 1;
    
    return 0;
}
