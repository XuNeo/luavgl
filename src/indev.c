#include "luavgl.h"
#include "private.h"

typedef struct luavgl_indev_s {
  lv_indev_t *indev;
} luavgl_indev_t;

/* group APIs */
static lv_group_t *luavgl_to_group(lua_State *L, int idx);

static luavgl_indev_t *luavgl_check_indev(lua_State *L, int idx)
{
  luavgl_indev_t *v = luaL_checkudata(L, idx, "lv_indev");
  if (v->indev == NULL) {
    luaL_error(L, "null indev");
  }

  return v;
}

/**
 * create new luavgl indev userdata.
 */
static int luavgl_indev_get(lua_State *L, lv_indev_t *indev)
{
  if (indev == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* check if already exists */
  lua_pushlightuserdata(L, indev);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    /* create new indev userdata and add to registry forever */
    lua_pop(L, 1);
    luavgl_indev_t *i = lua_newuserdata(L, sizeof(luavgl_indev_t));
    i->indev = indev;

    luaL_getmetatable(L, "lv_indev");
    lua_setmetatable(L, -2);

    lua_pushlightuserdata(L, indev);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }

  return 1;
}

static int luavgl_indev_get_act(lua_State *L)
{
  lv_indev_t *indev = lv_indev_get_act();
  return luavgl_indev_get(L, indev);
}

#if 0
static int luavgl_indev_get_obj_act(lua_State *L)
{
  lv_obj_t *obj = lv_indev_get_obj_act();
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    luavgl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}
#endif

static int luavgl_indev_get_next(lua_State *L)
{
  lv_indev_t *indev = NULL;
  if (!lua_isnoneornil(L, 1)) {
    indev = luavgl_check_indev(L, 1)->indev;
  }

  indev = lv_indev_get_next(indev);
  return luavgl_indev_get(L, indev);
}

static int luavgl_indev_get_type(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_indev_type_t t = lv_indev_get_type(i->indev);
  lua_pushinteger(L, t);
  return 1;
}

static int luavgl_indev_reset(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_obj_t *obj = luavgl_to_obj(L, 2);
  lv_indev_reset(i->indev, obj);
  return 0;
}

static int luavgl_indev_reset_long_press(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);

  lv_indev_reset_long_press(i->indev);
  return 1;
}

static int luavgl_indev_set_cursor(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_obj_t *obj = luavgl_to_obj(L, 2);
  lv_indev_set_cursor(i->indev, obj);
  return 0;
}

static int luavgl_indev_set_group(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_group_t *group = luavgl_to_group(L, 2);
  lv_indev_set_group(i->indev, group);
  return 0;
}

static int luavgl_indev_get_point(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_point_t point;
  lv_indev_get_point(i->indev, &point);

  lua_pushinteger(L, point.x);
  lua_pushinteger(L, point.y);
  return 2;
}

static int luavgl_indev_get_gesture_dir(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_dir_t dir = lv_indev_get_gesture_dir(i->indev);

  lua_pushinteger(L, dir);
  return 1;
}

static int luavgl_indev_get_key(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  uint32_t v = lv_indev_get_key(i->indev);

  lua_pushinteger(L, v);
  return 1;
}

static int luavgl_indev_get_scroll_dir(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_dir_t dir = lv_indev_get_scroll_dir(i->indev);

  lua_pushinteger(L, dir);
  return 1;
}

