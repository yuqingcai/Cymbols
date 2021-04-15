//
//  cee_ui_macos.h
//  Cymbols
//
//  Created by qing on 2019/11/21.
//  Copyright © 2019 Lazycatdesign. All rights reserved.
//

#ifndef __CEE_UI_MACOS_H__
#define __CEE_UI_MACOS_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

cee_boolean cee_color_component_from_hsb_descriptor(const char* descriptor,
                                                    CGFloat components[4]);
cee_boolean cee_color_component_from_rgb_descriptor(const char* descriptor,
                                                    CGFloat components[4]);
cee_boolean cee_color_component_from_hex(const char* descriptor,
                                         CGFloat components[4]);
void cee_font_attributes_from_descriptor(const cee_char* descriptor,
                                         cee_char** family,
                                         cee_char** style,
                                         cee_char** size);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __CEE_UI_MACOS_H__ */
