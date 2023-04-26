#include "luavgl.h"
#include "private.h"

static int luavgl_textarea_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_textarea_create);
}

static void _lv_textarea_set_txt(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "expect string");
    return;
  }

  lv_textarea_set_text(obj, lua_tostring(L, -1));
}

static void _lv_textarea_set_placeholder_txt(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "expect string");
    return;
  }

  lv_textarea_set_placeholder_text(obj, lua_tostring(L, -1));
}

static void _lv_textarea_set_password_bullet(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "expect string");
    return;
  }

  lv_textarea_set_password_bullet(obj, lua_tostring(L, -1));
}

static void _lv_textarea_set_accepted_chars(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "expect string");
    return;
  }

  lv_textarea_set_accepted_chars(obj, lua_tostring(L, -1));
}

/* clang-format off */
static const luavgl_value_setter_t textarea_property_table[] = {
    {"text", SETTER_TYPE_STACK, {.setter_stack = _lv_textarea_set_txt}},
    {"placeholder", SETTER_TYPE_STACK, {.setter_stack = _lv_textarea_set_placeholder_txt}},
    {"cursor", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_textarea_set_cursor_pos}},
    {"password_mode", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_textarea_set_password_mode}},
    {"one_line", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_textarea_set_one_line}},
    {"password_bullet", SETTER_TYPE_STACK, {.setter_stack = _lv_textarea_set_password_bullet}},
    {"accepted_chars", SETTER_TYPE_STACK, {.setter_stack = _lv_textarea_set_accepted_chars}},
    {"max_length", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_textarea_set_max_length}},
    {"password_show_time", SETTER_TYPE_INT, {.setter = (setter_int_t)lv_textarea_set_password_show_time}},
};
/* clang-format on */

LUALIB_API int luavgl_textarea_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, textarea_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for textarea: %s\n", lua_tostring(L, -2));
  }

  return -1;
}

static int luavgl_textarea_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_textarea_set_property_kv, obj);

  return 0;
}

/**
 * obj:get_text()
 */
static int luavgl_textarea_get_text(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushstring(L, lv_textarea_get_text(obj));
  return 1;
}

static const luaL_Reg luavgl_textarea_methods[] = {
    {"set",      luavgl_textarea_set     },
    {"get_text", luavgl_textarea_get_text},

    {NULL,       NULL                  },
};

static void luavgl_textarea_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_textarea_class, "lv_textarea",
                        luavgl_textarea_methods);
  lua_pop(L, 1);
}
