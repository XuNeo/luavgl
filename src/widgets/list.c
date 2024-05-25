#include "luavgl.h"
#include "private.h"

static int luavgl_list_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_list_create);
}

/* clang-format off */
static const luavgl_value_setter_t list_property_table[] = {
    {"dummy", SETTER_TYPE_STACK, {.setter_stack = _lv_dummy_set}},
};
/* clang-format on */

LUALIB_API int luavgl_list_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, list_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    LV_LOG_ERROR("unkown property for list");
  }

  return ret;
}

static int luavgl_list_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_list_set_property_kv, obj);

  return 0;
}

static int luavgl_list_add_text(lua_State *L)
{
  lv_obj_t *list = luavgl_to_obj(L, 1);
  const char *str = lua_tostring(L, 2);
  lv_obj_t *obj = lv_list_add_text(list, str);
  luavgl_add_lobj(L, obj)->lua_created = true;
  return 1;
}

static int luavgl_list_add_btn(lua_State *L)
{
  lv_obj_t *list = luavgl_to_obj(L, 1);
  const void *icon = luavgl_toimgsrc(L, 2);
  const char *str = lua_tostring(L, 3);
  lv_obj_t *obj = lv_list_add_btn(list, icon, str);
  luavgl_add_lobj(L, obj)->lua_created = true;
  return 1;
}

static int luavgl_get_btn_text(lua_State *L)
{
  lv_obj_t *list = luavgl_to_obj(L, 1);
  lv_obj_t *btn = luavgl_to_obj(L, 2);

  lua_pushstring(L, lv_list_get_btn_text(list, btn));
  return 1;
}

static const luaL_Reg luavgl_list_methods[] = {
    {"set",          luavgl_list_set     },

    {"add_text",     luavgl_list_add_text},
    {"add_btn",      luavgl_list_add_btn },
    {"get_btn_text", luavgl_get_btn_text },

    {NULL,           NULL              },
};

static void luavgl_list_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_list_class, "lv_list", luavgl_list_methods);
  lua_pop(L, 1);
}
