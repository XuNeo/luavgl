
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
#include "textarea.c"

static void lugl_widgets_init(lua_State *L)
{
  lugl_img_init(L);
  lugl_label_init(L);
  lugl_led_init(L);
  lugl_textarea_init(L);
  lugl_keyboard_init(L);
}
