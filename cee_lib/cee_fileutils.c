#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>

#include "cee_mem.h"
#include "cee_list.h"
#include "cee_strfuncs.h"
#include "cee_fileutils.h"

cee_char* cee_path_build(const cee_char *separator,
                         const cee_char *first_element,
                         ...)
{
    if (!separator)
        return NULL;
    
    if (!first_element)
        return NULL;
    
    cee_char* str;
    va_list args;
    cee_size l;
    cee_char* s;
    cee_char* ptr;
    
    CEEList* strv = NULL;
    CEEList* p = NULL;
    
    strv = cee_list_append(strv, cee_strdup(first_element));
    va_start(args, first_element);
    s = va_arg(args, cee_char*);
    while (s) {
        strv = cee_list_append(strv, cee_strdup(s));
        s = va_arg(args, cee_char*);
    }
    va_end(args);
    
    l = 0;
    p = strv;
    while (p) {
        s = p->data;
        cee_strstrip(s);
        l += strlen(s);
        p = p->next;
    }
    
    l += strlen(separator)*(cee_list_length(strv)-1);
    l += 1; /** '\0' terminal */
    
    ptr = str = cee_malloc0(sizeof(cee_char)*l);
    p = strv;
    while (p) {
        s = p->data;
        ptr = cee_stpcpy(ptr, s);
        
        if (p->next)
            ptr = cee_stpcpy(ptr, separator);
        
        p = p->next;
    }
    
    cee_list_free_full(strv, cee_free);
    
    
    return str;
}

#ifdef __WIN32__
static void contents_in_dir_recursive(const cee_char* path,
                                      DIR *dir,
                                      CEEList** contents)
{
}
#elif __APPLE__
static void contents_in_dir_recursive(const cee_char* path,
                                      DIR *dir,
                                      CEEList** contents)
{
    struct dirent *entry;
    while(dir) {
        entry = readdir(dir);
        if (!entry)
            break;
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                cee_char* child_path = cee_path_build ("/", path, entry->d_name, NULL);
                DIR *child_dir = opendir(child_path);
                contents_in_dir_recursive(child_path, child_dir, contents);
                closedir(child_dir);
                cee_free(child_path);
            }
        }
        else if (entry->d_type == DT_REG) {
            cee_char* child_path = cee_path_build ("/", path, entry->d_name, NULL);
            *contents = cee_list_prepend(*contents, child_path);
        }
    }
}
#elif __linux__

#endif


CEEList* cee_dir_enumerate(const cee_char* path)
{
    CEEList* contents = NULL;
    DIR *dir = opendir(path);
    contents_in_dir_recursive(path, dir, &contents);
    closedir(dir);
    return contents;
}

cee_boolean cee_path_is_dir(const cee_char* path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

cee_boolean cee_path_is_regular(const cee_char* path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

CEEList* cee_path_expand(const cee_char* path)
{
    CEEList* regulars = NULL;
    if (cee_path_is_regular(path))
        regulars = cee_list_prepend(regulars, cee_strdup(path));
    else if (cee_path_is_dir(path)) {
        CEEList* contents = cee_dir_enumerate(path);
        regulars = cee_list_concat(regulars, contents);
    }
    
    return regulars;
}

CEEList* cee_paths_expand(CEEList* paths)
{
    CEEList* regulars = NULL;
    
    CEEList* p = paths;
    while (p) {
        const cee_char* path = p->data;
        
        if (cee_path_is_regular(path))
            regulars = cee_list_prepend(regulars, cee_strdup(path));
        else if (cee_path_is_dir(path)) {
            CEEList* contents = cee_dir_enumerate(path);
            regulars = cee_list_concat(regulars, contents);
        }
        p = p->next;
    }
    
    return regulars;
}

cee_boolean cee_dir_existed(const cee_char* path)
{
    DIR* dir = opendir(path);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return TRUE;
    }
    return FALSE;
}

cee_boolean cee_path_existed(const cee_char* path)
{
    return (access(path, F_OK) != -1);
}


#ifdef __WIN32__

#elif __APPLE__
#define PATH_MAX_STRING_SIZE 512
/* recursive mkdir */
cee_int cee_dir_make_p(const cee_char* path)
{
    mode_t mode = S_IRWXU |S_IRWXG | S_IROTH | S_IXOTH;
    
    cee_char tmp[PATH_MAX_STRING_SIZE];
    cee_char* p = NULL;
    struct stat sb;
    size_t len;
    
    /* copy path */
    len = strnlen (path, PATH_MAX_STRING_SIZE);
    if (len == 0 || len == PATH_MAX_STRING_SIZE) {
        return -1;
    }
    memcpy (tmp, path, len);
    tmp[len] = '\0';
    
    /* remove trailing slash */
    if(tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }
    
    /* check if path exists and is a directory */
    if (stat (tmp, &sb) == 0) {
        if (S_ISDIR (sb.st_mode)) {
            return 0;
        }
    }
    
    /* recursive mkdir */
    for(p = tmp + 1; *p; p++) {
        if(*p == '/') {
            *p = 0;
            /* test path */
            if (stat(tmp, &sb) != 0) {
                /* path does not exist - create directory */
                if (mkdir(tmp, mode) < 0) {
                    return -1;
                }
            } else if (!S_ISDIR(sb.st_mode)) {
                /* not a directory */
                return -1;
            }
            *p = '/';
        }
    }
    /* test path */
    if (stat(tmp, &sb) != 0) {
        /* path does not exist - create directory */
        if (mkdir(tmp, mode) < 0) {
            return -1;
        }
    } else if (!S_ISDIR(sb.st_mode)) {
        /* not a directory */
        return -1;
    }
    return 0;
}

static void rmdir_f(const cee_char* path,
                    DIR *dir)
{
    struct dirent *entry;
    while(dir) {
        entry = readdir(dir);
        
        if (!entry)
            break;
        
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                cee_char* child_path = cee_path_build("/", path, entry->d_name, NULL);
                DIR *child_dir = opendir(child_path);
                rmdir_f(child_path, child_dir);
                closedir(child_dir);
                rmdir(child_path);
                cee_free(child_path);
            }
        }
        else if (entry->d_type == DT_REG) {
            cee_char* child_path = cee_path_build("/", path, entry->d_name, NULL);
            remove(child_path);
        }
    }
}
#elif __linux__

