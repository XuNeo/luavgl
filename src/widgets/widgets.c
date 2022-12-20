
#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

#include "led.c"

static void lugl_widgets_init(lua_State* L)
{
    lugl_led_init(L);
}
