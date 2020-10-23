#ifdef __APPLE__
#include "cee_regex_macos.h"
#elif __WIN32__

#elif __linux__

#endif

CEEList* cee_regex_search(const cee_char* string,
                          const cee_char* pattern,
                          cee_boolean repeat,
                          cee_long timeout_ms,
                          cee_boolean* timeout)
{
#ifdef __APPLE__
    return cee_regex_macos_search(string, 
                                  pattern, 
                                  repeat, 
                                  timeout_ms, 
                                  timeout);
#elif __WIN32__

#elif __linux__

#endif

}
