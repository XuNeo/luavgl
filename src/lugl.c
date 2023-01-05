#include <lauxlib.h>
#include <lua.h>

#include <stdio.h>

#include "lugl.h"
#include "private.h"

#include "anim.c"
#include "constants.c"
#include "font.c"
#include "obj.c"
#include "timer.c"
#include "util.c"

static const struct luaL_Reg lugl_functions[] = {
    /* widget create using lvgl.table, has parent of root */
    WIDGET_CREATE_FUNCTIONS,

#if 0 //currently, only obj can create anim
    { "Anim", lugl_anim_create },
#endif

    { "Timer", lugl_timer_create },
    { "Font", lugl_font_create },
    { "Style", lugl_style_create },

    { NULL, NULL },
};

static const luaL_Reg lugl_anim_methods[] = {
    // anim.c
    { "set", lugl_anim_set },
    { "start", lugl_anim_start },
    { "stop", lugl_anim_stop },
    { "delete", lugl_anim_delete },

    { NULL, NULL }
};

static const luaL_Reg lugl_timer_methods[] = {
    // timer.c
    { "set", lugl_timer_set },
    { "pause", lugl_timer_pause },
    { "resume", lugl_timer_resume },
    { "delete", lugl_timer_delete },
    { "ready", lugl_timer_ready },

    { NULL, NULL }
};

static const luaL_Reg lugl_style_methods[] = {
    // style.c
    { "set", lugl_style_set },
    { "delete", lugl_style_delete },
    { "remove_prop", lugl_style_remove_prop },

    { NULL, NULL }
};

static void lugl_anim_init(lua_State* L)
{
    luaL_newmetatable(L, "lv_anim");

    lua_pushcfunction(L, lugl_anim_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pushcfunction(L, lugl_anim_gc);
    lua_setfield(L, -2, "__gc");

    luaL_newlib(L, lugl_anim_methods); /* methods belong to this type */
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1); /* pop __index table */
}

static void lugl_timer_init(lua_State* L)
{
    luaL_newmetatable(L, "lv_timer");

    lua_pushcfunction(L, lugl_timer_gc);
    lua_setfield(L, -2, "__gc");

    luaL_newlib(L, lugl_timer_methods); /* methods belong to this type */
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1); /* pop __index table */
}

static void lugl_style_init(lua_State* L)
{
    luaL_newmetatable(L, "lv_style");

    lua_pushcfunction(L, lugl_style_gc);
    lua_setfield(L, -2, "__gc");

    luaL_newlib(L, lugl_style_methods); /* methods belong to this type */
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1); /* pop __index table */
}

LUALIB_API lugl_ctx_t* lugl_context(lua_State* L)
{
    static const char* lualvgl_key = "lugl_key";

    lugl_ctx_t* ctx;
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
        ctx = (lugl_ctx_t*)lua_newuserdata(L, sizeof(*ctx));
        memset(ctx, 0, sizeof(*ctx));
        lua_rawset(L, LUA_REGISTRYINDEX);
    } else {
        ctx = (lugl_ctx_t*)lua_touserdata(L, -1);
    }

    lua_pop(L, 1);
    return ctx;
}

LUALIB_API void lugl_set_root(lua_State* L, lv_obj_t* root)
{
    lugl_ctx_t* ctx = lugl_context(L);
    ctx->root = root;
}

LUALIB_API void lugl_set_font_extension(lua_State* L, make_font_cb make,
                                        delete_font_cb d)
{
    lugl_ctx_t* ctx = lugl_context(L);
    ctx->make_font = make;
    ctx->delete_font = d;
}

static int root_gc(lua_State* L)
{
  debug("enter.\n");
  lugl_ctx_t* ctx = lugl_context(L);
  lv_obj_del(ctx->root);
  return 0;
}

static int root_clean(lua_State* L)
{
  debug("enter.\n");
  lugl_ctx_t* ctx = lugl_context(L);
  lv_obj_clean(ctx->root);
  return 0;
}

int luaopen_lugl(lua_State* L)
{
    lugl_ctx_t* ctx = lugl_context(L);

    luaL_newlib(L, lugl_functions);

    /* similar to luv, setup a metatable to root, for __gc */
    luaL_newmetatable(L, "root.meta");
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, ctx->root ? root_clean : root_gc);
    lua_settable(L, -3);
    lua_pop(L, 1);

    lv_obj_t* root = ctx->root;
    if (root == NULL) {
        debug("create root obj for lua.\n");
        root = lv_obj_create(lv_scr_act());
        ctx->root = root;
    }

    lua_pushstring(L, "_root");
    *(void**)lua_newuserdata(L, sizeof(void*)) = root;
    // setup the userdata's metatable for __gc
    luaL_getmetatable(L, "root.meta");
    lua_setmetatable(L, -2);
    // create a ref to root, avoid __gc early
    // this puts the root userdata into the _root key
    // in the returned luv table
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

    lugl_constants_init(L);

    (void)dumpstack;
    (void)dumptable;
    (void)dumpvalue;
    return 1;
}
