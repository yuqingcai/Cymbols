#ifndef __CEE_TYPES_H__
#define __CEE_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
        
typedef char cee_char;
typedef short cee_short;
typedef long cee_long;
typedef int cee_int;
typedef signed char cee_boolean;
typedef unsigned short cee_unichar;


typedef char cee_char;
typedef unsigned char cee_uchar;
typedef unsigned short cee_ushort;
typedef unsigned long cee_ulong;
typedef unsigned int cee_uint;
    
typedef float cee_float;
typedef double cee_double;

typedef void* cee_pointer;
    
typedef unsigned long cee_size;

#define TRUE 1
#define FALSE 0
#define CEE_POINTER_TO_INT(p) ((cee_int)(p))
#define CEE_POINTER_TO_UINT(p) ((cee_uint)(p))
#define CEE_POINTER_TO_LONG(p) ((cee_long)(p))
#define CEE_POINTER_TO_ULONG(p) ((cee_ulong)(p))
#define CEE_POINTER_TO_UINT(p) ((cee_uint)(p))
    
#define CEE_INT_TO_POINTER(i) ((cee_pointer)(i))
#define CEE_UINT_TO_POINTER(u) ((cee_pointer)(u))
#define CEE_LONG_TO_POINTER(i) ((cee_pointer)(i))
#define CEE_ULONG_TO_POINTER(u) ((cee_pointer)(u))

#define __DEFAULT__
#define __IN_OUT__
#define __IN__
#define __OUT__
#define __NULLABLE__


typedef void (*CEEFunc)(cee_pointer data,
                        cee_pointer user_data);

typedef void (*CEEDestroyNotify)(cee_pointer data);

typedef cee_pointer (*CEECopyFunc)(const cee_pointer src,
                                   cee_pointer data);

typedef cee_int (*CEECompareFunc)(const cee_pointer a,
                                  const cee_pointer b);
typedef cee_int (*CEECompareDataFunc)(const cee_pointer a,
                                      const cee_pointer b,
                                      cee_pointer user_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_TYPES_H__ */
