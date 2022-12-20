
#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

#include "img.c"
#include "keyboard.c"
#include "label.c"
#include "led.c"
#include "list.c"
#include "textarea.c"

/* clang-format off */
#define WIDGET_CREATE_FUNCTIONS                 \
    { "Object", lugl_obj_create },              \
    { "Image", lugl_img_create },               \
    { "Label", lugl_label_create },             \
    { "Textarea", lugl_textarea_create },       \
    { "Keyboard", lugl_keyboard_create },       \
    { "Led", lugl_led_create },                 \
    { "List", lugl_list_create }

/* clang-format on */

static void lugl_widgets_init(lua_State *L)
{
  lugl_img_init(L);
  lugl_label_init(L);
  lugl_led_init(L);
  lugl_list_init(L);
  lugl_textarea_init(L);
  lugl_keyboard_init(L);
}
