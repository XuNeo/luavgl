#include <luavgl.h>

#include "lv_pointer.h"

static int luavgl_pointer_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_pointer_create);
}

static int _lv_pointer_set_range(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    /* No getter */
    lua_pushnil(L);
    return 1;
  }

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "expect date table.");
    return 1;
  }

  uint32_t value_start, value_range, angle_start, angle_range;
  lua_getfield(L, -1, "valueStart");
  value_start = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "valueRange");
  value_range = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "angleStart");
  angle_start = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "angleRange");
  angle_range = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_pointer_set_range(obj, value_start, value_range, angle_start, angle_range);
  return 1;
}

static int _lv_pointer_set_value(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    /* No getter */
    lua_pushnil(L);
    return 1;
  }

  lv_pointer_set_value(obj, lua_tointeger(L, -1));
  return 1;
}

static const luavgl_property_ops_t pointer_property_ops[] = {
    {.name = "range", .ops = _lv_pointer_set_range},
    {.name = "value", .ops = _lv_pointer_set_value},
};

static const luavgl_table_t pointer_property_table = {
    .len = sizeof(pointer_property_ops) / sizeof(pointer_property_ops[0]),
    .array = pointer_property_ops,
};

static const rotable_Reg luavgl_pointer_methods[] = {
    {"__property", LUA_TLIGHTUSERDATA, {.ptr = &pointer_property_table}},

    {0,            0,                  {0}                             },
};

void luavgl_pointer_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_pointer_class, "lv_pointer",
                          luavgl_pointer_methods);
  lua_pop(L, 1);

  luaL_getmetatable(L, "widgets");
  lua_pushcfunction(L, luavgl_pointer_create);
  lua_setfield(L, -2, "Pointer");
  lua_pop(L, 1);
}
