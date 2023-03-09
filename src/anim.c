#include <assert.h>

#include "luavgl.h"
#include "private.h"

typedef struct luavgl_anim_s {
  lv_anim_t *aa; /* the handler returns be lv_anim_start */
  lv_anim_t cfg; /* the configuration, must not be the first element. */
  lv_obj_t *obj;
  lua_State *L;
  int ref; /* execute callback to reference. */

  /* anim configured but not started. lv_anim does't support pause anim, this
   * is done by keep a lv_anim_t backup, thus we can re-start the anim any
   * time.
   *
   * deleted = ref == LUA_NOREF
   */
  bool paused;
} luavgl_anim_t;

typedef luavgl_anim_t *luavgl_anim_handle_t;

static luavgl_anim_t *luavgl_check_anim(lua_State *L, int index)
{
  luavgl_anim_t *v = *(luavgl_anim_t **)luaL_checkudata(L, index, "lv_anim");
  return v;
}

static void luavgl_anim_exec_cb(void *var, int32_t value)
{
  luavgl_anim_t *a = var;
  lua_State *L = a->L;
  int ref = a->ref;

  if (ref == LUA_NOREF) {
    debug("anim error, no callback found.\n");
    luaL_error(L, "anim founds no cb.");
    return;
  }

  /* stack: 1. function, 2. obj-userdata, 3. value */
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  lua_pushlightuserdata(L, a->obj);
  lua_rawget(L, LUA_REGISTRYINDEX); /* this should not fail*/
  lua_pushinteger(L, value);

  luavgl_pcall(L, 2, 0);
}

static inline void luavgl_obj_remove_anim(lua_State *L, luavgl_obj_t *lobj,
                                          luavgl_anim_t *a)
{
  if (a->ref == LUA_NOREF) {
    /* no callback assigned to it, it must be deleted. */
    return;
  }

  if (a->aa) {
    lv_anim_del(a->cfg.var, luavgl_anim_exec_cb);
  } else {
    debug("anim %p already stopped.\n", a);
  }

  debug("remove anim: %p, var:%p\n", a, a->cfg.var);

  luaL_unref(L, LUA_REGISTRYINDEX, a->ref);

  a->ref = LUA_NOREF;

  /* change anim handle in lobj->anims to NULL */

  /* find a slot to store this anim */
  int slot = 0;
  for (; slot < lobj->n_anim; slot++) {
    luavgl_anim_handle_t anim = lobj->anims[slot];
    if (anim == a) {
      lobj->anims[slot] = NULL;
      return;
    }
  }

  debug("cannot find anim in lobj->anims.\n");
}

/* could be called: 1. manually delete anim. 2. anim done, 3. manually stop
 * anim.
 */
static void luavgl_anim_delete_cb(lv_anim_t *_a)
{
  luavgl_anim_t *a = _a->var;
  a->aa = NULL; /* handler deleted by lvgl */

  /* deleted or paused. if a->ref == NOREF, it's deleted */
  a->paused = true;

  lua_State *L = a->L;

  /* it's paused, thus can be gc'ed */
  lua_pushlightuserdata(L, a);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);
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
    { "repeat_count", 0, { .setter = (setter_int_t)lv_anim_set_repeat_count } },
    { "repeat_delay", 0, { .setter = (setter_int_t)lv_anim_set_repeat_delay } },
    { "early_apply", 0, { .setter = (setter_int_t)lv_anim_set_early_apply } },
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

static inline void _luavgl_anim_start(lua_State *L, luavgl_anim_t *a)
{
  lv_anim_t *new_a = lv_anim_start(&a->cfg);
  a->aa = new_a;
  debug("anim %p, aa: %p\n", a, a->aa);
  a->paused = false; /* running */

  /* it's started, thus cannot be gc'ed */
  lua_pushvalue(L, -1);
  lua_pushlightuserdata(L, a);
  lua_rawset(L, LUA_REGISTRYINDEX);
}

/**
 * setup anim, parameter is in stack[2] and is a table.
 * {
 *   .run = true,
 *   .start_value = 123,
 *   .end_value = 456,
 *   .time = 1000, -- duration time, in ms
 *   .repeat_count = 10, -- repeat count
 *   .repeat_delay = 10,
 *   .exec_cb = function
 *   .path = "linear" -- only builtin function supported for now. support
 *                    -- linear, ease_in, ease_out,
 *                    -- ease_in_out, overshoot, bounce, step
 * }
 */
