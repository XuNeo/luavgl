#include "luavgl.h"
#include "private.h"
#include "rotable.h"

static int luavgl_dropdown_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_dropdown_create);
}

static int luavgl_dropdown_open(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_dropdown_open(obj);
  return 1;
}

static int luavgl_dropdown_close(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_dropdown_close(obj);
  return 1;
}

static int luavgl_dropdown_add_option(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  const char *str = lua_tostring(L, 2);
  uint32_t pos = lua_tointeger(L, 3);
  lv_dropdown_add_option(obj, str, pos);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_dropdown_clear_option(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  lv_dropdown_clear_options(obj);
  return 1;
}

static int luavgl_dropdown_get_option_index(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  int i = lv_dropdown_get_option_index(obj, lua_tostring(L, 2));
  lua_pushinteger(L, i);

  return 1;
}

static int dropdown_selected_str(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    return luaL_error(L, "cannot set selected_str");
  }

  char buf[32];
  lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
  lua_pushstring(L, buf);
  return 1;
}

static const luavgl_property_ops_t dropdown_property_ops[] = {
    {.name = "selected_str", .ops = dropdown_selected_str},
};

static const luavgl_table_t
    dropdown_property_table = {
        .len = sizeof(dropdown_property_ops) / sizeof(dropdown_property_ops[0]),
        .array = dropdown_property_ops,
};

static const rotable_Reg luavgl_dropdown_methods[] = {
    {"open",         LUA_TFUNCTION,      {luavgl_dropdown_open}            },
    {"close",        LUA_TFUNCTION,      {luavgl_dropdown_close}           },
    {"add_option",   LUA_TFUNCTION,      {luavgl_dropdown_add_option}      },
    {"clear_option", LUA_TFUNCTION,      {luavgl_dropdown_clear_option}    },
    {"option_index", LUA_TFUNCTION,      {luavgl_dropdown_get_option_index}},
    {"__property",   LUA_TLIGHTUSERDATA, {.ptr = &dropdown_property_table} },

    {0,              0,                  {0}                               },
};

static void luavgl_dropdown_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_dropdown_class, "lv_dropdown",
                          luavgl_dropdown_methods);
  lua_pop(L, 1);
}
