#include <lauxlib.h>
#include <lua.h>

#include "lugl.h"
#include "private.h"

/**
 * check object class and return metatable name.
 */
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
    return "lv_obj";
  else if (obj->class_p == &lv_list_class)
    return "lv_list";
  else if (obj->class_p == &lv_checkbox_class)
    return "lv_checkbox";
  else if (obj->class_p == &lv_calendar_class)
    return "lv_calendar";
  else if (obj->class_p == &lv_calendar_header_arrow_class)
    return "lv_obj";
  else if (obj->class_p == &lv_calendar_header_dropdown_class)
    return "lv_obj";
  else if (obj->class_p == &lv_btnmatrix_class)
    return "lv_obj"; /* needed, but not supported, so return lv_obj instead */
  else
    return NULL;
}

static lugl_obj_t *lugl_to_lobj(lua_State *L, int idx)
{
  lugl_obj_t *lobj = lua_touserdata(L, idx);
  if (lobj == NULL) {
    goto fail;
  }

  if (lobj->obj == NULL) {
    /* could be already deleted, but not gc'ed */
    return NULL;
  }

  return lobj;

fail:
  debug("arg not lv_obj userdata.\n");
  luaL_argerror(L, idx, "Expected lv_obj userdata");
  return NULL;
}

static lv_obj_t *lugl_check_obj(lua_State *L, int idx)
{
  lugl_obj_t *lobj = lugl_to_lobj(L, idx);
  if (lobj == NULL)
    return NULL;

  return lobj->obj;
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

  luaL_argerror(L, index, "function or callable table expected");
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
      p->setter_pointer(obj, (void *)lugl_toimgsrc(L, -1));
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
  int v = 0;
  if (lua_isboolean(L, idx)) {
    v = lua_toboolean(L, idx);
  } else if (lua_isinteger(L, idx)) {
    v = lua_tointeger(L, idx);
  } else {
    v = lua_tonumber(L, idx);
  }

  return v;
}

static lv_color_t lugl_tocolor(lua_State *L, int idx)
{
  lv_color_t color = {0};
  if (lua_type(L, idx) == LUA_TSTRING) {
    /* support #RGB and #RRGGBB */
    const char *s = lua_tostring(L, idx);
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

static const char *lugl_toimgsrc(lua_State *L, int idx)
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

static void dumpvalue(lua_State *L, int i, bool cr)
{
  const char ending = cr ? '\n' : '\0';
  switch (lua_type(L, i)) {
  case LUA_TNUMBER:
    printf("number: %g%c", lua_tonumber(L, i), ending);
    break;
  case LUA_TSTRING:
    printf("string: %s%c", lua_tostring(L, i), ending);
    break;
  case LUA_TBOOLEAN:
    printf("boolean: %s%c", (lua_toboolean(L, i) ? "true" : "false"), ending);
    break;
  case LUA_TNIL:
    printf("nil: %s%c", "nil", ending);
    break;
  default:
    printf("pointer: %p%c", lua_topointer(L, i), ending);
    break;
  }
}

static void dumptable(lua_State *L, int index)
{
  int i = index < 0 ? index - 1 : index;
  lua_pushnil(L); /* nil as initial key to iterate through table */
  while (lua_next(L, i)) {
    /* -1: value, -2: key */
    dumpvalue(L, -2, 0);
    dumpvalue(L, -1, 1);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }
  fflush(stdout);
}