static inline int luavgl_anim_setup(lua_State *L, lv_obj_t *obj,
                                    luavgl_anim_t *anim)
{
  lv_anim_t *cfg = &anim->cfg;

  *(void **)lua_newuserdata(L, sizeof(void *)) = anim;

  luaL_getmetatable(L, "lv_anim");
  lua_setmetatable(L, -2);

  anim->aa = NULL;
  anim->L = L;
  anim->obj = obj;
  anim->ref = LUA_NOREF;
  anim->paused = true;

  lv_anim_init(cfg);
  cfg->var = anim; /* var: luavgl_anim_t, which contains everything we need */
  cfg->deleted_cb = luavgl_anim_delete_cb;
  cfg->exec_cb = luavgl_anim_exec_cb;

  /* check exec_cb firstly, since it could fail. */
  lua_getfield(L, -2, "exec_cb");
  anim->ref = luavgl_check_continuation(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -2, "run");
  bool run = lua_toboolean(L, -1);
  lua_pop(L, 1);

  luavgl_iterate(L, -2, anim_set_para_cb, cfg);

  /* different to obj events and styles, return userdata so lua can control
   * anim using start/stop etc. */

  lua_rotate(L, 1, 1); /* [obj,para-table,anim] --> [anim,obj,para-table] */
  lua_pop(L, 2);       /* remove obj and parameter table */

  if (run) {
    _luavgl_anim_start(L, anim);
  } else {
    /* added to registry any way to avoid early gc */
    lua_pushvalue(L, -1);
    lua_pushlightuserdata(L, anim);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }

  debug("create anim: %p, aa: %p\n", anim, anim->aa);
  return 1;
}

/**
 * a = obj:anim({anim parameters})
 * a = obj:anim{anim parameters}
 * a:start()
 */
static int luavgl_anim_create(lua_State *L)
{
  bool remove_all; /* if third parameter is noneornil, remove all anims. */

  luavgl_obj_t *lobj = luavgl_to_lobj(L, 1);
  lv_obj_t *obj = lobj->obj;
  if (obj == NULL) {
    return luaL_argerror(L, 1, "expect obj userdata.\n");
  }

  remove_all = lua_isnoneornil(L, 2);
  if (!remove_all && !lua_istable(L, 2)) {
    luaL_argerror(L, 2, "expect anim para in table");
  }

  /* find a slot to store this anim */
  int slot = 0;
  if (lobj && lobj->anims) {
    for (; slot < lobj->n_anim; slot++) {
      luavgl_anim_handle_t anim = lobj->anims[slot];
      if (remove_all) {
        luavgl_obj_remove_anim(L, lobj, anim);
      } else if (anim == NULL) {
        /* this callback has been removed, thus, we can use this
         * slot */
        break;
      }
    }
  }

  if (remove_all) /* no need to add, just return */
    return 0;

  luavgl_anim_handle_t *anims = lobj->anims;

  /* create lobj->anims, if NULL, realloc if existing and find no slot
   */
  if (anims == NULL) {
    anims = calloc(sizeof(luavgl_anim_handle_t), 1);
    lobj->anims = anims;
    lobj->n_anim = 1;
  } else {
    /* realloc? */
    if (slot && slot == lobj->n_anim) {
      luavgl_anim_handle_t *_anims;
      _anims = realloc(lobj->anims, (lobj->n_anim + 1) * sizeof(*_anims));
      if (_anims == NULL) {
        return luaL_error(L, "No memory.");
      }
      anims = _anims;
      lobj->n_anim++; /* now we have +1 anim */
      lobj->anims = anims;
    }
    /* else: we have found a slot to reuse, use it. */
  }

  luavgl_anim_t *a = malloc(sizeof(*a));
  if (a == NULL) {
    return luaL_error(L, "no memory.");
  }

  anims[slot] = a;

  return luavgl_anim_setup(L, obj, a);
}

/**
 * a = obj:anims{{anim1 para}, {anim2 para}}
 */
static int luavgl_anims_create(lua_State *L)
{
  /* two argument expected: obj, table{table} */
  if (!lua_istable(L, 2)) {
    return luaL_argerror(L, 2, "expect anim param table");
  }

  lua_newtable(L); /* create a table to store created anims */
  /* the luavgl_anim_create expect obj + table */
  int len = lua_rawlen(L, 2); /* number of anims */
  int status;

  for (int i = 1; i <= len; i++) {
    /* stack now: obj, table{table}, {result} */
    lua_pushcfunction(L, luavgl_anim_create);
    lua_pushvalue(L, 1);  /* obj */
    lua_rawgeti(L, 2, i); /* get the parameter table */
    status = lua_pcall(L, 2, 1, 0);
    if (status != LUA_OK) {
      return luaL_error(L, "setup anim %d failed.", i);
    }
    /* stack now: obj, table{table}, {result}, anim */
    lua_rawseti(L, -2, i);
  }

  return 1;
}

