#include "luavgl.h"
#include "private.h"

/**
 * Different to lvgl anim, anim in lua can be restarted after it's done.
 * anim.stop()
 * anim.start()
 * anim.delete() this will mark anim as no longer needed and waiting for gc.
 */

typedef struct luavgl_anim_s {
  lua_State *L;
  lv_anim_t *aa; /* the handler returns be lv_anim_start */
  lv_anim_t cfg; /* the configuration, must not be the first element. */

  int obj_ref; /* the object used for animation, used as in exec_cb */
  int exec_cb;
  int done_cb;
  int self_ref; /* ref in registry to this anim */
  bool deleted; /* manually deleted from lua */
} luavgl_anim_t;

typedef luavgl_anim_t *luavgl_anim_handle_t;

static luavgl_anim_t *luavgl_check_anim(lua_State *L, int index)
{
  luavgl_anim_t *a = luaL_checkudata(L, index, "lv_anim");

  if (a->deleted) {
    luaL_argerror(L, index, "anim already deleted.");
    return NULL;
  }

  return a;
}

static void luavgl_anim_exec_cb(void *var, int32_t value)
{
  luavgl_anim_t *a = var;
  lua_State *L = a->L;

  if (a->exec_cb == LUA_NOREF || a->obj_ref == LUA_NOREF) {
    debug("anim error, callback or obj not found.\n");
    luaL_error(L, "anim founds no callback or obj.");
    return;
  }

  /* stack: 1. function, 2. obj-userdata, 3. value */
  lua_rawgeti(L, LUA_REGISTRYINDEX, a->exec_cb);
  lua_rawgeti(L, LUA_REGISTRYINDEX, a->obj_ref);
  lua_pushinteger(L, value);

  luavgl_pcall_int(L, 2, 0);
}

/* callback when anim is deleted by lvgl */
static void luavgl_anim_delete_cb(lv_anim_t *_a)
{
  luavgl_anim_t *a = _a->var;
  a->aa = NULL; /* handler deleted by lvgl */

  lua_State *L = a->L;

  if (a->done_cb != LUA_NOREF) {
    /* stack: 1. function, 2. this anim, 3. obj-userdata */
    lua_rawgeti(L, LUA_REGISTRYINDEX, a->done_cb);
    lua_rawgeti(L, LUA_REGISTRYINDEX, a->self_ref);
    lua_rawgeti(L, LUA_REGISTRYINDEX, a->obj_ref);
    luavgl_pcall_int(L, 2, 0);
  }

  /* it's paused or deleted, thus can be gc'ed */
  luaL_unref(L, LUA_REGISTRYINDEX, a->self_ref);
  a->self_ref = LUA_NOREF;
}

static void _lv_anim_set_start_value(lv_anim_t *a, int32_t v)
{
  a->start_value = v;
}

static void _lv_anim_set_end_value(lv_anim_t *a, int32_t v)
{
  a->end_value = v;
}

static void _lv_anim_set_path(void *obj, lua_State *L)
{
  lv_anim_t *a = obj;
  const char *path = NULL;

  if (lua_isstring(L, -1)) {
    path = lua_tostring(L, -1);
  }

  a->path_cb = lv_anim_path_linear;
  if (path == NULL || strcmp(path, "linear") == 0) {
    ; /* use default linear path */
  } else if (strcmp(path, "ease_in") == 0) {
    a->path_cb = lv_anim_path_ease_in;
  } else if (strcmp(path, "ease_out") == 0) {
    a->path_cb = lv_anim_path_ease_out;
  } else if (strcmp(path, "ease_in_out") == 0) {
    a->path_cb = lv_anim_path_ease_in_out;
  } else if (strcmp(path, "overshoot") == 0) {
    a->path_cb = lv_anim_path_overshoot;
  } else if (strcmp(path, "bounce") == 0) {
    a->path_cb = lv_anim_path_bounce;
  } else if (strcmp(path, "step") == 0) {
    a->path_cb = lv_anim_path_step;
  }
}

/* clang-format off */
static const luavgl_value_setter_t anim_property_table[] = {
    { "run", SETTER_TYPE_STACK, { .setter_stack = _lv_dummy_set } },
    { "start_value", 0, { .setter = (setter_int_t)_lv_anim_set_start_value } },
    { "end_value", 0, { .setter = (setter_int_t)_lv_anim_set_end_value } },
    { "time", 0, { .setter = (setter_int_t)lv_anim_set_time } },
    { "duration", 0, { .setter = (setter_int_t)lv_anim_set_time } },
    { "delay", 0, { .setter = (setter_int_t)lv_anim_set_delay } },
    { "repeat_count", 0, { .setter = (setter_int_t)lv_anim_set_repeat_count } },
    { "repeat_delay", 0, { .setter = (setter_int_t)lv_anim_set_repeat_delay } },
    { "early_apply", 0, { .setter = (setter_int_t)lv_anim_set_early_apply } },
    { "playback_time", 0, { .setter = (setter_int_t)lv_anim_set_playback_time } },
    { "playback_delay", 0, { .setter = (setter_int_t)lv_anim_set_playback_delay } },
    { "path", SETTER_TYPE_STACK, { .setter_stack = _lv_anim_set_path } },
    { "exec_cb", SETTER_TYPE_STACK, { .setter_stack = _lv_dummy_set } },
};

/* clang-format on */

static int anim_set_para_cb(lua_State *L, void *data)
{
  int ret = luavgl_set_property(L, data, anim_property_table);

  if (ret != 0) {
    debug("failed\n");
  }

  return ret;
}

