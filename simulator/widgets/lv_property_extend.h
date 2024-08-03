/**
 * @file lv_property_extend.h
 *
 */

#ifndef LV_PROPERTY_EXTEND_H
#define LV_PROPERTY_EXTEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

enum {
  _EXT_PROPID_START = LV_PROPERTY_ID_BUILTIN_LAST + 1,
  LV_PROPERTY_POINTER_START = _EXT_PROPID_START + 0x100,     /* lv_pointer.c */
  LV_PROPERTY_ANALOG_TIME_START = _EXT_PROPID_START + 0x200, /* lv_pointer.c */
};

#endif /* LV_PROPERTY_EXTEND_H */
