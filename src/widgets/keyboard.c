#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_keyboard_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_keyboard_create);
}

static void _lv_keyboard_set_textarea(void *obj, lua_State *L)
{
  lv_obj_t *ta = lugl_to_obj(L, -1);
  if (ta->class_p != &lv_textarea_class) {
    luaL_argerror(L, -1, "expect textarea obj");
    return;
  }
  lv_keyboard_set_textarea(obj, ta);
}

/* clang-format off */
static const lugl_value_setter_t keyboard_property_table[] = {
    {"textarea", SETTER_TYPE_STACK, {.setter_stack = _lv_keyboard_set_textarea}},
    {"mode", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_keyboard_set_mode}},
    {"popovers", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_keyboard_set_popovers}},
};
/* clang-format on */

static int lugl_keyboard_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, keyboard_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for keyboard.\n");
  }

  return -1;
}

static int lugl_keyboard_set(lua_State *L)
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

  lugl_iterate(L, -1, lugl_keyboard_set_property_kv, obj);

  return 0;
}

static const luaL_Reg lugl_keyboard_methods[] = {
    // widget/textarea.c
    {"set", lugl_keyboard_set},

    {NULL, NULL},
};

static void lugl_keyboard_init(lua_State *L)
{
  static const luaL_Reg btm_methods[] = {
      {NULL, NULL},
  };
  lugl_obj_newmetatable(L, &lv_btnmatrix_class, "lv_btnm", btm_methods);
  lua_pop(L, 1);

  lugl_obj_newmetatable(L, &lv_keyboard_class, "lv_keyboard",
                        lugl_keyboard_methods);
  lua_pop(L, 1);
}