#endif

cee_int cee_dir_remove_rf(const cee_char* path)
{
    DIR *dir = opendir(path);
    rmdir_f(path, dir);
    closedir(dir);
    rmdir(path);
    return 0;
}

cee_char* cee_path_basename(const cee_char* path)
{
    if (!path)
        return NULL;
    
    cee_long i = strlen(path)-1;
    cee_char spliter = '/';
    
    if (path[i] == spliter)
        i --;
    
    while (i >= 0) {
        if (path[i] == spliter)
            return cee_strdup(&path[i+1]);
        i --;
    }
    return cee_strdup(path);
}

cee_char* cee_path_extension(const cee_char* path)
{
    cee_char* ptr = NULL;
    cee_char* basename = NULL;
    const cee_char* dot = NULL;
    cee_long offset = 0;
    
    basename = cee_strtrim(cee_path_basename(path), " \t\v\n\r\f");
    if (!basename)
        return NULL;
    
    dot = strrchr(basename, '.');
    if(dot && dot != basename) {
        offset = dot - basename;
        if (basename[offset+1] != '\0')
            ptr = cee_strdup(dot+1);
    }
    cee_free(basename);
    
    return ptr;
}

cee_char* cee_path_remove_extension(const cee_char* path)
{
    cee_char* ptr = NULL;
    const cee_char* dot = NULL;
    cee_long count = 0;
    
    dot = strrchr(path, '.');
    if(dot && dot != path) {
        count = dot - path;
        ptr = cee_strndup(path, count);
    }
    else {
        ptr = cee_strdup(path);
    }
    
    return ptr;
}

cee_boolean cee_path_contained(CEEList* paths,
                               const cee_char* path)
{
    if (!paths)
        return FALSE;
    
    if (!path)
        return FALSE;
    
    CEEList* p = paths;
    while (p) {
        if (!strcmp(p->data, path))
            return TRUE;
            
        p = p->next;
    }
    
    return FALSE;
}

void cee_paths_exclusive_append(CEEList** paths,
                                CEEList* appendings,
                                CEEList** appendeds,
                                CEEList** excludeds)
{
    if (!paths)
        return;
    
    CEEList* regulars = cee_paths_expand(appendings);
    if (!regulars) {
        if (appendeds)
            *appendeds = NULL;
        if (excludeds)
            *excludeds = NULL;
        return ;
    }
    
    if (!*paths) {
        *paths = regulars;
        if (appendeds) {
            CEEList* p = regulars;
            while (p) {
                const cee_char* path = p->data;
                *appendeds = cee_list_prepend(*appendeds, cee_strdup(path));
                p = p->next;
            }
        }
        if (excludeds)
            *excludeds = NULL;
    }
    else {
        CEEList* _appendings = NULL;
        CEEList* _excludeds = NULL;
        
        CEEList* p = regulars;
        while (p) {
            const cee_char* path = p->data;
            
            if (cee_path_contained(*paths, path))
                _excludeds =
                    cee_list_prepend(_excludeds, cee_strdup(path));
            else
                _appendings =
                    cee_list_prepend(_appendings, cee_strdup(path));
            
            p = p->next;
        }
        
        if (_appendings) {
            CEEList* p = _appendings;
            while (p) {
                const cee_char* path = p->data;
                *paths = cee_list_prepend(*paths, cee_strdup(path));
                p = p->next;
            }
        }
        
        if (appendeds)
            *appendeds = _appendings;
        else
            cee_list_free_full(_appendings, cee_free);
        
        if (excludeds)
            *excludeds = _excludeds;
        else
            cee_list_free_full(_excludeds, cee_free);
        
        cee_list_free_full(regulars, cee_free);
    }
    
    return;
}

