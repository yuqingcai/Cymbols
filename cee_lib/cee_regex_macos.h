//
//  cee_regex_macos.h
//  Cymbols
//
//  Created by qing on 2020/10/23.
//  Copyright © 2020 Lazycatdesign. All rights reserved.
//

#ifndef __CEE_REGEX_MACOS_H__
#define __CEE_REGEX_MACOS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cee_types.h"
#include "cee_list.h"

CEEList* cee_regex_macos_search(const cee_char* string,
                                const cee_char* pattern,
                                cee_boolean repeat,
                                cee_long timeout_ms,
                                cee_boolean* timeout);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_REGEX_MACOS_H__ */
