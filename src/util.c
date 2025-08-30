#include "luavgl.h"
#include "private.h"
#include "rotable.h"

LUALIB_API luavgl_obj_t *luavgl_to_lobj(lua_State *L, int idx)
{
  luavgl_obj_t *lobj = lua_touserdata(L, idx);
  if (lobj == NULL) {
    goto fail;
  }

  if (lobj->obj == NULL) {
    /* could be already deleted, but not gc'ed */
    return NULL;
  }

  return lobj;

fail:
  LV_LOG_ERROR("arg not lv_obj userdata");
  luaL_argerror(L, idx, "Expected lv_obj userdata");
  return NULL;
}

LUALIB_API int luavgl_is_callable(lua_State *L, int index)
{
  if (luaL_getmetafield(L, index, "__call") != LUA_TNIL) {
    /* getmetatable(x).__call must be a function for x() to work */
    int callable = lua_isfunction(L, -1);
    lua_pop(L, 1);
    return callable;
  }
  return lua_isfunction(L, index);
}

static void luavgl_check_callable(lua_State *L, int index)
{
  if (luavgl_is_callable(L, index))
    return;

  luaL_argerror(L, index, "function or callable table expected");
}

static int luavgl_check_continuation(lua_State *L, int index)
{
  if (lua_isnoneornil(L, index))
    return LUA_NOREF;

  luavgl_check_callable(L, index);
  lua_pushvalue(L, index);
  return luaL_ref(L, LUA_REGISTRYINDEX);
}

static void dumpvalue(lua_State *L, int i, bool cr)
{
  const char ending = cr ? '\n' : '\0';
  switch (lua_type(L, i)) {
  case LUA_TNUMBER:
    LV_LOG_USER("number: %g%c", lua_tonumber(L, i), ending);
    break;
  case LUA_TSTRING:
    LV_LOG_USER("string: %s%c", lua_tostring(L, i), ending);
    break;
  case LUA_TBOOLEAN:
    LV_LOG_USER("boolean: %s%c", (lua_toboolean(L, i) ? "true" : "false"),
                ending);
    break;
  case LUA_TNIL:
    LV_LOG_USER("nil: %s%c", "nil", ending);
    break;
  default:
    LV_LOG_USER("pointer: %p%c", lua_topointer(L, i), ending);
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
    LV_LOG_USER(" ");
    dumpvalue(L, -1, 1);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }
  fflush(stdout);
}

static void dumpstack(lua_State *L)
{
  int top = lua_gettop(L);
  for (int i = 1; i <= top; i++) {
    switch (lua_type(L, i)) {
    case LUA_TNUMBER:
      LV_LOG_USER("%s: %g", luaL_typename(L, i), lua_tonumber(L, i));
      break;
    case LUA_TSTRING:
      LV_LOG_USER("%s: %s", luaL_typename(L, i), lua_tostring(L, i));
      break;
    case LUA_TBOOLEAN:
      LV_LOG_USER("%s: %s", luaL_typename(L, i),
                  (lua_toboolean(L, i) ? "true" : "false"));
      break;
    case LUA_TNIL:
      LV_LOG_USER("%s: %s", luaL_typename(L, i), "nil");
      break;
    case LUA_TUSERDATA:
      LV_LOG_USER("%s: %p", luaL_typename(L, i), lua_touserdata(L, i));
      break;
    default:
      LV_LOG_USER("%s: %p", luaL_typename(L, i), lua_topointer(L, i));
      break;
    }
  }
  fflush(stdout);
}

LUALIB_API int luavgl_createmetatable(lua_State *L, const void *key,
                                      const char *name)
{
  /* create metatable, 3 elements, normally for, __index, __gc and
   * __name. */
  lua_createtable(L, 0, 4);
  if (name) {
    lua_pushstring(L, name);
    lua_setfield(L, -2, "__name"); /* metatable.__name = tname */
  }

  /* add to registry */
  lua_pushlightuserdata(L, (void *)key);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX); /* registry[key] = metatable */
  return 1;
}

