#include "luavgl.h"
#include "private.h"

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
  debug("arg not lv_obj userdata.\n");
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
    printf(" ");
    dumpvalue(L, -1, 1);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }
  fflush(stdout);
}

static void dumpstack(lua_State *L)
{
  int top = lua_gettop(L);
  printf("\n");
  for (int i = 1; i <= top; i++) {
    printf("%d\t%s\t", i, luaL_typename(L, i));
    switch (lua_type(L, i)) {
    case LUA_TNUMBER:
      printf("number: %g\n", lua_tonumber(L, i));
      break;
    case LUA_TSTRING:
      printf("string: %s\n", lua_tostring(L, i));
      break;
    case LUA_TBOOLEAN:
      printf("boolean: %s\n", (lua_toboolean(L, i) ? "true" : "false"));
      break;
    case LUA_TNIL:
      printf("nil: %s\n", "nil");
      break;
    default:
      printf("pointer: %p\n", lua_topointer(L, i));
      break;
    }
  }
  fflush(stdout);
}

/**
 * Create a table(used as object metatable), using clz as key in lua
 * registry. The name is set to metatable.__name if not NULL
 */
LUALIB_API int luavgl_obj_createmetatable(lua_State *L,
                                          const lv_obj_class_t *clz,
                                          const char *name, const luaL_Reg *l,
                                          int n)
{
  if (luavgl_obj_getmetatable(L, clz) != LUA_TNIL) /* meta already exists */
    return 0; /* leave previous value on top, but return 0 */
  lua_pop(L, 1);

  /* create metatable, 4 elements, normally for __magic, __index, __gc and
   * __name. */
  lua_createtable(L, 0, 4);
  if (name) {
    lua_pushstring(L, name);
    lua_setfield(L, -2, "__name"); /* metatable.__name = tname */
  }

  /** A magic string we used to check if userdata is a luavgl object. */
  lua_pushstring(L, "luavglObj");
  lua_setfield(L, -2, "__magic");

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
  lua_createtable(L, 0, n); /* t = {} */
  luaL_setfuncs(L, l, 0);   /* set methods to t */
  if (clz != &lv_obj_class) {
    /* b = getmatatable(clz.base_clz) */
    if (luavgl_obj_getmetatable(L, clz->base_class) == LUA_TNIL) {
      return luaL_error(L, "need to init base class firstly: %s.", name);
    }

    /* setmetatable(t, b) */
    lua_setmetatable(L, -2);
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
    debug("crashed\n%s", lua_tostring(L, -1));
  }
  lua_remove(L, base); /* remove message handler from the stack */

  return status;
}

LUALIB_API void *luavgl_test_obj(lua_State *L, int ud)
{
  void *p = lua_touserdata(L, ud);
  if (p != NULL) {                    /* value is a userdata? */
    if (lua_getmetatable(L, ud)) {    /* does it have a metatable? */
      lua_getfield(L, -1, "__magic"); /* get the magic string */
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
    color = lv_color_hex(luavgl_tointeger(L, idx)); /* make to lv_color_t */
  }

  return color;
}

LUALIB_API const char *luavgl_toimgsrc(lua_State *L, int idx)
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

LUALIB_API void luavgl_iterate(lua_State *L, int index,
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

LUALIB_API int luavgl_set_property_array(lua_State *L, void *obj,
                                         const luavgl_value_setter_t table[],
                                         uint32_t len)
{
  const char *key = lua_tostring(L, -2);
  if (key == NULL) {
    debug("Null key, ignored.\n");
    return -1;
  }

  for (int i = 0; i < len; i++) {
    const luavgl_value_setter_t *p = &table[i];
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
      lv_color_t color = luavgl_tocolor(L, -1);
      p->setter(obj, color.full);
    } else if (p->type == SETTER_TYPE_IMGSRC) {
      /* img src */
      p->setter_pointer(obj, (void *)luavgl_toimgsrc(L, -1));
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

LUALIB_API void _lv_dummy_set(void *obj, lua_State *L) {}

static int luavgl_pcall_int(lua_State *L, int nargs, int nresult)
{
  return luavgl_context(L)->pcall(L, nargs, nresult);
}
