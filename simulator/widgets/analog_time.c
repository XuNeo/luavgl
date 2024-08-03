#include <luavgl.h>

#include "lv_analog_time.h"

static int luavgl_analog_time_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_analog_time_create);
}

static int analog_time_set_hands(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set)
    return 0;

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "expect date table.");
    return 1;
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
  return 1;
}

static const luavgl_property_ops_t analog_time_property_ops[] = {
    {.name = "hands", .ops = analog_time_set_hands},
};

static const luavgl_table_t analog_time_property_table = {
    .len = sizeof(analog_time_property_ops) / sizeof(luavgl_property_ops_t),
    .array = analog_time_property_ops,
};

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

static const rotable_Reg luavgl_analog_time_methods[] = {
    {"pause",      LUA_TFUNCTION,      {luavgl_analog_time_pause}          },
    {"resume",     LUA_TFUNCTION,      {luavgl_analog_time_resume}         },
    {"__property", LUA_TLIGHTUSERDATA, {.ptr = &analog_time_property_table}},

    {0,            0,                  {0}                                 },
};

void luavgl_analog_time_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_analog_time_class, "lv_analog_time",
                          luavgl_analog_time_methods);
  lua_pop(L, 1);

  luaL_getmetatable(L, "widgets");
  lua_pushcfunction(L, luavgl_analog_time_create);
  lua_setfield(L, -2, "AnalogTime");
  lua_pop(L, 1);
}