LUALIB_API int luavgl_getmetatable(lua_State *L, const void *key)
{
  lua_pushlightuserdata(L, (void *)key);
  return lua_rawget(L, LUA_REGISTRYINDEX);
}

/**
 * Create a table(used as object metatable), using clz as key in lua
 * registry. The name is set to metatable.__name if not NULL
 */
LUALIB_API int luavgl_obj_createmetatable(lua_State *L,
                                          const lv_obj_class_t *clz,
                                          const char *name,
                                          const rotable_Reg *l, int n)
{
  if (luavgl_obj_getmetatable(L, clz) != LUA_TNIL) /* meta already exists */
    return 1;
  lua_pop(L, 1);

  /* create metatable, 3 elements, normally for __index, __gc and __name. */
  lua_createtable(L, 0, 3);
  LV_LOG_INFO("create metatable for %s: %p", clz->name, lua_topointer(L, -1));
  if (name) {
    lua_pushstring(L, name);
    lua_setfield(L, -2, "__name"); /* metatable.__name = tname */
  }

  /* add to registry */
  lua_pushlightuserdata(L, (void *)clz);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX); /* registry[clz] = metatable */

  /* New index table.
   * M = {} -- stack top
   *
   * t = {l} -- table contains the lib functions
   * b = getmatatable(clz.base_clz)
   * setmetatable(t, b)
   * M.__index = t
   */
  rotable_newlib(L, l); /* t = {l} */
  if (clz != &lv_obj_class) {
    /* b = getmatatable(clz.base_clz) */
    if (luavgl_obj_getmetatable(L, clz->base_class) == LUA_TNIL) {
      return luaL_error(L, "need to init base class firstly: %s.", name);
    }

    /* setmetatable(t, b) */
    rotable_setmetatable(L, -2);
  } else {
    /* For base obj, add widgets creation method as metatable. So we can use
     * obj:Object{} to create widgets.
     */
    luaL_getmetatable(L, "widgets");
    rotable_setmetatable(L, -2);
  }

  lua_setfield(L, -2, "__index"); /* M.__index = t */
  return 1;
}

int luavgl_obj_getmetatable(lua_State *L, const lv_obj_class_t *clz)
{
  lua_pushlightuserdata(L, (void *)clz);
  return lua_rawget(L, LUA_REGISTRYINDEX);
}

/**
 * get metatable of clz, and set it as the metatable of table on stack idx
 * return 0 if failed.
 */
int luavgl_obj_setmetatable(lua_State *L, int idx, const lv_obj_class_t *clz)
{
  if (luavgl_obj_getmetatable(L, clz) == LUA_TNIL) {
    lua_pop(L, 1);
    return 0;
  }

  lua_setmetatable(L, idx);
  return 1;
}

static int msghandler(lua_State *L)
{
  if (!lua_isstring(L, 1)) /* 'message' not a string? */
    return 1;              /* keep it intact */

  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {                         /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") && /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)      /* that produces a string? */
      return 1;                              /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
                            luaL_typename(L, 1));
  }

  /* append a standard traceback */
  luaL_traceback(L, L, msg, 1);

  /* show it on screen */
  lv_obj_t *root = NULL;
  luavgl_ctx_t *ctx = luavgl_context(L);
  root = lv_obj_create(ctx->root ? ctx->root : lv_scr_act());
  lv_obj_set_size(root, LV_PCT(80), LV_PCT(80));
  lv_obj_center(root);

  lv_obj_set_style_bg_color(root, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_70, 0);
  lv_obj_set_style_border_width(root, 1, 0);

  lv_obj_t *label = lv_label_create(root);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(label, LV_FONT_DEFAULT, 0);
  lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_LIGHT_GREEN),
                              0);
  lv_obj_set_width(label, LV_PCT(80));
  lv_obj_center(label);

  lv_label_set_text(label, lua_tostring(L, -1));
  return 1;
}