/* anim:set({}) */
static int luavgl_anim_set(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, 1);

  if (!a->paused) {
    return luaL_error(L, "cannot change para while running.");
  }

  /* check exec_cb firstly, since it could fail. */
  lua_getfield(L, 2, "exec_cb");
  if (!lua_isnoneornil(L, -1)) {
    /* update exec_cb */
    int ref = luavgl_check_continuation(L, -1);
    if (a->ref != LUA_NOREF) {
      luaL_unref(L, LUA_REGISTRYINDEX, a->ref);
    }
    a->ref = ref;
  }
  lua_pop(L, 1);

  lua_getfield(L, 2, "run");
  bool run = lua_toboolean(L, -1);
  lua_pop(L, 1);

  luavgl_iterate(L, 2, anim_set_para_cb, &a->cfg);

  if (run)
    _luavgl_anim_start(L, a);

  return 0;
}

static void luavgl_obj_anim_init(luavgl_obj_t *lobj)
{
  lobj->anims = NULL;
  lobj->n_anim = 0;
}

static int luavgl_obj_remove_all_anim_int(lua_State *L, luavgl_obj_t *lobj)
{
  debug("obj: %p\n", lobj->obj);

  if (lobj == NULL || lobj->anims == NULL) {
    return 0;
  }

  int i = 0;
  for (; i < lobj->n_anim; i++) {
    luavgl_anim_handle_t anim = lobj->anims[i];
    if (anim != NULL)
      luavgl_obj_remove_anim(L, lobj, anim);
    /* else, it's an empty slot, anim already removed */
  }

  free(lobj->anims);
  lobj->n_anim = 0;
  lobj->anims = NULL;

  return 0;
}

/**
 * Remove all anims added, and free memory of anims
 */
static int luavgl_obj_remove_all_anim(lua_State *L)
{
  debug("\n");
  luavgl_obj_t *lobj = luavgl_to_lobj(L, 1);

  luavgl_obj_remove_all_anim_int(L, lobj);
  return 0;
}

static int luavgl_anim_start(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, -1);

  if (!a->paused) {
    debug("already running.\n");
    return 0;
  }

  a->aa = lv_anim_start(&a->cfg);
  a->paused = false;

  /* it's started, thus cannot be gc'ed */
  // lua_pushvalue(L, -1); /* already in stack */
  lua_pushlightuserdata(L, a);
  lua_rawset(L, LUA_REGISTRYINDEX);
  return 0;
}

static int luavgl_anim_stop(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, -1);

  if (a->paused) {
    debug("already stopped");
    return 0;
  }

  lv_anim_del(a->cfg.var, luavgl_anim_exec_cb);

  /* work done in luavgl_anim_delete_cb */
  return 0;
}

/* remove anim from obj,  */
static int luavgl_anim_delete(lua_State *L)
{
  debug("\n");
  luavgl_anim_t *a = luavgl_check_anim(L, -1);

  lua_pushlightuserdata(L, a->obj);
  lua_rawget(L, LUA_REGISTRYINDEX);
  luavgl_obj_t *lobj = luavgl_to_lobj(L, -1);
  lua_pop(L, 1);
  luavgl_obj_remove_anim(L, lobj, a);

  return 0;
}

static int luavgl_anim_gc(lua_State *L)
{
  debug("\n");

  luavgl_anim_t *a = luavgl_check_anim(L, -1);

  /* stop anim if not stopped. */
  luavgl_anim_delete(L);

  free(a);
  return 0;
}

static int luavgl_anim_tostring(lua_State *L)
{
  luavgl_anim_t *a = luavgl_check_anim(L, -1);

  lua_pushfstring(L,
                  "anim %p: %s, value: [%d..%d], current: %d, repeat_cnt: %d",
                  a->aa, a->paused ? "paused" : "running", a->cfg.start_value,
                  a->cfg.end_value, a->cfg.current_value, a->cfg.repeat_cnt);
  return 1;
}

static const luaL_Reg luavgl_anim_methods[] = {
  // anim.c
    {"set",    luavgl_anim_set   },
    {"start",  luavgl_anim_start },
    {"stop",   luavgl_anim_stop  },
    {"delete", luavgl_anim_delete},

    {NULL,     NULL              }
};

static void luavgl_anim_init(lua_State *L)
{
  luaL_newmetatable(L, "lv_anim");

  lua_pushcfunction(L, luavgl_anim_tostring);
  lua_setfield(L, -2, "__tostring");

  lua_pushcfunction(L, luavgl_anim_gc);
  lua_setfield(L, -2, "__gc");

  luaL_newlib(L, luavgl_anim_methods); /* methods belong to this type */
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
