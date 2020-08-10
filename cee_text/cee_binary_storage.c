#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "cee_binary_storage.h"

CEEBinaryStorageRef cee_binary_storage_create(cee_pointer host_ref,
                                              const cee_uchar* string,
                                              cee_ulong length,
                                              void (*buffer_update)(cee_pointer, 
                                                                    CEEBinaryStorageRef,
                                                                    CEERange,
                                                                    CEERange))
{
    return NULL;
}

void cee_binary_storage_free(CEEBinaryStorageRef data)
{
    
}
