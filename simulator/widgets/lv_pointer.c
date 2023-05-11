/*********************
 *      INCLUDES
 *********************/
#include "lv_pointer.h"

#if LV_USE_POINTER

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_pointer_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_pointer_constructor(const lv_obj_class_t* class_p,
                                    lv_obj_t* obj);
static void angle_update(lv_obj_t* obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_pointer_class = {
    .constructor_cb = lv_pointer_constructor,
    .instance_size = sizeof(lv_pointer_t),
    .base_class = &lv_img_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_pointer_create(lv_obj_t* parent)
{
    lv_obj_t* obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    /* pointer can rotate out of parent's area. */
    lv_obj_add_flag(parent, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    return obj;
}

void lv_pointer_set_value(lv_obj_t* obj, int value)
{
    lv_pointer_t* pointer = (lv_pointer_t*)obj;
    if (pointer->value == value) {
        return;
    }

    pointer->value = value;
    angle_update(obj);
}

void lv_pointer_set_range(lv_obj_t* obj, int value_start, int value_range,
                           int angle_start, int angle_range)
{
    lv_pointer_t* pointer = (lv_pointer_t*)obj;
    pointer->value_start = value_start;
    pointer->value_range = value_range;
    pointer->angle_start = angle_start;
    pointer->angle_range = angle_range;

    /* update angle */
    angle_update(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_pointer_constructor(const lv_obj_class_t* class_p,
                                    lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_pointer_t* pointer = (lv_pointer_t*)obj;

    /* set default range parameters */
    pointer->angle_start = 0;
    pointer->angle_range = 360 * 10;
    pointer->value_start = 0;
    pointer->value_range = 100;
    LV_TRACE_OBJ_CREATE("finished");
}

static void angle_update(lv_obj_t* obj)
{
    lv_pointer_t* pointer = (lv_pointer_t*)obj;
    int angle = 0;
    int value = pointer->value;
    int value_start = pointer->value_start;
    int value_range = pointer->value_range;
    int angle_start = pointer->angle_start;
    int angle_range = pointer->angle_range;

    if (value_range != 0) {
        /* Check value overflow, overwrite angle if so. */
        int delta = value - value_start;
        angle = angle_start;
        angle += (delta * angle_range) / value_range;

        /* check overflow */
        if (value_range > 0) {
            if (delta > value_range)
                angle = angle_start + angle_range;
            else if (value < value_start)
                angle = angle_start;
        } else { /* case of value_range < 0 */
            if (delta < value_range)
                angle = angle_start + angle_range;
            else if (value > value_start)
                angle = angle_start;
        }

        while (angle >= 3600)
            angle -= 3600;
        while (angle < 0)
            angle += 3600;
    }

    if (angle == lv_img_get_angle(obj)) {
        /* not changed */
        return;
    }

    lv_img_set_angle(obj, angle);
}

#endif
