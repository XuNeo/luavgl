#ifndef LV_WIDGETS_POINTER_H_
#define LV_WIDGETS_POINTER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <lvgl.h>

#if LV_USE_POINTER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_img_t obj;
    int value;
    int value_start;
    int value_range;
    int angle_start;
    int angle_range;
} lv_pointer_t;

extern const lv_obj_class_t lv_pointer_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t* lv_pointer_create(lv_obj_t* parent);

/**
 * Set range parameters for pointer
 * @param value_start the value mapped to angle_start
 * @param value_range the value range, could be negative
 * @param angle_start the angle will be set then value equals to value_start
 * @param angle_range the angle range, could be negative
 */
void lv_pointer_set_range(lv_obj_t* obj, int value_start, int value_range,
                           int angle_start, int angle_range);

/**
 * Set current value pointer points to
 * @param value If value is out of range, then angle is also clampped to limits
 */
void lv_pointer_set_value(lv_obj_t* obj, int value);

/**********************
 *      MACROS
 **********************/

#endif /*  */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_WIDGETS_POINTER_H_ */
