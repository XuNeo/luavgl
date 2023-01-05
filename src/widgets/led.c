#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_led_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_led_create);
}

/* clang-format off */
static const lugl_value_setter_t led_property_table[] = {
    {"color", SETTER_TYPE_COLOR, {.setter = (setter_int_t)lv_led_set_color}},
    {"brightness", 0, {.setter = (setter_int_t)lv_led_set_brightness}},
};
/* clang-format on */

static int lugl_led_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, led_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for led.\n");
  }

  return -1;
}

static int lugl_led_set(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  lugl_iterate(L, -1, lugl_led_set_property_kv, obj);

  return 0;
}

static int lugl_led_on(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_led_on(obj);
  return 0;
}

static int lugl_led_off(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_led_off(obj);
  return 0;
}

static int lugl_led_toggle(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_led_toggle(obj);
  return 0;
}

static int lugl_led_get_brightness(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  lua_pushinteger(L, lv_led_get_brightness(obj));
  return 1;
}

static const luaL_Reg lugl_led_methods[] = {
    {"set", lugl_led_set},
    {"on", lugl_led_on},
    {"off", lugl_led_off},
    {"toggle", lugl_led_toggle},

    {"get_brightness", lugl_led_get_brightness},

    {NULL, NULL},
};

static void lugl_led_init(lua_State *L)
{
  lugl_obj_newmetatable(L, &lv_led_class, "lv_led", lugl_led_methods);
  lua_pop(L, 1);
}
