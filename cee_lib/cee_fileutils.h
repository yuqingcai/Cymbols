#ifndef __CEE_FILEUTILS_H__
#define __CEE_FILEUTILS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cee_types.h"
    
cee_char* cee_path_build(const cee_char *separator,
                         const cee_char *first_element,
                         ...);
CEEList* cee_dir_enumerate(const cee_char* path);
cee_boolean cee_path_is_dir(const cee_char* path);
cee_boolean cee_path_is_regular(const cee_char* path);
CEEList* cee_path_expand(const cee_char* path);
CEEList* cee_paths_expand(CEEList* paths);
cee_boolean cee_dir_existed(const cee_char* path);
cee_boolean cee_path_existed(const cee_char* path);
cee_int cee_dir_make_p(const cee_char* path);
cee_int cee_dir_remove_rf(const cee_char* path);
    
cee_char* cee_path_basename(const cee_char* path);
cee_char* cee_path_extension(const cee_char* path);
cee_char* cee_path_remove_extension(const cee_char* path);
    
cee_boolean cee_path_contained(CEEList* paths,
                               const cee_char* path);
void cee_paths_exclusive_append(CEEList** paths,
                                CEEList* appendings,
                                CEEList** appendeds,
                                CEEList** excludeds);
cee_boolean cee_paths_equal(const cee_char* path0,
                            const cee_char* path1);
cee_boolean cee_path_parent(const cee_char* path0,
                            const cee_char* path1);
cee_char* cee_path_dirname(const cee_char* path);
CEEList* cee_paths_uniform(CEEList* paths);
cee_boolean cee_file_contents_get(const cee_char *filename,
                                  cee_uchar** contents,
                                  cee_size* length);

typedef struct _CEEFileContent {
    cee_char* filepath;
    CEEList* blocks;
} CEEFileContent;

typedef struct _CEEFileContentBlock {
    cee_char* content;
    cee_ulong size;
} CEEFileContentBlock;

CEEList* cee_file_content_blocks_get(const cee_char *filename,
                                     cee_size block_size);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_FILEUTILS_H__ */
