
#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

#include "calendar.c"
#include "checkbox.c"
#include "dropdown.c"
#include "img.c"
#include "keyboard.c"
#include "label.c"
#include "led.c"
#include "list.c"
#include "roller.c"
#include "textarea.c"

/* clang-format off */
#define WIDGET_CREATE_FUNCTIONS                 \
    { "Object", lugl_obj_create },              \
    \
    { "Calendar", lugl_calendar_create },       \
    { "Checkbox", lugl_checkbox_create },       \
    { "Dropdown", lugl_dropdown_create },       \
    { "Image", lugl_img_create },               \
    { "Textarea", lugl_textarea_create },       \
    { "Keyboard", lugl_keyboard_create },       \
    { "Label", lugl_label_create },             \
    { "Led", lugl_led_create },                 \
    { "List", lugl_list_create },               \
    { "Roller", lugl_roller_create }

/* clang-format on */

static void lugl_widgets_init(lua_State *L)
{
  lugl_img_init(L);
  lugl_label_init(L);
  lugl_led_init(L);
  lugl_list_init(L);
  lugl_textarea_init(L);
  lugl_keyboard_init(L);
  lugl_checkbox_init(L);
  lugl_calendar_init(L);
  lugl_roller_init(L);
  lugl_dropdown_init(L);
}
