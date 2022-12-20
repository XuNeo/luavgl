#include <lauxlib.h>
#include <lua.h>

#include "lugl.h"
#include "private.h"

static const char *lugl_class_to_metatable_name(lv_obj_t *obj)
{
  if (obj->class_p == &lv_obj_class)
    return "lv_obj";
  else if (obj->class_p == &lv_img_class)
    return "lv_img";
  else if (obj->class_p == &lv_label_class)
    return "lv_label";
  else if (obj->class_p == &lv_textarea_class)
    return "lv_textarea";
  else if (obj->class_p == &lv_keyboard_class)
    return "lv_keyboard";
  else if (obj->class_p == &lv_led_class)
    return "lv_led";
  else if (obj->class_p == &lv_list_text_class)
    return "lv_label";
  else if (obj->class_p == &lv_list_btn_class)
    return "lv_btn";
  else if (obj->class_p == &lv_list_class)
    return "lv_list";
  else
    return NULL;
}

static int lugl_arg_type_error(lua_State *L, int index, const char *fmt)
{
  const char *msg;
  const char *typearg; /* name for the type of the actual argument */
  if (luaL_getmetafield(L, index, "__name") == LUA_TSTRING)
    typearg = lua_tostring(L, -1); /* use the given type name */
  else if (lua_type(L, index) == LUA_TLIGHTUSERDATA)
    typearg = "light userdata"; /* special name for messages */
  else
    typearg = luaL_typename(L, index); /* standard name */
  msg = lua_pushfstring(L, fmt, typearg);
  return luaL_argerror(L, index, msg);
}

static int lugl_is_callable(lua_State *L, int index)
{
  if (luaL_getmetafield(L, index, "__call") != LUA_TNIL) {
    // getmetatable(x).__call must be a function for x() to work
    int callable = lua_isfunction(L, -1);
    lua_pop(L, 1);
    return callable;
  }
  return lua_isfunction(L, index);
}

static void lugl_check_callable(lua_State *L, int index)
{
  if (lugl_is_callable(L, index))
    return;

  lugl_arg_type_error(L, index, "function or callable table expected, got %s");
}

static int lugl_check_continuation(lua_State *L, int index)
{
  if (lua_isnoneornil(L, index))
    return LUA_NOREF;
  lugl_check_callable(L, index);
  lua_pushvalue(L, index);
  return luaL_ref(L, LUA_REGISTRYINDEX);
}

static int _lugl_set_property(lua_State *L, void *obj,
                              const lugl_value_setter_t table[], uint32_t len)
{
  const char *key = lua_tostring(L, -2);
  if (key == NULL) {
    debug("Null key, ignored.\n");
    return -1;
  }

  for (int i = 0; i < len; i++) {
    const lugl_value_setter_t *p = &table[i];
    if (strcmp(key, p->key))
      continue;

    if (p->type == SETTER_TYPE_INT) {
      int v;
      if (lua_isboolean(L, -1)) {
        v = lua_toboolean(L, -1);
      } else if (lua_isinteger(L, -1)) {
        v = lua_tointeger(L, -1);
      } else {
        v = lua_tonumber(L, -1);
      }
      p->setter(obj, v);
    } else if (p->type == SETTER_TYPE_COLOR) {
      /* color */
      lv_color_t color = lugl_tocolor(L, -1);
      p->setter(obj, color.full);
    } else if (p->type == SETTER_TYPE_IMGSRC) {
      /* img src */
      p->setter_pointer(obj, (void *)lugl_get_img_src(L, -1));
    } else if (p->type == SETTER_TYPE_STACK) {
      p->setter_stack(obj, L);
    } else if (p->type == SETTER_TYPE_POINTER) {
      void *data = lua_touserdata(L, -1);
      p->setter_pointer(obj, data);
    } else {
      debug("unsupported type: %d\n", p->type);
    }
    return 0;
  }

  return -1; /* property not found */
}

static void lugl_iterate(lua_State *L, int index,
                         int (*cb)(lua_State *, void *), void *cb_para)
{
  int i = index < 0 ? index - 1 : index;
  lua_pushnil(L); /* nil as initial key to iterate through table */
  while (lua_next(L, i)) {
    /* -1: value, -2: key */
    if (!lua_isstring(L, -2)) {
      /* we expect string as key, ignore it if not */
      debug("ignore non-string key in table.\n");
      lua_pop(L, 1);
      continue;
    }

    cb(L, cb_para);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }
}

static void _lv_dummy_set(void *obj, lua_State *L)
{
  //
}

static int lugl_tointeger(lua_State *L, int idx)
{
  int v;
  if (lua_isboolean(L, -1)) {
    v = lua_toboolean(L, -1);
  }
  if (lua_isinteger(L, -1)) {
    v = lua_tointeger(L, -1);
  } else {
    v = lua_tonumber(L, -1);
  }

  return v;
}

static lv_color_t lugl_tocolor(lua_State *L, int idx)
{
  lv_color_t color = {0};
  if (lua_type(L, idx) == LUA_TSTRING) {
    /* support #RGB and #RRGGBB */
    const char *s = lua_tostring(L, -1);
    if (s == NULL) {
      luaL_error(L, "unknown color.");
      return color;
    }

    int len = strlen(s);
    if (len == 4 && s[0] == '#') {
      /* #RGB */
      int r = to_int(s[1]);
      r |= r << 4;
      int g = to_int(s[2]);
      g |= g << 4;
      int b = to_int(s[3]);
      b |= b << 4;
      color = lv_color_make(r, g, b);
    } else if (len == 7 && s[0] == '#') {
      /* #RRGGBB */
      int r = (to_int(s[1]) << 4) | to_int(s[2]);
      int g = (to_int(s[3]) << 4) | to_int(s[4]);
      int b = (to_int(s[5]) << 4) | to_int(s[6]);
      color = lv_color_make(r, g, b);
    } else {
      luaL_error(L, "unknown color format.");
      return color;
    }
  } else {
    color = lv_color_hex(lugl_tointeger(L, idx)); /* make to lv_color_t */
  }

  return color;
}

static const char *lugl_get_img_src(lua_State *L, int idx)
{
  const char *src = NULL;
  if (lua_isuserdata(L, idx)) {
    src = lua_touserdata(L, idx);
    debug("set img src to user data: %p\n", src);
  } else if (lua_isstring(L, idx)) {
    src = lua_tostring(L, idx);
  } else {
    debug("img src should be string or userdata.\n");
    return NULL;
  }

  return src;
}
