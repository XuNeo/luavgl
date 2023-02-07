#include <assert.h>

#include "lugl.h"
#include "private.h"

typedef struct lugl_group_s {
  lv_group_t* group;
  bool lua_created;     /* this group is created from lua */
} lugl_group_t;

static lv_group_t *lugl_check_group(lua_State *L, int index)
{
  lv_group_t *v = *(lv_group_t **)luaL_checkudata(L, index, "lv_group");
  return v;
}

/**
 * t = lugl:group({group parameters})
 * t:pause()
 * t:resume()
 * t:ready() -- make it ready right now
 *
 */
static int lugl_group_create(lua_State *L)
{
  lugl_group_t *data = malloc(sizeof(lugl_group_t));
  if (data == NULL) {
    return luaL_error(L, "No memory.");
  }
  data->ref = LUA_NOREF;
  data->L = L;

  lv_group_t *g = lv_group_create();

  *(void **)lua_newuserdata(L, sizeof(void *)) = g;
  luaL_getmetatable(L, "lv_group");
  lua_setmetatable(L, -2);

  lua_pushlightuserdata(L, g);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  return 1;
}

static int lugl_group_ready(lua_State *L)
{
  lv_group_t *t = lugl_check_group(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "group is null.");
  }

  lv_group_ready(t);
  return 0;
}

static int lugl_group_resume(lua_State *L)
{
  lv_group_t *t = lugl_check_group(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "group is null.");
  }

  lv_group_resume(t);
  return 0;
}

static int lugl_group_pause(lua_State *L)
{
  lv_group_t *t = lugl_check_group(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "group is null.");
  }

  lv_group_pause(t);

  return 0;
}

/* remove group from obj,  */
static int lugl_group_delete(lua_State *L)
{
  lv_group_t *t = lugl_check_group(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "group is null.");
  }

  lugl_group_t *data = t->user_data;
  if (data->ref) {
    luaL_unref(L, LUA_REGISTRYINDEX, data->ref);
    data->ref = LUA_NOREF;
  }

  lua_pushlightuserdata(L, t);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  /* we can only release memory in gc, since we need t->use_data */
  lv_group_pause(t);

  debug("delete group:%p\n", t);
  return 0;
}

static int lugl_group_gc(lua_State *L)
{
  /* stop group if not stopped. */
  lugl_group_delete(L);

  lv_group_t *t = lugl_check_group(L, 1);
  free(t->user_data);
  lv_group_del(t);

  debug("gc group:%p\n", t);
  return 0;
}
