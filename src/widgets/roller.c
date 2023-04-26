#include "luavgl.h"
#include "private.h"

static int luavgl_roller_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_roller_create);
}

static void _lv_roller_set_options(void *obj, lua_State *L)
{
  int type = lua_type(L, -1);
  if (type == LUA_TSTRING) {
    lv_roller_set_options(obj, lua_tostring(L, -1), 0);
  } else if (type == LUA_TTABLE) {
    lua_getfield(L, -1, "options");
    if (!lua_isstring(L, -1)) {
      luaL_error(L, "expect string.");
      return;
    }

    const char *options = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "mode");
    lv_roller_mode_t mode = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lv_roller_set_options(obj, options, mode);
    return;
  }

  luaL_error(L, "expect string or table.");
}

static void _lv_roller_set_selected(void *obj, lua_State *L)
{
  int type = lua_type(L, -1);
  uint16_t selected;
  int anim = 0;

  if (type == LUA_TTABLE) {
    lua_getfield(L, -1, "selected");
    selected = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "anim");
    anim = luavgl_tointeger(L, -1);
    lua_pop(L, 1);
  } else {
    selected = lua_tointeger(L, -1);
  }

  lv_roller_set_selected(obj, selected, anim);
}

static const luavgl_value_setter_t roller_property_table[] = {
    {"options",     SETTER_TYPE_STACK, {.setter_stack = _lv_roller_set_options}                 },
    {"selected",    SETTER_TYPE_STACK, {.setter_stack = _lv_roller_set_selected}                },
    {"visible_cnt",
     0,                                {.setter = (setter_int_t)lv_roller_set_visible_row_count}},
};

LUALIB_API int luavgl_roller_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, roller_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for roller.\n");
  }

  return ret;
}

static int luavgl_roller_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_roller_set_property_kv, obj);

  return 0;
}

static int luavgl_roller_get_options(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushstring(L, lv_roller_get_options(obj));
  return 1;
}

static int luavgl_roller_get_selected(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushinteger(L, lv_roller_get_selected(obj));
  return 1;
}

static int luavgl_roller_get_selected_str(lua_State *L)
{
  char buf[64];
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_roller_get_selected_str(obj, buf, sizeof(buf));
  lua_pushstring(L, buf);
  return 1;
}

static int luavgl_roller_get_options_cnt(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushinteger(L, lv_roller_get_option_cnt(obj));
  return 1;
}

static const luaL_Reg luavgl_roller_methods[] = {
    {"set",              luavgl_roller_set             },
    {"get_options",      luavgl_roller_get_options     },
    {"get_selected",     luavgl_roller_get_selected    },
    {"get_selected_str", luavgl_roller_get_selected_str},
    {"get_options_cnt",  luavgl_roller_get_options_cnt },

    {NULL,               NULL                        },
};

static void luavgl_roller_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_roller_class, "lv_roller", luavgl_roller_methods);
  lua_pop(L, 1);
}
