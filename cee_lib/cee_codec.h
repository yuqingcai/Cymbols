#ifndef __CEE_CODEC_H__
#define __CEE_CODEC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cee_lib.h"

typedef uint32_t CEEUnicodePoint;

#define CEE_UNICODE_POINT_INVALID   0xFFFFFFFF
#define CEE_UNICODE_POINT_NUL       0x00000000
#define CEE_UNICODE_POINT_SOH       0x00000001
#define CEE_UNICODE_POINT_STX       0x00000002
#define CEE_UNICODE_POINT_ETX       0x00000003
#define CEE_UNICODE_POINT_EOT       0x00000004
#define CEE_UNICODE_POINT_ENQ       0x00000005
#define CEE_UNICODE_POINT_ACK       0x00000006
#define CEE_UNICODE_POINT_BEL       0x00000007
#define CEE_UNICODE_POINT_BS        0x00000008
#define CEE_UNICODE_POINT_HT        0x00000009
#define CEE_UNICODE_POINT_LF        0x0000000A
#define CEE_UNICODE_POINT_VT        0x0000000B
#define CEE_UNICODE_POINT_FF        0x0000000C
#define CEE_UNICODE_POINT_CR        0x0000000D
#define CEE_UNICODE_POINT_SO        0x0000000E
#define CEE_UNICODE_POINT_SI        0x0000000F
#define CEE_UNICODE_POINT_DLE       0x00000010
#define CEE_UNICODE_POINT_DC1       0x00000011
#define CEE_UNICODE_POINT_DC2       0x00000012
#define CEE_UNICODE_POINT_DC3       0x00000013
#define CEE_UNICODE_POINT_DC4       0x00000014
#define CEE_UNICODE_POINT_NAK       0x00000015
#define CEE_UNICODE_POINT_SYN       0x00000016
#define CEE_UNICODE_POINT_ETB       0x00000017
#define CEE_UNICODE_POINT_CAN       0x00000018
#define CEE_UNICODE_POINT_EM        0x00000019
#define CEE_UNICODE_POINT_SUB       0x0000001A
#define CEE_UNICODE_POINT_ESC       0x0000001B
#define CEE_UNICODE_POINT_FS        0x0000001C
#define CEE_UNICODE_POINT_GS        0x0000001D
#define CEE_UNICODE_POINT_RS        0x0000001E
#define CEE_UNICODE_POINT_US        0x0000001F
#define CEE_UNICODE_POINT_SP        0x00000020
#define CEE_UNICODE_POINT_DEL       0x0000007F
#define CEE_UNICODE_POINT_PAD       0x00000080
#define CEE_UNICODE_POINT_HOP       0x00000081
#define CEE_UNICODE_POINT_BPH       0x00000082
#define CEE_UNICODE_POINT_NBH       0x00000083
#define CEE_UNICODE_POINT_IND       0x00000084
#define CEE_UNICODE_POINT_NEL       0x00000085
#define CEE_UNICODE_POINT_SSA       0x00000086
#define CEE_UNICODE_POINT_ESA       0x00000087
#define CEE_UNICODE_POINT_HTS       0x00000088
#define CEE_UNICODE_POINT_HTJ       0x00000089
#define CEE_UNICODE_POINT_VTS       0x0000008A
#define CEE_UNICODE_POINT_PLD       0x0000008B
#define CEE_UNICODE_POINT_PLU       0x0000008C
#define CEE_UNICODE_POINT_RI        0x0000008D
#define CEE_UNICODE_POINT_SS2       0x0000008E
#define CEE_UNICODE_POINT_SS3       0x0000008F
#define CEE_UNICODE_POINT_DCS       0x00000090
#define CEE_UNICODE_POINT_PU1       0x00000091
#define CEE_UNICODE_POINT_PU2       0x00000092
#define CEE_UNICODE_POINT_STS       0x00000093
#define CEE_UNICODE_POINT_CCH       0x00000094
#define CEE_UNICODE_POINT_MW        0x00000095
#define CEE_UNICODE_POINT_SPA       0x00000096
#define CEE_UNICODE_POINT_EPA       0x00000097
#define CEE_UNICODE_POINT_SOS       0x00000098
#define CEE_UNICODE_POINT_SGCI      0x00000099
#define CEE_UNICODE_POINT_SCI       0x0000009A
#define CEE_UNICODE_POINT_CSI       0x0000009B
#define CEE_UNICODE_POINT_ST        0x0000009C
#define CEE_UNICODE_POINT_OSC       0x0000009D
#define CEE_UNICODE_POINT_PM        0x0000009E
#define CEE_UNICODE_POINT_APC       0x0000009F
#define CEE_UNICODE_POINT_UNKONW    0x000025A1


#define CODEC_ENCODING_UTF8         "encoding_utf8"
#define CODEC_ENCODING_UNKNOW       "encoding_unknow"
    
typedef enum _CEEUnicodePointType {
    kCEEUnicodePointTypeControl,
    kCEEUnicodePointTypeASCIIPunctuation,
    kCEEUnicodePointTypeASCIIAlphabet,
    kCEEUnicodePointTypeWorldWideCharacter,
} CEEUnicodePointType;
    

cee_boolean cee_codec_encoding_utf8(const cee_uchar* subject,
                                    cee_ulong length);

cee_long cee_codec_utf8_decode_next(const cee_uchar* subject,
                                    cee_long offset,
                                    CEEUnicodePoint* codepoint,
                                    cee_ulong* length);
cee_long cee_codec_utf8_decode_prev(const cee_uchar* subject,
                                    cee_long offset,
                                    CEEUnicodePoint* codepoint,
                                    cee_ulong* length);
cee_ulong cee_codec_utf8_decode_length(const cee_uchar* subject,
                                      cee_long offset);
CEEUnicodePointType cee_codec_unicode_point_type(CEEUnicodePoint codepoint);
cee_ulong cee_codec_utf8_nb_codepoint(const cee_uchar* subject,
                                      cee_ulong length);
cee_long cee_codec_utf8_encoded_byte0_get(const cee_uchar* subject,
                                          cee_long offset);
CEERange cee_codec_utf8_range_decoded_to_encoded(const cee_uchar* subject,
                                                 CEERange range);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CEE_CODEC_H__ */
