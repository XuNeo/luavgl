#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_label_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_label_create);
}

static void _lv_label_set_txt(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "only support string for text.");
    return;
  }

  lv_label_set_text(obj, lua_tostring(L, -1));
}

static const lugl_value_setter_t label_property_table[] = {
    {"text", SETTER_TYPE_STACK, {.setter_stack = _lv_label_set_txt}},
    {"long_mode", 0, {.setter = (setter_int_t)lv_label_set_long_mode}},
    {"recolor", 0, {.setter = (setter_int_t)lv_label_set_recolor}},
    {"text_selection_start",
     0,
     {.setter = (setter_int_t)lv_label_set_text_selection_start}},
    {"text_selection_end",
     0,
     {.setter = (setter_int_t)lv_label_set_text_selection_end}},
};

static int label_set_property_cb(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, label_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for label.\n");
  }

  return -1;
}


static int lugl_label_set(lua_State *L)
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

  lugl_iterate(L, -1, label_set_property_cb, obj);

  return 0;
}

static int lugl_label_get_text(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lua_pushstring(L, lv_label_get_text(obj));
  return 1;
}

static int lugl_label_get_long_mode(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lua_pushinteger(L, lv_label_get_long_mode(obj));
  return 1;
}

static int lugl_label_get_recolor(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lua_pushinteger(L, lv_label_get_recolor(obj));
  return 1;
}

static int lugl_label_ins_text(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  uint32_t pos = lugl_tointeger(L, 2);
  const char *txt = lua_tostring(L, 3);

  lv_label_ins_text(obj, pos, txt);
  return 0;
}

static int lugl_label_cut_text(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  uint32_t pos = lugl_tointeger(L, 2);
  uint32_t cnt = lugl_tointeger(L, 3);

  lv_label_cut_text(obj, pos, cnt);
  return 0;
}

static const luaL_Reg lugl_label_methods[] = {
    // label.c
    {"set", lugl_label_set},
    {"get_text", lugl_label_get_text},
    {"get_long_mode", lugl_label_get_long_mode},
    {"get_recolor", lugl_label_get_recolor},
    {"ins_text", lugl_label_ins_text},
    {"cut_text", lugl_label_cut_text},

    {NULL, NULL},
};

static void lugl_label_init(lua_State *L)
{
  luaL_newmetatable(L, "lv_label");

  lugl_new_objlib(L);
  luaL_setfuncs(L, lugl_label_methods, 0);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
