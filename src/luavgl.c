#include "luavgl.h"
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

static const struct luaL_Reg luavgl_methods[] = {
    {"Timer", luavgl_timer_create}, /* timer.c */
    {"Font",  luavgl_font_create }, /* font.c */
    {"Style", luavgl_style_create}, /* style.c */
    {"Anim",  luavgl_anim_create }, /* anim.c */

    {NULL,    NULL               },
};

LUALIB_API luavgl_ctx_t *luavgl_context(lua_State *L)
{
  static const char *luavglgl_key = "luavgl_key";

  luavgl_ctx_t *ctx;
  lua_pushstring(L, luavglgl_key);
  lua_rawget(L, LUA_REGISTRYINDEX);

  /**
   * ctx = registry[luavgl_key]
   * if ctx == nil then
   *    ctx = new ctx
   * else
   *    return ctx
   * end
   */

  if (lua_isnil(L, -1)) {
    /* create it if not exist in registry */
    lua_pushstring(L, luavglgl_key);
    ctx = (luavgl_ctx_t *)lua_newuserdata(L, sizeof(*ctx));
    memset(ctx, 0, sizeof(*ctx));
    lua_rawset(L, LUA_REGISTRYINDEX);
  } else {
    ctx = (luavgl_ctx_t *)lua_touserdata(L, -1);
  }

  lua_pop(L, 1);
  return ctx;
}

LUALIB_API void luavgl_set_pcall(lua_State *L, luavgl_pcall_t pcall)
{
  luavgl_ctx_t *ctx = luavgl_context(L);
  ctx->pcall = pcall;
}

LUALIB_API void luavgl_set_root(lua_State *L, lv_obj_t *root)
{
  luavgl_ctx_t *ctx = luavgl_context(L);
  ctx->root = root;
}

LUALIB_API void luavgl_set_font_extension(lua_State *L, make_font_cb make,
                                          delete_font_cb d)
{
  luavgl_ctx_t *ctx = luavgl_context(L);
  ctx->make_font = make;
  ctx->delete_font = d;
}

static int root_gc(lua_State *L)
{
  debug("enter.\n");
  luavgl_ctx_t *ctx = luavgl_context(L);
  lv_obj_del(ctx->root);
  return 0;
}

static int root_clean(lua_State *L)
{
  debug("enter.\n");
  luavgl_ctx_t *ctx = luavgl_context(L);
  lv_obj_clean(ctx->root);
  return 0;
}

LUALIB_API int luaopen_lvgl(lua_State *L)
{
  luavgl_ctx_t *ctx = luavgl_context(L);

  luaL_newlib(L, luavgl_methods);

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

  if (ctx->pcall == NULL)
    ctx->pcall = luavgl_pcall;

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

  luavgl_obj_init(L);
  luavgl_widgets_init(L);
  luavgl_anim_init(L);
  luavgl_timer_init(L);
  luavgl_style_init(L);
  luavgl_fs_init(L);
  luavgl_indev_init(L);
  luavgl_group_init(L);
  luavgl_disp_init(L);

  luavgl_constants_init(L);

  /* Methods to create widget locate in widgets table, check `luavgl_obj_init`
   */
  luaL_getmetatable(L, "widgets");
  lua_setmetatable(L, -2);

  (void)dumpstack;
  (void)dumptable;
  (void)dumpvalue;
  return 1;
}
