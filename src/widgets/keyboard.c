#include "luavgl.h"
#include "private.h"

static int luavgl_keyboard_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_keyboard_create);
}

static void _lv_keyboard_set_textarea(void *obj, lua_State *L)
{
  lv_obj_t *ta = luavgl_to_obj(L, -1);
  if (ta->class_p != &lv_textarea_class) {
    luaL_argerror(L, -1, "expect textarea obj");
    return;
  }
  lv_keyboard_set_textarea(obj, ta);
}

/* clang-format off */
static const luavgl_value_setter_t keyboard_property_table[] = {
    {"textarea", SETTER_TYPE_STACK, {.setter_stack = _lv_keyboard_set_textarea}},
    {"mode", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_keyboard_set_mode}},
    {"popovers", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_keyboard_set_popovers}},
};
/* clang-format on */

LUALIB_API int luavgl_keyboard_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, keyboard_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    LV_LOG_ERROR("unkown property for keyboard");
  }

  return ret;
}

static int luavgl_keyboard_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_keyboard_set_property_kv, obj);

  return 0;
}

static const luaL_Reg luavgl_keyboard_methods[] = {
    {"set", luavgl_keyboard_set},

    {NULL,  NULL             },
};

static void luavgl_keyboard_init(lua_State *L)
{
  static const luaL_Reg btm_methods[] = {
      {NULL, NULL},
  };
  luavgl_obj_newmetatable(L, &lv_buttonmatrix_class, "lv_btnm", btm_methods);
  lua_pop(L, 1);

  luavgl_obj_newmetatable(L, &lv_keyboard_class, "lv_keyboard",
                        luavgl_keyboard_methods);
  lua_pop(L, 1);
}
