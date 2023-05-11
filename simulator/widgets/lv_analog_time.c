/*********************
 *      INCLUDES
 *********************/
#include "lv_analog_time.h"
#include "lv_pointer.h"

#include <time.h>

#if LV_USE_ANALOG_TIME

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_analog_time_class

#define UNIT_ONE_SECOND (1000)
#define UNIT_ONE_MINUTE (60 * UNIT_ONE_SECOND)
#define UNIT_ONE_HOUR (60 * UNIT_ONE_MINUTE)
#define UNIT_ONE_DAY (24 * UNIT_ONE_HOUR)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_analog_time_constructor(const lv_obj_class_t* class_p,
                                        lv_obj_t* obj);
static void lv_analog_time_destructor(const lv_obj_class_t* class_p,
                                       lv_obj_t* obj);
static void update_time(lv_obj_t* obj);
static void timer_cb(lv_timer_t* timer);
static lv_obj_t* create_hand(lv_obj_t* obj, const char* img);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_analog_time_class = {
    .constructor_cb = lv_analog_time_constructor,
    .destructor_cb = lv_analog_time_destructor,
    .instance_size = sizeof(lv_analog_time_t),
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_analog_time_create(lv_obj_t* parent)
{
    lv_obj_t* obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_analog_time_set_hands(lv_obj_t* obj, const void* hour,
                               const void* minute, const void* second)
{
    lv_analog_time_t* analog = (lv_analog_time_t*)obj;
    uint32_t period = UNIT_ONE_MINUTE; /* default */

    /* remove hand if it's null */
    if (analog->second) {
        lv_obj_del(analog->second);
        analog->second = NULL;
    }

    if (analog->minute) {
        lv_obj_del(analog->minute);
        analog->minute = NULL;
    }

    if (analog->hour) {
        lv_obj_del(analog->hour);
        analog->hour = NULL;
    }

    /* create hand if not null */

    if (hour) {
        analog->hour = create_hand(obj, hour);

        /* for hour, we use minute as unit*/
        lv_pointer_set_range(analog->hour, 0, 24 * 60, 0, 3600 * 2);

        /* update period should be faster than 1minute(default) */
    }

    if (minute) {
        analog->minute = create_hand(obj, minute);

        /* for minute, update per second */
        lv_pointer_set_range(analog->minute, 0, 60 * 60, 0, 3600);

        /* update period should be faster than 1second */
        if (period > 1 * UNIT_ONE_SECOND) {
            period = UNIT_ONE_SECOND;
        }
    }

    if (second) {
        analog->second = create_hand(obj, second);

        /* update per second */
        lv_pointer_set_range(analog->second, 0, 60 * UNIT_ONE_SECOND, 0, 3600);

        /* update period should be faster than 1second */
        if (period > 1 * UNIT_ONE_SECOND) {
            period = UNIT_ONE_SECOND;
        }
    }

    if (period < analog->period)
        analog->period = period;

    if (analog->hour || analog->minute || analog->second) {
        lv_timer_resume(analog->timer);
    } else {
        /* no more needed */
        lv_timer_pause(analog->timer);
    }

    update_time(obj);
}

void lv_analog_time_pause(lv_obj_t* obj)
{
    lv_analog_time_t* analog = (lv_analog_time_t*)obj;
    lv_timer_pause(analog->timer);
}

void lv_analog_time_resume(lv_obj_t* obj)
{
    lv_analog_time_t* analog = (lv_analog_time_t*)obj;
    lv_timer_resume(analog->timer);
    update_time(obj);
}

void lv_analog_time_set_period(lv_obj_t* obj, uint32_t period)
{
    lv_analog_time_t* analog = (lv_analog_time_t*)obj;
    analog->period = period;
    lv_timer_set_period(analog->timer, period);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_analog_time_constructor(const lv_obj_class_t* class_p,
                                        lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_analog_time_t* analog = (lv_analog_time_t*)obj;
    analog->period = 60 * 60 * 1000; /* default to 1Hour = 60min*60sec*1000ms*/
    lv_obj_add_flag(lv_obj_get_parent(obj), LV_OBJ_FLAG_OVERFLOW_VISIBLE);

    /* hands pivot is place to 0, 0 */
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 0, 0);

    analog->timer = lv_timer_create(timer_cb, UNIT_ONE_MINUTE, obj);
    lv_timer_pause(analog->timer);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_analog_time_destructor(const lv_obj_class_t* class_p,
                                       lv_obj_t* obj)
{
    lv_analog_time_t* analog = (lv_analog_time_t*)obj;
    lv_timer_del(analog->timer);
}

static lv_obj_t* create_hand(lv_obj_t* obj, const char* img)
{
    lv_obj_t* hand;
    hand = lv_pointer_create(obj);
    lv_img_set_src(hand, img);
    lv_point_t pivot;
    lv_img_get_pivot(hand, &pivot);

    /* we position the hand using image's center */
    lv_obj_set_pos(hand, -pivot.x, -pivot.y);
    return hand;
}

static void update_time(lv_obj_t* obj)
{
    int value;
    struct timespec ts;
    struct tm* time;
    int ret;

    ret = clock_gettime(CLOCK_REALTIME, &ts);
    if (ret != 0) {
        LV_LOG_ERROR("get real time failed");
        return;
    }

    time = localtime(&ts.tv_sec);
    if (time == NULL) {
        LV_LOG_ERROR("get local time failed");
        return;
    }

    lv_analog_time_t* analog = (lv_analog_time_t*)obj;

    value = time->tm_sec * 1000;
    if (analog->period < UNIT_ONE_SECOND) {
        /* sub-second update mode */
        value += ts.tv_nsec / 1000000; /* add ms part */
    }

    if (analog->second)
        lv_pointer_set_value(analog->second, value);

    if (analog->minute)
        lv_pointer_set_value(analog->minute, time->tm_min * 60 + time->tm_sec);

    if (analog->hour)
        lv_pointer_set_value(analog->hour, time->tm_hour * 60 + time->tm_min);
}

static void timer_cb(lv_timer_t* t)
{
    /* timeup */
    update_time(t->user_data);
}

/* examples */
#if 0

void lv_analog_time_example(void)
{
    lv_obj_t* scr = lv_scr_act();
    lv_obj_t* analog = lv_analog_time_create(scr);
    lv_obj_center(analog);
    lv_analog_time_set_hands(analog, "/data/hand-second.png", "/data/hand-second.png", "/data/hand-second.png");
    lv_analog_time_set_period(analog, 60);
}

#endif

#endif