LUALIB_API int luavgl_pcall(lua_State *L, int nargs, int nresult)
{
  int base = lua_gettop(L) - nargs; /* function index */
  lua_pushcfunction(L, msghandler); /* push message handler */
  lua_insert(L, base);              /* put it under function and args */
  int status = lua_pcall(L, nargs, nresult, base);
  if (status != LUA_OK) {
    LV_LOG_ERROR("crashed\n%s", lua_tostring(L, -1));
  }
  lua_remove(L, base); /* remove message handler from the stack */

  return status;
}

LUALIB_API void *luavgl_test_obj(lua_State *L, int ud)
{
  void *p = lua_touserdata(L, ud);
  if (p != NULL) {                   /* value is a userdata? */
    if (lua_getmetatable(L, ud)) {   /* does it have a metatable? */
      lua_getfield(L, -1, "__name"); /* get the name string */
      lua_pushstring(L, "luavglObj");
      /* strings are interned, so no need to use strcmp. */
      if (!lua_rawequal(L, -1, -2)) /* not the same? */
        p = NULL;    /* value is a userdata with wrong metatable */
      lua_pop(L, 3); /* remove metatable and two strings */
      return p;
    }
  }

  return NULL; /* value is not a userdata with a metatable */
}

LUALIB_API lv_obj_t *luavgl_to_obj(lua_State *L, int idx)
{
  luavgl_obj_t *lobj = luavgl_test_obj(L, idx);
  if (lobj == NULL || lobj->obj == NULL) {
    luaL_argerror(L, idx, "expect lua lvgl object, got null");
    return NULL;
  }

  return lobj->obj;
}

LUALIB_API int luavgl_tointeger(lua_State *L, int idx)
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

LUALIB_API lv_color_t luavgl_tocolor(lua_State *L, int idx)
{
  lv_color_t color = {0};
  if (lua_type(L, idx) == LUA_TSTRING) {
    /* support #RGB and #RRGGBB */
    const char *s = lua_tostring(L, idx);
    if (s == NULL) {
      luaL_error(L, "unknown color");
      return color;
    }

    int len = lv_strlen(s);
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
      luaL_error(L, "unknown color format");
      return color;
    }
  } else {
    color = lv_color_hex(luavgl_tointeger(L, idx)); /* make to lv_color_t */
  }

  return color;
}

LUALIB_API const char *luavgl_toimgsrc(lua_State *L, int idx)
{
  const char *src = NULL;
  if (lua_isuserdata(L, idx)) {
    src = lua_touserdata(L, idx);
    LV_LOG_INFO("set img src to user data: %p", src);
  } else if (lua_isstring(L, idx)) {
    src = lua_tostring(L, idx);
  } else {
    LV_LOG_ERROR("img src should be string or userdata");
    return NULL;
  }

  return src;
}

LUALIB_API lv_point_t luavgl_topoint(lua_State *L, int idx)
{
  lv_point_t point = {0};
  if (lua_istable(L, idx)) {
    lua_getfield(L, idx, "x");
    point.x = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "y");
    point.y = lua_tointeger(L, -1);
    lua_pop(L, 1);
  } else {
    luaL_error(L, "point should be table");
  }

  return point;
}

