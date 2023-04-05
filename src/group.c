#include "luavgl.h"
#include "private.h"

typedef struct luavgl_group_s {
  lv_group_t *group;
  bool lua_created; /* this group is created from lua */
  bool deleted;     /* deleted but not gc'ed */
} luavgl_group_t;

static luavgl_group_t *luavgl_check_group(lua_State *L, int idx)
{
  luavgl_group_t *v = luaL_checkudata(L, idx, "lv_group");

  if (v->deleted) {
    luaL_error(L, "attempt to use a deleted group");
    return NULL;
  }

  return v;
}

static lv_group_t *luavgl_to_group(lua_State *L, int idx)
{
  return luavgl_check_group(L, idx)->group;
}

/**
 * create new luavgl group userdata.
 */
static int luavgl_group_get(lua_State *L, lv_group_t *group)
{
  /* check if already exists */
  lua_pushlightuserdata(L, group);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    /* create new indev userdata and add to registry */
    lua_pop(L, 1);
    luavgl_group_t *g = lua_newuserdata(L, sizeof(luavgl_group_t));
    g->deleted = false;
    g->lua_created = true;
    g->group = group;

    luaL_getmetatable(L, "lv_group");
    lua_setmetatable(L, -2);

    lua_pushlightuserdata(L, group);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }

  return 1;
}

/**
 * group = luavgl.group.create()
 */
static int luavgl_group_create(lua_State *L)
{
  lv_group_t *group = lv_group_create();
  return luavgl_group_get(L, group);
}

/**
 * Delete a lua created group
 */
static int luavgl_group_delete(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  /* remove from registry */
  lua_pushlightuserdata(L, g->group);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  g->deleted = true;

#if 0
  /* delete group if it's lua created. */
  if (g->lua_created) {
    lv_group_del(g->group);
    g->group = NULL;
  }
#else
  /* delete the group anyway */
  lv_group_del(g->group);
  g->group = NULL;
#endif

  debug("delete group:%p\n", g);
  return 0;
}

/**
 * group = luavgl.group.get_default()
 */
static int luavgl_group_get_default(lua_State *L)
{
  lv_group_t *group = lv_group_get_default();

  /* check if already exists */
  lua_pushlightuserdata(L, group);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    lua_pop(L, 1);
    luavgl_group_get(L, group);
    luavgl_check_group(L, -1)->lua_created = false;
  }

  return 1;
}

static int luavgl_group_set_default(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  lv_group_set_default(g->group);
  return 0;
}

static int luavgl_group_add_obj(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  lv_obj_t *obj = luavgl_to_obj(L, 2);

  lv_group_add_obj(g->group, obj);
  return 0;
}

static int luavgl_group_remove_objs(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  lv_group_remove_all_objs(g->group);
  return 0;
}

static int luavgl_group_focus_next(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  lv_group_focus_next(g->group);
  return 0;
}

static int luavgl_group_focus_prev(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  lv_group_focus_prev(g->group);
  return 0;
}

static int luavgl_group_focus_freeze(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  bool en = luavgl_tointeger(L, 2);
  lv_group_focus_freeze(g->group, en);
  return 0;
}

static int luavgl_group_send_data(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  uint32_t c = luavgl_tointeger(L, 2);
  lv_group_send_data(g->group, c);
  return 0;
}

static int luavgl_group_set_focus_cb(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  (void)g;
  return 0;
}

static int luavgl_group_set_edge_cb(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  (void)g;
  return 0;
}

static int luavgl_group_get_focus_cb(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  (void)g;
  return 0;
}

static int luavgl_group_get_edge_cb(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);

  (void)g;
  return 0;
}

static int luavgl_group_set_editing(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  bool en = luavgl_tointeger(L, 2);
  lv_group_set_editing(g->group, en);
  return 0;
}

static int luavgl_group_set_wrap(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  bool en = luavgl_tointeger(L, 2);
  lv_group_set_wrap(g->group, en);
  return 0;
}

static int luavgl_group_get_wrap(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  bool en = lv_group_get_wrap(g->group);
  lua_pushboolean(L, en);
  return 1;
}

static int luavgl_group_get_obj_count(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  uint32_t count = lv_group_get_obj_count(g->group);
  lua_pushinteger(L, count);
  return 1;
}

static int luavgl_group_get_focused(lua_State *L)
{
  luavgl_group_t *g = luavgl_check_group(L, 1);
  lv_obj_t *obj = lv_group_get_focused(g->group);
  if (obj == NULL) {
    lua_pushnil(L);
  } else {
    lua_pushlightuserdata(L, obj);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isnoneornil(L, -1)) {
      luavgl_add_lobj(L, obj)->lua_created = false;
    }
  }

  return 1;
}

static int luavgl_group_remove_obj(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_group_remove_obj(obj);
  return 0;
}

static int luavgl_group_focus_obj(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_group_focus_obj(obj);
  return 0;
}

static int luavgl_group_gc(lua_State *L)
{
  luavgl_group_t *g = luaL_checkudata(L, 1, "lv_group");
  if (g->deleted) {
    return 0;
  }

#if 0
  /* delete group if it's lua created. */
  if (g->lua_created) {
    lv_group_del(g->group);
    g->group = NULL;
  }
#else
  /* delete the group anyway */
  lv_group_del(g->group);
  g->group = NULL;
#endif

  debug("gc group: %p\n", g);
  return 0;
}

/**
 * luavgl.group lib
 */
static const luaL_Reg group_lib[] = {
    {"create",      luavgl_group_create     },
    {"get_default", luavgl_group_get_default},
    {"remove_obj",  luavgl_group_remove_obj },
    {"remove_objs", luavgl_group_remove_objs},

    {"focus_obj",   luavgl_group_focus_obj  },
    {NULL,          NULL                    },
};

/*
** methods for file handles
*/
static const luaL_Reg group_methods[] = {
    {"delete",        luavgl_group_delete       },
    {"set_default",   luavgl_group_set_default  },
    {"add_obj",       luavgl_group_add_obj      },
    {"remove_obj",    luavgl_group_remove_obj   },
    {"focus_next",    luavgl_group_focus_next   },
    {"focus_prev",    luavgl_group_focus_prev   },
    {"focus_freeze",  luavgl_group_focus_freeze },
    {"send_data",     luavgl_group_send_data    },
    {"set_focus_cb",  luavgl_group_set_focus_cb },
    {"set_edge_cb",   luavgl_group_set_edge_cb  },
    {"get_focus_cb",  luavgl_group_get_focus_cb },
    {"get_edge_cb",   luavgl_group_get_edge_cb  },
    {"set_editing",   luavgl_group_set_editing  },
    {"set_wrap",      luavgl_group_set_wrap     },
    {"get_wrap",      luavgl_group_get_wrap     },
    {"get_obj_count", luavgl_group_get_obj_count},
    {"get_focused",   luavgl_group_get_focused  },

    {NULL,            NULL                      },
};

static const luaL_Reg group_meta[] = {
    {"__gc",    luavgl_group_gc},
    {"__index", NULL           }, /* place holder */

    {NULL,      NULL           }
};

static void luavgl_group_init(lua_State *L)
{
  /* create lv_group metatable */
  luaL_newmetatable(L, "lv_group");
  luaL_setfuncs(L, group_meta, 0);

  luaL_newlib(L, group_methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */

  /* luavgl.indev lib */
  luaL_newlib(L, group_lib);
  lua_setfield(L, -2, "group");
}
