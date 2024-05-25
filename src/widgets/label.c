#include "luavgl.h"
#include "private.h"

static int luavgl_label_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_label_create);
}

static void _lv_label_set_txt(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "only support string for text.");
    return;
  }

  lv_label_set_text(obj, lua_tostring(L, -1));
}

static const luavgl_value_setter_t label_property_table[] = {
    {"text",                 SETTER_TYPE_STACK, {.setter_stack = _lv_label_set_txt}                        },
    {"long_mode",            0,                 {.setter = (setter_int_t)lv_label_set_long_mode}           },
#if LVGL_VERSION_MAJOR == 9
    {"text_selection_start",
     0,                                         {.setter = (setter_int_t)lv_label_set_text_selection_start}},
    {"text_selection_end",
     0,                                         {.setter = (setter_int_t)lv_label_set_text_selection_end}  },
#else
    {"text_selection_start",
     0,
     {.setter = (setter_int_t)lv_label_set_text_sel_start}},
    {"text_selection_end",
     0,
     {.setter = (setter_int_t)lv_label_set_text_sel_end}},
#endif
};

LUALIB_API int luavgl_label_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, label_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    LV_LOG_ERROR("unkown property for label");
  }

  return ret;
}

static int luavgl_label_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_label_set_property_kv, obj);

  return 0;
}

static int luavgl_label_get_text(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushstring(L, lv_label_get_text(obj));
  return 1;
}

static int luavgl_label_get_long_mode(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushinteger(L, lv_label_get_long_mode(obj));
  return 1;
}

static int luavgl_label_ins_text(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  uint32_t pos = luavgl_tointeger(L, 2);
  const char *txt = lua_tostring(L, 3);

  lv_label_ins_text(obj, pos, txt);
  return 0;
}

static int luavgl_label_cut_text(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  uint32_t pos = luavgl_tointeger(L, 2);
  uint32_t cnt = luavgl_tointeger(L, 3);

  lv_label_cut_text(obj, pos, cnt);
  return 0;
}

/* demo purpose, there is no need to use set_text_static */
static int luavgl_label_set_text_static(lua_State *L)
{
  const char *str = lua_tostring(L, 2);
  luavgl_obj_t *lobj = luavgl_to_lobj(L, 1);
  if (lobj->obj == NULL) {
    return luaL_error(L, "obj null.");
  }

  luavgl_obj_getuserdatauv(L, 1);

  /* uservalue is on top */
  lua_pushvalue(L, 2);
  lua_setfield(L, -2, "text_static");

  lv_label_set_text_static(lobj->obj, str);
  return 0;
}

static int luavgl_label_tostring(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushfstring(L, "lv_label:%p, text: %s", obj, lv_label_get_text(obj));
  return 1;
}

static const luaL_Reg luavgl_label_methods[] = {
    {"set",             luavgl_label_set            },
    {"set_text_static", luavgl_label_set_text_static},
    {"get_text",        luavgl_label_get_text       },
    {"get_long_mode",   luavgl_label_get_long_mode  },
    {"ins_text",        luavgl_label_ins_text       },
    {"cut_text",        luavgl_label_cut_text       },

    {NULL,              NULL                      },
};

static void luavgl_label_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_label_class, "lv_label", luavgl_label_methods);
  lua_pushcfunction(L, luavgl_label_tostring);
  lua_setfield(L, -2, "__tostring");
  lua_pop(L, 1);
}