static int luavgl_indev_get_scroll_obj(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_obj_t *obj = lv_indev_get_scroll_obj(i->indev);
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    luavgl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

static int luavgl_indev_get_vect(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_point_t point;
  lv_indev_get_vect(i->indev, &point);

  lua_pushinteger(L, point.x);
  lua_pushinteger(L, point.y);
  return 2;
}

static int luavgl_indev_wait_release(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lv_indev_wait_release(i->indev);
  return 0;
}

#if 0
static void indev_feedback_cb(lv_indev_drv_t *driver, uint8_t code)
{
  lua_State *L = driver->user_data;
  int top = lua_gettop(L);
  lua_pushlightuserdata(L, driver);
  lua_rawget(L, LUA_REGISTRYINDEX); /* get indev on stack */

  lua_getuservalue(L, -1);  /* stack: indev, uservalue-table */
  lua_rawgeti(L, -1, code); /* indev, uservalue, cb */
  if (lua_isnoneornil(L, -1)) {
    lua_settop(L, top);
    return;
  }

  lua_pushvalue(L, 1);
  lua_pushinteger(L, code);
  luavgl_pcall_int(L, 2, 0);
  lua_settop(L, top);
}

/**
 * indev:on_event(code, cb)
 */
static int luavgl_indev_on_event(lua_State *L)
{
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  int code = lua_tointeger(L, 2);
  luavgl_check_callable(L, 3);

  lua_pushlightuserdata(L, i->indev->driver);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnoneornil(L, -1)) {
    lua_pop(L, 1);

    /* set indev to registry using pointer as key */
    lua_pushlightuserdata(L, i->indev->driver);
    lua_pushvalue(L, 1);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }

  int type = lua_getuservalue(L, 1);
  if (type != LUA_TTABLE) {
    lua_pop(L, 1);
    lua_createtable(L, 0, 1);
    lua_pushvalue(L, -1);
    lua_setuservalue(L, 1);
  }

  LV_LOG_INFO("add feedback_cb code %d, for indev->driver: %p\n", code,
               i->indev->driver);
  lua_pushvalue(L, 3);
  lua_rawseti(L, -2, code);

  i->indev->driver->feedback_cb = indev_feedback_cb;
  i->indev->driver->user_data = L;
  lua_settop(L, 1);
  return 1;
}
#endif

static int luavgl_indev_tostring(lua_State *L)
{
  lua_pushfstring(L, "lv_indev: %p\n", luavgl_check_indev(L, 1));
  return 1;
}

static int luavgl_indev_gc(lua_State *L)
{
  LV_LOG_INFO("enter");

#if 0
  /* If set_feedback_cb is used, then the indev only gets gc'ed when lua vm
   * exits.
   */
  luavgl_indev_t *i = luavgl_check_indev(L, 1);
  lua_getuservalue(L, 1);
  if (!lua_isnoneornil(L, -1)) {
    lua_pushlightuserdata(L, i->indev->driver);
    lua_pushnil(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
    i->indev->driver->feedback_cb = NULL;
  }
#endif

  return 0;
}

/**
 * luavgl.indev lib
 */
static const luaL_Reg indev_lib[] = {
    {"get_act",     luavgl_indev_get_act    },
#if 0
    {"get_obj_act", luavgl_indev_get_obj_act},
#endif
    {"get_next",    luavgl_indev_get_next   },

    {NULL,          NULL                    },
};

/*
** methods for file handles
*/
static const luaL_Reg methods[] = {
    {"get_type",         luavgl_indev_get_type        },
    {"reset",            luavgl_indev_reset           },
    {"reset_long_press", luavgl_indev_reset_long_press},
    {"set_cursor",       luavgl_indev_set_cursor      },
    {"set_group",        luavgl_indev_set_group       },
    {"get_point",        luavgl_indev_get_point       },
    {"get_gesture_dir",  luavgl_indev_get_gesture_dir },
    {"get_key",          luavgl_indev_get_key         },
    {"get_scroll_dir",   luavgl_indev_get_scroll_dir  },
    {"get_scroll_obj",   luavgl_indev_get_scroll_obj  },
    {"get_vect",         luavgl_indev_get_vect        },
    {"wait_release",     luavgl_indev_wait_release    },
#if 0
    {"on_event",         luavgl_indev_on_event        },
#endif
    {NULL,               NULL                         },
};

static const luaL_Reg meta[] = {
    {"__gc",       luavgl_indev_gc      },
    {"__tostring", luavgl_indev_tostring},
    {"__index",    NULL                 }, /* place holder */

    {NULL,         NULL                 }
};

static void luavgl_indev_init(lua_State *L)
{
  /* create lv_fs metatable */
  luaL_newmetatable(L, "lv_indev");
  luaL_setfuncs(L, meta, 0);

  luaL_newlib(L, methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */

  /* luavgl.indev lib */
  luaL_newlib(L, indev_lib);
  lua_setfield(L, -2, "indev");
}
