#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_dropdown_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_dropdown_create);
}

static void _lv_dropdown_set_text(void *obj, lua_State *L)
{
  const char *str = lua_tostring(L, -1);

  lv_dropdown_set_text(obj, str);
}

/* clang-format off */
static const lugl_value_setter_t dropdown_property_table[] = {
    {"text", SETTER_TYPE_STACK, {.setter_stack = _lv_dropdown_set_text}},
    {"options", SETTER_TYPE_STACK, {.setter_stack = _lv_dummy_set}},
    {"selected", 0, {.setter = (setter_int_t)lv_dropdown_set_selected}},
    {"dir", 0, {.setter = (setter_int_t)lv_dropdown_set_dir}},
    {"symbol", SETTER_TYPE_IMGSRC, {.setter_pointer = (setter_pointer_t)lv_dropdown_set_symbol}},
    {"highlight", 0, {.setter = (setter_int_t)lv_dropdown_set_selected_highlight}},
};
/* clang-format on */

static int lugl_dropdown_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, dropdown_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for dropdown.\n");
  }

  return -1;
}

static int lugl_dropdown_set(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been deleted.");
    return 0;
  }

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  /* set options firstly, otherwise set selected may fail */
  lua_getfield(L, -1, "options");
  if (lua_type(L, -1) == LUA_TSTRING) {
    lv_dropdown_set_options(obj, lua_tostring(L, -1));
  }
  lua_pop(L, 1);

  lugl_iterate(L, -1, lugl_dropdown_set_property_kv, obj);

  return 0;
}

static int lugl_dropdown_get(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been deleted.");
    return 0;
  }

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_argerror(L, 2, "expect string");
  }

  const char *key = lua_tostring(L, 2);
  if (strcmp(key, "list") == 0) {
    lv_obj_t *list = lv_dropdown_get_list(obj);
    lua_pushlightuserdata(L, list);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isnoneornil(L, -1)) {
      lua_pop(L, 1);
      lugl_add_lobj(L, list)->lua_created = false;
    }
    return 1;
  }

  if (strcmp(key, "text") == 0) {
    lua_pushstring(L, lv_dropdown_get_text(obj));
    return 1;
  }

  if (strcmp(key, "options") == 0) {
    lua_pushstring(L, lv_dropdown_get_options(obj));
    return 1;
  }

  if (strcmp(key, "selected") == 0) {
    lua_pushinteger(L, lv_dropdown_get_selected(obj));
    return 1;
  }

  if (strcmp(key, "option_cnt") == 0) {
    lua_pushinteger(L, lv_dropdown_get_option_cnt(obj));
    return 1;
  }

  if (strcmp(key, "selected_str") == 0) {
    char buf[64];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    lua_pushstring(L, buf);
    return 1;
  }

  if (strcmp(key, "option_index") == 0) {
    const char* option = lua_tostring(L, 3);
    lua_pushinteger(L, lv_dropdown_get_option_index(obj, option));
    return 1;
  }

  if (strcmp(key, "symbol") == 0) {
    lua_pushlightuserdata(L, (void*)lv_dropdown_get_symbol(obj));
    return 1;
  }

  if (strcmp(key, "dir") == 0) {
    lua_pushinteger(L, lv_dropdown_get_dir(obj));
    return 1;
  }

  return 0;
}

static int lugl_dropdown_open(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lv_dropdown_open(obj);
  return 0;
}

static int lugl_dropdown_close(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lv_dropdown_close(obj);
  return 0;
}

static int lugl_dropdown_is_open(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lua_pushboolean(L, lv_dropdown_is_open(obj));
  return 1;
}

static int lugl_dropdown_add_option(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  const char *str = lua_tostring(L, 2);
  uint32_t pos = lua_tointeger(L, 3);
  lv_dropdown_add_option(obj, str, pos);

  return 0;
}

static int lugl_dropdown_clear_option(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);

  lv_dropdown_clear_options(obj);
  return 0;
}

static const luaL_Reg lugl_dropdown_methods[] = {
    // dropdown.c
    {"set", lugl_dropdown_set},
    {"get", lugl_dropdown_get},
    {"open", lugl_dropdown_open},
    {"close", lugl_dropdown_close},
    {"is_open", lugl_dropdown_is_open},
    {"add_option", lugl_dropdown_add_option},
    {"clear_option", lugl_dropdown_clear_option},

    {NULL, NULL},
};

static void lugl_dropdown_init(lua_State *L)
{
  lugl_obj_newmetatable(L, &lv_dropdown_class, "lv_dropdown");
  lugl_new_objlib(L);
  luaL_setfuncs(L, lugl_dropdown_methods, 0);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
