#include "luavgl.h"
#include "private.h"

static int luavgl_dropdown_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_dropdown_create);
}

static void _lv_dropdown_set_text(void *obj, lua_State *L)
{
  const char *str = lua_tostring(L, -1);

  lv_dropdown_set_text(obj, str);
}

/* clang-format off */
static const luavgl_value_setter_t dropdown_property_table[] = {
    {"text", SETTER_TYPE_STACK, {.setter_stack = _lv_dropdown_set_text}},
    {"options", SETTER_TYPE_STACK, {.setter_stack = _lv_dummy_set}},
    {"selected", 0, {.setter = (setter_int_t)lv_dropdown_set_selected}},
    {"dir", 0, {.setter = (setter_int_t)lv_dropdown_set_dir}},
    {"symbol", SETTER_TYPE_IMGSRC, {.setter_pointer = (setter_pointer_t)lv_dropdown_set_symbol}},
    {"highlight", 0, {.setter = (setter_int_t)lv_dropdown_set_selected_highlight}},
};
/* clang-format on */

LUALIB_API int luavgl_dropdown_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, dropdown_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    LV_LOG_ERROR("unkown property for dropdown");
  }

  return ret;
}

static int luavgl_dropdown_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

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

  luavgl_iterate(L, -1, luavgl_dropdown_set_property_kv, obj);

  return 0;
}

static int luavgl_dropdown_get(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (lua_type(L, 2) != LUA_TSTRING) {
    return luaL_argerror(L, 2, "expect string");
  }

  const char *key = lua_tostring(L, 2);
  if (lv_strcmp(key, "list") == 0) {
    lv_obj_t *list = lv_dropdown_get_list(obj);
    lua_pushlightuserdata(L, list);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isnoneornil(L, -1)) {
      lua_pop(L, 1);
      luavgl_add_lobj(L, list)->lua_created = false;
    }
    return 1;
  }

  if (lv_strcmp(key, "text") == 0) {
    lua_pushstring(L, lv_dropdown_get_text(obj));
    return 1;
  }

  if (lv_strcmp(key, "options") == 0) {
    lua_pushstring(L, lv_dropdown_get_options(obj));
    return 1;
  }

  if (lv_strcmp(key, "selected") == 0) {
    lua_pushinteger(L, lv_dropdown_get_selected(obj));
    return 1;
  }

  if (lv_strcmp(key, "option_cnt") == 0) {
    lua_pushinteger(L, lv_dropdown_get_option_cnt(obj));
    return 1;
  }

  if (lv_strcmp(key, "selected_str") == 0) {
    char buf[64];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    lua_pushstring(L, buf);
    return 1;
  }

#if LV_VERSION_CHECK(8, 3, 0)
  if (lv_strcmp(key, "option_index") == 0) {
    const char *option = lua_tostring(L, 3);
    lua_pushinteger(L, lv_dropdown_get_option_index(obj, option));
    return 1;
  }
#endif

  if (lv_strcmp(key, "symbol") == 0) {
    lua_pushlightuserdata(L, (void *)lv_dropdown_get_symbol(obj));
    return 1;
  }

  if (lv_strcmp(key, "dir") == 0) {
    lua_pushinteger(L, lv_dropdown_get_dir(obj));
    return 1;
  }

  return 0;
}

static int luavgl_dropdown_open(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_dropdown_open(obj);
  return 0;
}

static int luavgl_dropdown_close(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_dropdown_close(obj);
  return 0;
}

static int luavgl_dropdown_is_open(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushboolean(L, lv_dropdown_is_open(obj));
  return 1;
}

static int luavgl_dropdown_add_option(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  const char *str = lua_tostring(L, 2);
  uint32_t pos = lua_tointeger(L, 3);
  lv_dropdown_add_option(obj, str, pos);

  return 0;
}

static int luavgl_dropdown_clear_option(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  lv_dropdown_clear_options(obj);
  return 0;
}

static const luaL_Reg luavgl_dropdown_methods[] = {
    {"set",          luavgl_dropdown_set         },
    {"get",          luavgl_dropdown_get         },
    {"open",         luavgl_dropdown_open        },
    {"close",        luavgl_dropdown_close       },
    {"is_open",      luavgl_dropdown_is_open     },
    {"add_option",   luavgl_dropdown_add_option  },
    {"clear_option", luavgl_dropdown_clear_option},

    {NULL,           NULL                      },
};

static void luavgl_dropdown_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_dropdown_class, "lv_dropdown",
                        luavgl_dropdown_methods);
  lua_pop(L, 1);
}
