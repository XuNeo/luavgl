#include "luavgl.h"
#include "private.h"

static int luavgl_checkbox_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_checkbox_create);
}

static void _lv_checkbox_set_txt(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "only support string for text.");
    return;
  }

  lv_checkbox_set_text(obj, lua_tostring(L, -1));
}

static const luavgl_value_setter_t checkbox_property_table[] = {
    {"text", SETTER_TYPE_STACK, {.setter_stack = _lv_checkbox_set_txt}},
};

LUALIB_API int luavgl_checkbox_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, checkbox_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for checkbox.\n");
  }

  return ret;
}

static int luavgl_checkbox_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_checkbox_set_property_kv, obj);

  return 0;
}

static int luavgl_checkbox_get_text(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushstring(L, lv_checkbox_get_text(obj));
  return 1;
}

static const luaL_Reg luavgl_checkbox_methods[] = {
    {"set",      luavgl_checkbox_set     },
    {"get_text", luavgl_checkbox_get_text},

    {NULL,       NULL                  },
};

static void luavgl_checkbox_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_checkbox_class, "lv_checkbox",
                        luavgl_checkbox_methods);
  lua_pop(L, 1);
}
