#include "luavgl.h"
#include "private.h"

typedef struct luavgl_timer_s {
  lua_State *L;
  int ref; /* ref to callback */
} luavgl_timer_t;

static lv_timer_t *luavgl_check_timer(lua_State *L, int index)
{
  lv_timer_t *v = *(lv_timer_t **)luaL_checkudata(L, index, "lv_timer");
  return v;
}

static void luavgl_timer_cb(lv_timer_t *t)
{
  luavgl_timer_t *data = t->user_data;
  lua_State *L = data->L;
  int ref = data->ref;
  if (ref == LUA_NOREF) {
    return;
  }

  int top = lua_gettop(L);
  /* stack: 1. function, 2. timer userdata */
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  lua_pushlightuserdata(L, t);
  lua_rawget(L, LUA_REGISTRYINDEX); /* this should not fail*/

  luavgl_pcall_int(L, 1, 0);
  lua_settop(L, top);
}

static void luavgl_timer_set_cb(void *_t, lua_State *L)
{
  lv_timer_t *t = _t;
  int ref = luavgl_check_continuation(L, -1);
  luavgl_timer_t *data = t->user_data;

  if (data->ref != LUA_NOREF) {
    luaL_unref(L, LUA_REGISTRYINDEX, data->ref);
  }
  data->ref = ref;
}

static void luavgl_timer_set_paused(lv_timer_t *t, int paused)
{
  /* pause or resume. */
  paused ? lv_timer_pause(t) : lv_timer_resume(t);
}

/* clang-format off */
static const luavgl_value_setter_t timer_property_table[] = {
    { "paused", 0, { .setter = (setter_int_t)luavgl_timer_set_paused } },
    { "period", 0, { .setter = (setter_int_t)lv_timer_set_period } },
    { "repeat_count", 0, { .setter = (setter_int_t)lv_timer_set_repeat_count } },
    { "cb", SETTER_TYPE_STACK, { .setter_stack = luavgl_timer_set_cb } },
};

/* clang-format on */

static int timer_set_para_cb(lua_State *L, void *data)
{
  int ret = luavgl_set_property(L, data, timer_property_table);

  if (ret != 0) {
    LV_LOG_ERROR("failed");
  }

  return ret;
}

/* setup timer using parameter in table idx */
int luavgl_timer_setup(lua_State *L, int idx, lv_timer_t *t)
{
  luavgl_iterate(L, idx, timer_set_para_cb, t);
  return 1;
}

/**
 * t = luavgl:timer({timer parameters})
 * t:pause()
 * t:resume()
 * t:ready() -- make it ready right now
 *
 */
static int luavgl_timer_create(lua_State *L)
{
  luavgl_timer_t *data = malloc(sizeof(luavgl_timer_t));
  if (data == NULL) {
    return luaL_error(L, "No memory");
  }
  data->ref = LUA_NOREF;
  data->L = L;

  lv_timer_t *t = lv_timer_create(luavgl_timer_cb, 0, data);

  *(void **)lua_newuserdata(L, sizeof(void *)) = t;
  luaL_getmetatable(L, "lv_timer");
  lua_setmetatable(L, -2);

  lua_pushlightuserdata(L, t);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  /* parameter table is on stack[1] */
  luavgl_timer_setup(L, 1, t);
  lua_remove(L, 1); /* remove it to keep stack balance */

  if (t->period == 0) {
    /* if period is not set, then it's paused. */
    lv_timer_pause(t);
  }

  return 1;
}

static int luavgl_timer_set(lua_State *L)
{
  lv_timer_t *t = luavgl_check_timer(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "timer is null");
  }

  luavgl_timer_setup(L, 2, t);
  return 0;
}

static int luavgl_timer_ready(lua_State *L)
{
  lv_timer_t *t = luavgl_check_timer(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "timer is null");
  }

  lv_timer_ready(t);
  return 0;
}

static int luavgl_timer_resume(lua_State *L)
{
  lv_timer_t *t = luavgl_check_timer(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "timer is null");
  }

  lv_timer_resume(t);
  return 0;
}

static int luavgl_timer_pause(lua_State *L)
{
  lv_timer_t *t = luavgl_check_timer(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "timer is null");
  }

  lv_timer_pause(t);

  return 0;
}

/* remove timer from obj,  */
static int luavgl_timer_delete(lua_State *L)
{
  lv_timer_t *t = luavgl_check_timer(L, 1);
  if (t == NULL) {
    return luaL_argerror(L, 1, "timer is null");
  }

  luavgl_timer_t *data = t->user_data;
  if (data->ref) {
    luaL_unref(L, LUA_REGISTRYINDEX, data->ref);
    data->ref = LUA_NOREF;
  }

  lua_pushlightuserdata(L, t);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  /* we can only release memory in gc, since we need t->use_data */
  lv_timer_pause(t);

  LV_LOG_INFO("delete timer:%p", t);
  return 0;
}

static int luavgl_timer_gc(lua_State *L)
{
  /* stop timer if not stopped. */
  luavgl_timer_delete(L);

  lv_timer_t *t = luavgl_check_timer(L, 1);
  free(t->user_data);
  lv_timer_del(t);

  LV_LOG_INFO("gc timer:%p", t);
  return 0;
}

static const luaL_Reg luavgl_timer_methods[] = {
    {"set",    luavgl_timer_set   },
    {"pause",  luavgl_timer_pause },
    {"resume", luavgl_timer_resume},
    {"delete", luavgl_timer_delete},
    {"ready",  luavgl_timer_ready },

    {NULL,     NULL               }
};

static void luavgl_timer_init(lua_State *L)
{
  luaL_newmetatable(L, "lv_timer");

  lua_pushcfunction(L, luavgl_timer_gc);
  lua_setfield(L, -2, "__gc");

  luaL_newlib(L, luavgl_timer_methods); /* methods belong to this type */
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