static int luavgl_anim_stop(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, 1);

  if (a->aa == NULL || a->self_ref == LUA_NOREF) {
    debug("already stopped");
    return 0;
  }

  lv_anim_del(a, luavgl_anim_exec_cb);

  /* work done in luavgl_anim_delete_cb */
  return 0;
}

static int luavgl_anim_delete(lua_State *L)
{
  luavgl_anim_stop(L);
  luavgl_anim_t *a = luavgl_check_anim(L, 1);

  /* object and callbacks can be gc'ed now */
  luaL_unref(L, LUA_REGISTRYINDEX, a->done_cb);
  luaL_unref(L, LUA_REGISTRYINDEX, a->exec_cb);
  luaL_unref(L, LUA_REGISTRYINDEX, a->obj_ref);
  luaL_unref(L, LUA_REGISTRYINDEX, a->self_ref);
  a->done_cb = LUA_NOREF;
  a->exec_cb = LUA_NOREF;
  a->obj_ref = LUA_NOREF;
  a->self_ref = LUA_NOREF;
  a->deleted = true;
  return 0;
}

static int luavgl_anim_start(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, 1);

  if (a->aa) {
    debug("we have an anim ongoing, stop it.");
    luavgl_anim_stop(L);
  }

  lv_anim_t *new_a = lv_anim_start(&a->cfg);
  a->aa = new_a;
  debug("anim %p, aa: %p\n", a, a->aa);

  if (a->self_ref == LUA_NOREF) {
    /* it's started, thus cannot be gc'ed */
    lua_pushvalue(L, 1);
    a->self_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  }

  return 1;
}

/**
 * anim:set { para }
 * update anim parameter, stop anim if it's ongoing.
 */
static int luavgl_anim_set(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, 1);
  if (a->aa) {
    luavgl_anim_stop(L);
  }

  /* update anim parameters */

  lv_anim_t *cfg = &a->cfg;

  lua_getfield(L, 2, "exec_cb");
  if (!lua_isnoneornil(L, -1)) {
    luaL_unref(L, LUA_REGISTRYINDEX, a->exec_cb);
    a->exec_cb = luavgl_check_continuation(L, -1);
  }
  lua_pop(L, 1);

  lua_getfield(L, 2, "done_cb");
  if (!lua_isnoneornil(L, -1)) {
    luaL_unref(L, LUA_REGISTRYINDEX, a->done_cb);
    a->done_cb = luavgl_check_continuation(L, -1);
  }
  lua_pop(L, 1);

  lua_getfield(L, 2, "run");
  bool run = lua_toboolean(L, -1);
  lua_pop(L, 1);

  luavgl_iterate(L, 2, anim_set_para_cb, cfg);

  if (run) {
    luavgl_anim_start(L);
  }

  return 1;
}

/**
 * a = obj:Anim({anim parameters})
 * a = obj:Anim{anim parameters}
 * a = lvgl.Anim(var, anim_para)
 *
 * a:start()
 */
static int luavgl_anim_create(lua_State *L)
{
  if (lua_isnoneornil(L, 1)) {
    return luaL_argerror(L, 1, "anim var must not be nil or none.");
  }

  if (!lua_istable(L, 2)) {
    return luaL_argerror(L, 2, "expect anim para table.");
  }

  luavgl_anim_t *a = lua_newuserdata(L, sizeof(luavgl_anim_t));
  luaL_getmetatable(L, "lv_anim");
  lua_setmetatable(L, -2);

  lua_pushvalue(L, 1);
  a->obj_ref = luaL_ref(L, LUA_REGISTRYINDEX);
  a->exec_cb = LUA_NOREF;
  a->done_cb = LUA_NOREF;
  a->self_ref = LUA_NOREF;
  a->aa = NULL;
  a->L = L;
  a->deleted = false;

  lv_anim_t *cfg = &a->cfg;
  lv_anim_init(cfg);
  cfg->var = a;
  cfg->deleted_cb = luavgl_anim_delete_cb;
  cfg->exec_cb = luavgl_anim_exec_cb;

  /* leave only anim userdata and para table on stack */
  lua_remove(L, 1); /* para, anim */
  lua_insert(L, 1); /* anim, para */

  luavgl_anim_set(L);
  lua_pop(L, 1); /* anim */

  debug("create anim: %p, aa: %p\n", a, a->aa);
  return 1;
}

static int luavgl_anim_gc(lua_State *L)
{
  debug("\n");
  luavgl_anim_t *a = luaL_checkudata(L, 1, "lv_anim");
  if (a->deleted)
    return 0;

  luavgl_anim_delete(L);
  return 0;
}

static int luavgl_anim_tostring(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, -1);

  lua_pushfstring(
      L, "anim %p: %s, value: [%d..%d], current: %d, repeat_cnt: %d", a->aa,
      a->aa ? "running" : "stopped", a->cfg.start_value, a->cfg.end_value,
      a->aa ? a->aa->current_value : -1, a->cfg.repeat_cnt);
  return 1;
}

static const luaL_Reg luavgl_anim_meta[] = {
    {"__gc",       luavgl_anim_gc      },
    {"__tostring", luavgl_anim_tostring},
    {"__index",    NULL                }, /* place holder */

    {NULL,         NULL                },
};

static const luaL_Reg luavgl_anim_methods[] = {
    {"set",    luavgl_anim_set   },
    {"start",  luavgl_anim_start },

 /* in lua anim, stopped anim can be restarted. */
    {"stop",   luavgl_anim_stop  },
    {"delete", luavgl_anim_delete},

    {NULL,     NULL              }
};

static void luavgl_anim_init(lua_State *L)
{
  /* create metatable */
  luaL_newmetatable(L, "lv_anim");
  luaL_setfuncs(L, luavgl_anim_meta, 0);

  luaL_newlib(L, luavgl_anim_methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */
}
