#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include <luavgl.h>

#include "lv_pointer.h"

static int luavgl_pointer_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_pointer_create);
}

static void _lv_pointer_set_range(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "expect date table.");
    return;
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

  lv_pointer_set_range(obj, value_start, value_range, angle_start,
                        angle_range);
}

/* clang-format off */
static const luavgl_value_setter_t pointer_property_table[] = {
    {"range", SETTER_TYPE_STACK, {.setter_stack = _lv_pointer_set_range}},
    {"value", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_pointer_set_value}},
};

/* clang-format on */

static int luavgl_pointer_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, pointer_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_img_set_property_kv(L, obj);
  if (ret != 0) {
    printf("unkown property for pointer: %s\n", lua_tostring(L, -2));
  }

  return -1;
}

static int luavgl_pointer_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_pointer_set_property_kv, obj);

  return 0;
}

static const luaL_Reg luavgl_pointer_methods[] = {
    {"set", luavgl_pointer_set},

    {NULL,  NULL              },
};

void luavgl_pointer_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_pointer_class, "lv_pointer",
                          luavgl_pointer_methods);
  lua_pop(L, 1);

  luaL_getmetatable(L, "widgets");
  lua_getfield(L, -1, "__index");
  lua_pushcfunction(L, luavgl_pointer_create);
  lua_setfield(L, -2, "Pointer");
  lua_pop(L, 2);
}
