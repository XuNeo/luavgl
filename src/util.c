#include <lauxlib.h>
#include <lua.h>

#include "lugl.h"
#include "private.h"

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
int lugl_obj_createmetatable(lua_State *L, const lv_obj_class_t *clz,
                             const char *name, const luaL_Reg *l, int n)
{
  if (lugl_obj_getmetatable(L, clz) != LUA_TNIL) /* meta already exists */
    return 0; /* leave previous value on top, but return 0 */
  lua_pop(L, 1);

  /* create metatable, 3 elements, normally for __index, __gc and __name*/
  lua_createtable(L, 0, 3);
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
  lua_createtable(L, 0, n); /* t = {} */
  luaL_setfuncs(L, l, 0);   /* set methods to t */
  if (clz != &lv_obj_class) {
    /* b = getmatatable(clz.base_clz) */
    if (lugl_obj_getmetatable(L, clz->base_class) == LUA_TNIL) {
      return luaL_error(L, "need to init base class firstly: %s.", name);
    }

    /* setmetatable(t, b) */
    lua_setmetatable(L, -2);
  }

  lua_setfield(L, -2, "__index"); /* M.__index = t */
  return 1;
}

int lugl_obj_getmetatable(lua_State *L, const lv_obj_class_t *clz)
{
  lua_pushlightuserdata(L, (void *)clz);
  return lua_rawget(L, LUA_REGISTRYINDEX);
}

/**
 * get metatable of clz, and set it as the metatable of table on stack idx
 * return 0 if failed.
 */
int lugl_obj_setmetatable(lua_State *L, int idx, const lv_obj_class_t *clz)
{
  if (lugl_obj_getmetatable(L, clz) == LUA_TNIL) {
    lua_pop(L, 1);
    return 0;
  }

  lua_setmetatable(L, idx);
  return 1;
}

lv_obj_t *lugl_to_obj(lua_State *L, int idx)
{
  lugl_obj_t *lobj = lugl_to_lobj(L, idx);
  if (lobj == NULL)
    return NULL;

  return lobj->obj;
}

int lugl_tointeger(lua_State *L, int idx)
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

lv_color_t lugl_tocolor(lua_State *L, int idx)
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

const char *lugl_toimgsrc(lua_State *L, int idx)
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

void lugl_iterate(lua_State *L, int index, int (*cb)(lua_State *, void *),
                  void *cb_para)
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

int lugl_set_property_array(lua_State *L, void *obj,
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

void _lv_dummy_set(void *obj, lua_State *L)
{
  //
}