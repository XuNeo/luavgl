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
      } if (lua_isinteger(L, -1)) {
        v = lua_tointeger(L, -1);
      } else {
        v = lua_tonumber(L, -1);
      }
      p->setter(obj, v);
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

static int _lugl_set_obj_style(lua_State *L, lv_obj_t *obj,
                               lv_style_selector_t selector,
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
      } if (lua_isinteger(L, -1)) {
        v = lua_tointeger(L, -1);
      } else {
        v = lua_tonumber(L, -1);
      }
      p->setter_s(obj, v, selector);
    } else if (p->type == SETTER_TYPE_STACK) {
      p->setter_stack_s(obj, L, selector);
    } else if (p->type == SETTER_TYPE_POINTER) {
      void *data = lua_touserdata(L, -1);
      p->setter_pointer_s(obj, data, selector);
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