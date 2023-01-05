#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include <lugl.h>

static int lugl_extension_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_obj_create);
}

/* clang-format off */
static const lugl_value_setter_t extension_property_table[] = {
    {"dummy", SETTER_TYPE_STACK, {.setter_stack = _lv_dummy_set}},
};
/* clang-format on */

static int lugl_extension_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, extension_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    printf("unkown property for extension.\n");
  }

  return -1;
}

static int lugl_extension_set(lua_State *L)
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

  lugl_iterate(L, -1, lugl_extension_set_property_kv, obj);

  return 0;
}

static const luaL_Reg lugl_extension_methods[] = {
    {"set", lugl_extension_set},

    {NULL, NULL},
};

void lugl_extension_init(lua_State *L)
{
  lugl_obj_newmetatable(L, &lv_btn_class, "lv_extension",
                        lugl_extension_methods);
  lua_pop(L, 1);

  luaL_getmetatable(L, "widgets");
  lua_getfield(L, -1, "__index");
  lua_pushcfunction(L, lugl_extension_create);
  lua_setfield(L, -2, "Extension");
  lua_pop(L, 2);
}