cee_boolean cee_paths_equal(const cee_char* path0,
                            const cee_char* path1)
{
    if (!path0)
        return FALSE;
    
    if (!path1)
        return FALSE;
    
    if (!strcmp(path0, path1))
        return TRUE;
    
    return FALSE;
}

cee_boolean cee_path_parent(const cee_char* path0,
                            const cee_char* path1)
{
    
    if (!path0)
        return FALSE;
    
    if (!path1)
        return FALSE;
    
    if (!cee_path_is_dir(path0))
        return FALSE;
    
    cee_char separator;
    
    separator = '/';
#ifdef __WIN32__
    separator = '\\';
#endif
    
    cee_char* dirname = cee_path_dirname(path1);
    if (!dirname)
        return FALSE;
    
    cee_boolean ret = FALSE;
    if (strlen(dirname) == strlen(path0) && cee_paths_equal(path0, dirname))
        ret = TRUE;
    else if (strlen(dirname) > strlen(path0)) {
        cee_uint len = (cee_uint)strlen(path0);
        cee_char* prefix = cee_strndup(dirname, len);
        if (prefix) {
            if (cee_paths_equal(path0, prefix) && dirname[len] == separator)
                ret = TRUE;
            cee_free(prefix);
        }
    }
    
    cee_free(dirname);
    
    return ret;
}

cee_char* cee_path_dirname(const cee_char* path)
{
    cee_char* dirname;
    cee_char* basename;
    cee_size length;
    
    if (!path)
        return NULL;
    
    cee_char separator;
    
    separator = '/';
#ifdef __WIN32__
    separator = '\\';
#endif
    
    basename = strrchr(path, separator);
    if (!basename) {
        dirname = cee_strdup(".");
        return dirname;
    }
    
    length = basename - path;
    dirname = (cee_char*)cee_malloc0(sizeof(cee_char)*(length+1));
    memcpy(dirname, path, length);
    
    return dirname;
}

CEEList* cee_paths_uniform(CEEList* paths)
{
    CEEList* regulars = NULL;
    
    CEEList* p = paths;
    while (p) {
        
        cee_char* path = p->data;
        cee_boolean ignored = FALSE;
        
        if (!cee_path_existed(path))
            p = p->next;
        
        CEEList* regular = regulars;
        while (regular) {
            cee_char* path_r = regular->data;
            if (cee_paths_equal(path_r, path)) {
                ignored = TRUE;
                break;
            }
            else if(cee_path_parent(path_r, path)) {
                ignored = TRUE;
                break;
            }
            regular = regular->next;
        }
        
        if (!ignored) {
            CEEList* children = NULL;
            
            CEEList* regular = regulars;
            while (regular) {
                cee_char* path_r = regular->data;
                if(cee_path_parent(path, path_r))
                    children = cee_list_append(children, regular);
                regular = regular->next;
            }
            
            CEEList* child = children;
            while (child) {
                CEEList* node = child->data;
                regulars = cee_list_remove_link(regulars, node);
                cee_list_free_full(node, cee_free);
                child = child->next;
            }
            cee_list_free(children);
            
            regulars = cee_list_append(regulars, cee_strdup(path));
        }
        
        p = p->next;
    }
    
    p = regulars;
    
    return regulars;
}

cee_boolean cee_file_contents_get(const cee_char *path,
                                  cee_uchar** contents,
                                  cee_size* length)
{
    if (!path)
        return FALSE;
    
    if (!contents)
        return FALSE;
    
    *contents = NULL;
    
    if (length)
        *length = 0;
    
    FILE* fp;
    cee_long l;
    cee_uchar* buffer;
    size_t result;
    
    fp = fopen(path , "rb");
    if (!fp)
        return FALSE;
    
    fseek(fp , 0 , SEEK_END);
    l = ftell(fp);
    rewind(fp);
    
    buffer = (cee_uchar*)cee_malloc0(sizeof(cee_uchar)*(l+1));
    if (!buffer)
        goto error_exit;
    
    result = fread(buffer, 1, l, fp);
    if (result != l)
        goto error_exit;
    
    if (length)
        *length = l;
    
    *contents = buffer;
    
    fclose (fp);
    
    return TRUE;
    
error_exit:
    
    if (buffer)
        cee_free(buffer);
    
    if (fp)
        fclose(fp);
    
    return FALSE;
}





