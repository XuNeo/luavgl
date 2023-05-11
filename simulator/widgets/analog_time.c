#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include <luavgl.h>

#include "lv_analog_time.h"

static int luavgl_analog_time_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_analog_time_create);
}

static void _lv_analog_time_set_hands(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "expect date table.");
    return;
  }

  const void *hour, *minute, *second;
  lua_getfield(L, -1, "hour");

  hour = luavgl_toimgsrc(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "minute");
  minute = luavgl_toimgsrc(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "second");
  second = luavgl_toimgsrc(L, -1);
  lua_pop(L, 1);

  lv_analog_time_set_hands(obj, hour, minute, second);
}

/* clang-format off */
static const luavgl_value_setter_t analog_time_property_table[] = {
    {"hands", SETTER_TYPE_STACK, {.setter_stack = _lv_analog_time_set_hands}},
    {"period", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_analog_time_set_period}},
};

/* clang-format on */

static int luavgl_analog_time_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, analog_time_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    printf("unkown property for analog_time: %s\n", lua_tostring(L, -2));
  }

  return -1;
}

static int luavgl_analog_time_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_analog_time_set_property_kv, obj);

  return 0;
}

static int luavgl_analog_time_pause(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_analog_time_pause(obj);
  return 0;
}

static int luavgl_analog_time_resume(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_analog_time_resume(obj);
  return 0;
}

static const luaL_Reg luavgl_analog_time_methods[] = {
    {"set",    luavgl_analog_time_set   },
    {"pause",  luavgl_analog_time_pause },
    {"resume", luavgl_analog_time_resume},

    {NULL,     NULL                     },
};

void luavgl_analog_time_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_analog_time_class, "lv_analog_time",
                          luavgl_analog_time_methods);
  lua_pop(L, 1);

  luaL_getmetatable(L, "widgets");
  lua_getfield(L, -1, "__index");
  lua_pushcfunction(L, luavgl_analog_time_create);
  lua_setfield(L, -2, "AnalogTime");
  lua_pop(L, 2);
}
