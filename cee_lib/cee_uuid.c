#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "cee_mem.h"
#include "cee_uuid.h"


typedef struct {
    cee_uchar bytes[16];
} CEEUUID;

static cee_char *cee_uuid_to_string(const CEEUUID *uuid)
{
    const cee_uchar *bytes;
    
    if (!uuid)
        return NULL;
    
    bytes = uuid->bytes;
    
    cee_char* str = (cee_char*)cee_malloc0(sizeof(cee_char)*37);
    
    sprintf(str, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x"
            "-%02x%02x%02x%02x%02x%02x",
            bytes[0], bytes[1], bytes[2], bytes[3],
            bytes[4], bytes[5], bytes[6], bytes[7],
            bytes[8], bytes[9], bytes[10], bytes[11],
            bytes[12], bytes[13], bytes[14], bytes[15]);
    return str;
}



static void uuid_set_version(CEEUUID *uuid, cee_uint version)
{
    cee_uchar *bytes = uuid->bytes;

    bytes[6] &= 0x0f;
    bytes[6] |= version << 4;

    bytes[8] &= 0x3f;
    bytes[8] |= 0x80;
}

static void cee_uuid_generate_v4(CEEUUID *uuid)
{
    cee_int i;
    cee_uchar* bytes;
    cee_uint* ints;
    
    if (!uuid)
        return ;
    
    time_t t;
    srand((unsigned int)time(&t));
    
    bytes = uuid->bytes;
    ints = (cee_uint*)bytes;
    for (i = 0; i < 4; i++)
        ints[i] = rand();
    
    uuid_set_version(uuid, 4);
}

cee_char* cee_uuid_string_random()
{
    CEEUUID uuid;
    cee_uuid_generate_v4(&uuid);
    return cee_uuid_to_string(&uuid);
}
