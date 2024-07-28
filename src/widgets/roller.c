#include "luavgl.h"
#include "private.h"

static int luavgl_roller_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_roller_create);
}

static int _lv_roller_set_options(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    int type = lua_type(L, -1);
    int top = lua_gettop(L);
    if (type == LUA_TSTRING) {
      lv_roller_set_options(obj, lua_tostring(L, -1), 0);
    } else if (type == LUA_TTABLE) {
      lua_getfield(L, -1, "options");
      if (!lua_isstring(L, -1)) {
        luaL_error(L, "expect string.");
        return 1;
      }

      const char *options = lua_tostring(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "mode");
      lv_roller_mode_t mode = lua_tointeger(L, -1);
      lua_pop(L, 1);
      lv_roller_set_options(obj, options, mode);
      lua_settop(L, top);
      return 1;
    }

    luaL_error(L, "expect string or table.");
  } else {
    lua_pushstring(L, lv_roller_get_options(obj));
  }

  return 1;
}

static int _lv_roller_set_selected(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    int type = lua_type(L, -1);
    uint16_t selected;
    int anim = 0;
    int top = lua_gettop(L);

    if (type == LUA_TTABLE) {
      lua_getfield(L, -1, "selected");
      selected = lua_tointeger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "anim");
      anim = luavgl_tointeger(L, -1);
    } else {
      selected = lua_tointeger(L, -1);
    }

    lv_roller_set_selected(obj, selected, anim);
    lua_settop(L, top);
  } else {
    lua_pushinteger(L, lv_roller_get_selected(obj));
  }

  return 1;
}

static const luavgl_property_ops_t roller_property_ops[] = {
    {.name = "options",  .ops = _lv_roller_set_options },
    {.name = "selected", .ops = _lv_roller_set_selected},
};

static const luavgl_table_t roller_property_table = {
    .len = sizeof(roller_property_ops) / sizeof(roller_property_ops[0]),
    .array = roller_property_ops,
};

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
  lua_pushinteger(L, lv_roller_get_option_count(obj));
  return 1;
}

static const rotable_Reg luavgl_roller_methods[] = {
    {"get_selected_str", LUA_TFUNCTION,      {luavgl_roller_get_selected_str}},
    {"get_options_cnt",  LUA_TFUNCTION,      {luavgl_roller_get_options_cnt} },

    {"__property",       LUA_TLIGHTUSERDATA, {.ptr = &roller_property_table} },
    {0,                  0,                  {0}                             },
};

static void luavgl_roller_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_roller_class, "lv_roller",
                          luavgl_roller_methods);
  lua_pop(L, 1);
}
