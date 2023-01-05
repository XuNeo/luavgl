#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_list_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_list_create);
}

/* clang-format off */
static const lugl_value_setter_t list_property_table[] = {
    {"dummy", SETTER_TYPE_STACK, {.setter_stack = _lv_dummy_set}},
};
/* clang-format on */

static int lugl_list_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, list_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for list.\n");
  }

  return -1;
}

static int lugl_list_set(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  lugl_iterate(L, -1, lugl_list_set_property_kv, obj);

  return 0;
}

static int lugl_list_add_text(lua_State *L)
{
  lv_obj_t *list = lugl_check_obj(L, 1);
  const char *str = lua_tostring(L, 2);
  lv_obj_t *obj = lv_list_add_text(list, str);
  lugl_add_lobj(L, obj)->lua_created = true;
  return 1;
}

static int lugl_list_add_btn(lua_State *L)
{
  lv_obj_t *list = lugl_check_obj(L, 1);
  const void *icon = lugl_toimgsrc(L, 2);
  const char *str = lua_tostring(L, 3);
  lv_obj_t *obj = lv_list_add_btn(list, icon, str);
  lugl_add_lobj(L, obj)->lua_created = true;
  return 1;
}

static int lugl_get_btn_text(lua_State *L)
{
  lv_obj_t *list = lugl_check_obj(L, 1);
  lv_obj_t *btn = lugl_check_obj(L, 2);

  lua_pushstring(L, lv_list_get_btn_text(list, btn));
  return 1;
}

static const luaL_Reg lugl_list_methods[] = {
    {"set", lugl_list_set},

    {"add_text", lugl_list_add_text},
    {"add_btn", lugl_list_add_btn},
    {"get_btn_text", lugl_get_btn_text},

    {NULL, NULL},
};

static void lugl_list_init(lua_State *L)
{
  lugl_obj_newmetatable(L, &lv_list_class, "lv_list");

  lugl_new_objlib(L);
  luaL_setfuncs(L, lugl_list_methods, 0);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