LUALIB_API lv_property_t luavgl_toproperty(lua_State *L, int idx,
                                           lv_prop_id_t id)
{
  lv_property_t prop = {0};
  prop.id = id;
  prop.num = 0;

  int type = LV_PROPERTY_ID_TYPE(id);
  switch (type) {
  case LV_PROPERTY_TYPE_INT:
    prop.num = luavgl_tointeger(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_PRECISE:
    prop.precise = lua_tonumber(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_COLOR:
    prop.color = luavgl_tocolor(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_IMGSRC:
    prop.ptr = luavgl_toimgsrc(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_POINT:
    prop.point = luavgl_topoint(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_OBJ:
    prop.ptr = luavgl_to_obj(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_TEXT:
    prop.ptr = lua_tostring(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_FONT:
    prop.ptr = lua_touserdata(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_BOOL:
    prop.enable = lua_toboolean(L, idx);
    return prop;
  case LV_PROPERTY_TYPE_POINTER:
    break;
  default:
    break;
  }

  prop.id = LV_PROPERTY_ID_INVALID;
  return prop;
}

LUALIB_API int luavgl_pushcolor(lua_State *L, lv_color_t color)
{
  lua_pushinteger(L, lv_color_to_int(color));
  return 1;
}

LUALIB_API int luavgl_pushproperty(lua_State *L, const lv_property_t *prop)
{
  lv_prop_id_t id = prop->id;

  int type = LV_PROPERTY_ID_TYPE(id);
  switch (type) {
  case LV_PROPERTY_TYPE_INT:
    lua_pushinteger(L, prop->num);
    return 1;
  case LV_PROPERTY_TYPE_PRECISE:
    lua_pushnumber(L, prop->precise);
    return 1;
  case LV_PROPERTY_TYPE_COLOR:
    luavgl_pushcolor(L, prop->color);
    return 1;
  case LV_PROPERTY_TYPE_POINT:
    /* table of {x, y} */
    lua_createtable(L, 0, 2);
    lua_pushinteger(L, prop->point.x);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, prop->point.y);
    lua_rawseti(L, -2, 2);
    return 1;
  case LV_PROPERTY_TYPE_OBJ:
    lua_pushlightuserdata(L, (void *)prop->ptr);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isnoneornil(L, -1)) {
      luavgl_add_lobj(L, (void *)prop->ptr)->lua_created = false;
    }
    return 1;
  case LV_PROPERTY_TYPE_TEXT:
    lua_pushstring(L, prop->ptr);
    return 1;
  case LV_PROPERTY_TYPE_IMGSRC:
  case LV_PROPERTY_TYPE_FONT:
  case LV_PROPERTY_TYPE_POINTER:
    lua_pushlightuserdata(L, (void *)prop->ptr);
    return 1;
  case LV_PROPERTY_TYPE_BOOL:
    lua_pushboolean(L, prop->enable);
    return 1;
  default:
    return luaL_error(L, "unsupported property type: %d", type);
  }
}

LUALIB_API void luavgl_iterate(lua_State *L, int index,
                               int (*cb)(lua_State *, void *), void *cb_para)
{
  int i = index < 0 ? index - 1 : index;

  lua_pushnil(L); /* nil as initial key to iterate through table */
  while (lua_next(L, i)) {
    /* -1: value, -2: key */
    if (!lua_isstring(L, -2)) {
      /* we expect string as key, ignore it if not */
      LV_LOG_INFO("ignore non-string key in table");
      lua_pop(L, 1);
      continue;
    }

    cb(L, cb_para);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }
}

LUALIB_API int luavgl_set_property_array(lua_State *L, void *obj,
                                         const luavgl_value_setter_t table[],
                                         uint32_t len)
{
  const char *key = lua_tostring(L, -2);
  if (key == NULL) {
    LV_LOG_ERROR("Null key, ignored");
    return -1;
  }

  for (int i = 0; i < len; i++) {
    const luavgl_value_setter_t *p = &table[i];
    if (lv_strcmp(key, p->key))
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
      union {
        lv_color_t c;
        uint32_t v;
      } color;

      color.c = luavgl_tocolor(L, -1);
      p->setter(obj, color.v);
    } else if (p->type == SETTER_TYPE_IMGSRC) {
      /* img src */
      p->setter_pointer(obj, (void *)luavgl_toimgsrc(L, -1));
    } else if (p->type == SETTER_TYPE_STACK) {
      p->setter_stack(obj, L);
    } else if (p->type == SETTER_TYPE_POINTER) {
      void *data = lua_touserdata(L, -1);
      p->setter_pointer(obj, data);
    } else {
      LV_LOG_ERROR("unsupported type: %d", p->type);
    }
    return 0;
  }

  return -1; /* property not found */
}

LUALIB_API void _lv_dummy_set(void *obj, lua_State *L) {}

static int luavgl_pcall_int(lua_State *L, int nargs, int nresult)
{
  return luavgl_context(L)->pcall(L, nargs, nresult);
}
