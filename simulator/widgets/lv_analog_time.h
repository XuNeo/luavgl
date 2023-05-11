#ifndef LV_WIDGETS_ANALOG_TIME_H_
#define LV_WIDGETS_ANALOG_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <lvgl.h>

#if LV_USE_ANALOG_TIME

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    lv_obj_t* hour;
    lv_obj_t* minute;
    lv_obj_t* second;
    uint32_t period;
    lv_timer_t* timer;
} lv_analog_time_t;

extern const lv_obj_class_t lv_analog_time_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t* lv_analog_time_create(lv_obj_t* parent);

/**
 * Set images for hands of hour, minute and second.
 * If specified hands image is NULL, then it's deleted.
 *
 * @note for simplicity, image pivot is set to image center.
 */
void lv_analog_time_set_hands(lv_obj_t* obj, const void* hour,
                               const void* minute, const void* second);

void lv_analog_time_pause(lv_obj_t* obj);
void lv_analog_time_resume(lv_obj_t* obj);
void lv_analog_time_set_period(lv_obj_t* obj, uint32_t period);

/**********************
 *      MACROS
 **********************/

#endif /*  */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_WIDGETS_ANALOG_TIME_H_ */
