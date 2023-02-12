#include <lauxlib.h>
#include <lua.h>

#include <stdio.h>

#include "lugl.h"
#include "private.h"

#include "anim.c"
#include "constants.c"
#include "disp.c"
#include "font.c"
#include "fs.c"
#include "group.c"
#include "indev.c"
#include "obj.c"
#include "timer.c"
#include "util.c"

static const struct luaL_Reg lugl_methods[] = {
    {"Timer", lugl_timer_create}, /* timer.c */
    {"Font",  lugl_font_create }, /* font.c */
    {"Style", lugl_style_create}, /* style.c */

    {NULL,    NULL             },
};

LUALIB_API lugl_ctx_t *lugl_context(lua_State *L)
{
  static const char *lualvgl_key = "lugl_key";

  lugl_ctx_t *ctx;
  lua_pushstring(L, lualvgl_key);
  lua_rawget(L, LUA_REGISTRYINDEX);

  /**
   * ctx = registry[lugl_key]
   * if ctx == nil then
   *    ctx = new ctx
   * else
   *    return ctx
   * end
   */

  if (lua_isnil(L, -1)) {
    // create it if not exist in registry
    lua_pushstring(L, lualvgl_key);
    ctx = (lugl_ctx_t *)lua_newuserdata(L, sizeof(*ctx));
    memset(ctx, 0, sizeof(*ctx));
    lua_rawset(L, LUA_REGISTRYINDEX);
  } else {
    ctx = (lugl_ctx_t *)lua_touserdata(L, -1);
  }

  lua_pop(L, 1);
  return ctx;
}

LUALIB_API void lugl_set_root(lua_State *L, lv_obj_t *root)
{
  lugl_ctx_t *ctx = lugl_context(L);
  ctx->root = root;
}

LUALIB_API void lugl_set_font_extension(lua_State *L, make_font_cb make,
                                        delete_font_cb d)
{
  lugl_ctx_t *ctx = lugl_context(L);
  ctx->make_font = make;
  ctx->delete_font = d;
}

static int root_gc(lua_State *L)
{
  debug("enter.\n");
  lugl_ctx_t *ctx = lugl_context(L);
  lv_obj_del(ctx->root);
  return 0;
}

static int root_clean(lua_State *L)
{
  debug("enter.\n");
  lugl_ctx_t *ctx = lugl_context(L);
  lv_obj_clean(ctx->root);
  return 0;
}

LUALIB_API int luaopen_lugl(lua_State *L)
{
  lugl_ctx_t *ctx = lugl_context(L);

  luaL_newlib(L, lugl_methods);

  luaL_newmetatable(L, "root.meta");
  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, ctx->root ? root_clean : root_gc);
  lua_settable(L, -3);
  lua_pop(L, 1);

  lv_obj_t *root = ctx->root;
  if (root == NULL) {
    debug("create root obj for lua.\n");
    root = lv_obj_create(lv_scr_act());
    ctx->root = root;
  }

  lua_pushstring(L, "_root");
  *(void **)lua_newuserdata(L, sizeof(void *)) = root;
  luaL_getmetatable(L, "root.meta");
  lua_setmetatable(L, -2);

  /*
   * create a ref to root, avoid __gc early
   * this puts the root userdata into the _root key
   * in the returned luv table
   * */
  lua_rawset(L, -3);

  lv_obj_remove_style_all(root);
  lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

  lugl_obj_init(L);
  lugl_label_init(L);
  lugl_widgets_init(L);
  lugl_anim_init(L);
  lugl_timer_init(L);
  lugl_style_init(L);
  lugl_fs_init(L);
  lugl_indev_init(L);
  lugl_group_init(L);
  lugl_disp_init(L);

  lugl_constants_init(L);

  /* Methods to create widget locate in widgets table, check `lugl_obj_init` */
  luaL_getmetatable(L, "widgets");
  lua_setmetatable(L, -2);

  (void)dumpstack;
  (void)dumptable;
  (void)dumpvalue;
  return 1;
}
