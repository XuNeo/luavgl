#include <assert.h>

#include "lugl.h"
#include "private.h"

typedef struct lugl_indev_s {
  lv_indev_t *indev;
} lugl_indev_t;

static lugl_indev_t *lugl_to_indev(lua_State *L, int idx)
{
  lugl_indev_t *v = luaL_checkudata(L, idx, "lv_indev");

  return v;
}

/**
 * create new lugl indev userdata.
 */
static int lugl_indev_get(lua_State *L, lv_indev_t *indev)
{
  /* check if already exists */
  lua_pushlightuserdata(L, indev);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    /* create new indev userdata and add to registry forever */
    lua_pop(L, 1);
    lugl_indev_t *i = lua_newuserdata(L, sizeof(lugl_indev_t));
    i->indev = indev;

    luaL_getmetatable(L, "lv_indev");
    lua_setmetatable(L, -2);

    lua_pushlightuserdata(L, indev);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }

  return 1;
}

static int lugl_indev_get_act(lua_State *L)
{
  lv_indev_t *indev = lv_indev_get_act();
  return lugl_indev_get(L, indev);
}

static int lugl_indev_get_obj_act(lua_State *L)
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
    lugl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

static int lugl_indev_get_type(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_indev_type_t t = lv_indev_get_type(i->indev);
  lua_pushinteger(L, t);
  return 1;
}

static int lugl_indev_reset(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_obj_t *obj = lugl_to_obj(L, 2);
  if (obj == NULL) {
    return luaL_argerror(L, 2, "expect lvgl obj.");
  }

  lv_indev_reset(i->indev, obj);
  return 0;
}

static int lugl_indev_reset_long_press(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);

  lv_indev_reset_long_press(i->indev);
  return 1;
}

static int lugl_indev_set_cursor(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_obj_t *obj = lugl_to_obj(L, 2);
  if (obj == NULL) {
    return luaL_argerror(L, 2, "expect lvgl obj.");
  }

  lv_indev_set_cursor(i->indev, obj);
  return 0;
}

static int lugl_indev_set_group(lua_State *L)
{
  luaL_error(L, "not implemented yet.");
  return 0;
}

static int lugl_indev_get_point(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_point_t point;
  lv_indev_get_point(i->indev, &point);

  lua_pushinteger(L, point.x);
  lua_pushinteger(L, point.y);
  return 2;
}

static int lugl_indev_get_gesture_dir(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_dir_t dir = lv_indev_get_gesture_dir(i->indev);

  lua_pushinteger(L, dir);
  return 1;
}

static int lugl_indev_get_key(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  uint32_t v = lv_indev_get_key(i->indev);

  lua_pushinteger(L, v);
  return 1;
}

static int lugl_indev_get_scroll_dir(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_dir_t dir = lv_indev_get_scroll_dir(i->indev);

  lua_pushinteger(L, dir);
  return 1;
}

static int lugl_indev_get_scroll_obj(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_obj_t *obj = lv_indev_get_scroll_obj(i->indev);
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    lugl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

static int lugl_indev_get_vect(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_point_t point;
  lv_indev_get_vect(i->indev, &point);

  lua_pushinteger(L, point.x);
  lua_pushinteger(L, point.y);
  return 2;
}

static int lugl_indev_wait_release(lua_State *L)
{
  lugl_indev_t *i = lugl_to_indev(L, 1);
  lv_indev_wait_release(i->indev);
  return 0;
}

static int lugl_indev_tostring(lua_State *L)
{
  lua_pushfstring(L, "lv_indev: %p\n", lugl_to_indev(L, 1));
  return 1;
}

static int lugl_indev_gc(lua_State *L)
{
  debug("\n");

  return 0;
}

/**
 * lugl.indev lib
 */
static const luaL_Reg indev_lib[] = {
    {"get_act",     lugl_indev_get_act    },
    {"get_obj_act", lugl_indev_get_obj_act},

    {NULL,          NULL                  },
};

/*
** methods for file handles
*/
static const luaL_Reg methods[] = {
    {"get_type",         lugl_indev_get_type        },
    {"reset",            lugl_indev_reset           },
    {"reset_long_press", lugl_indev_reset_long_press},
    {"set_cursor",       lugl_indev_set_cursor      },
    {"set_group",        lugl_indev_set_group       },
    {"get_point",        lugl_indev_get_point       },
    {"get_gesture_dir",  lugl_indev_get_gesture_dir },
    {"get_key",          lugl_indev_get_key         },
    {"get_scroll_dir",   lugl_indev_get_scroll_dir  },
    {"get_scroll_obj",   lugl_indev_get_scroll_obj  },
    {"get_vect",         lugl_indev_get_vect        },
    {"wait_release",     lugl_indev_wait_release    },

    {NULL,               NULL                       },
};

static const luaL_Reg meta[] = {
    {"__gc",       lugl_indev_gc      },
    {"__tostring", lugl_indev_tostring},
    {"__index",    NULL               }, /* place holder */

    {NULL,         NULL               }
};

static void lugl_indev_init(lua_State *L)
{
  /* create lv_fs metatable */
  luaL_newmetatable(L, "lv_indev");
  luaL_setfuncs(L, meta, 0);

  luaL_newlib(L, methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */

  /* lugl.indev lib */
  luaL_newlib(L, indev_lib);
  lua_setfield(L, -2, "indev");
}
