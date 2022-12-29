#include <lauxlib.h>
#include <lua.h>

#include <stdio.h>

#include "lugl.h"
#include "private.h"

#include "anim.c"
#include "constants.c"
#include "event.c"
#include "font.c"
#include "obj.c"
#include "style.c"
#include "timer.c"
#include "util.c"

#include "widgets/widgets.c"

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

static const luaL_Reg lugl_obj_methods[] = {
    /* widget created using any_obj:Object() has parent of any_obj */
    WIDGET_CREATE_FUNCTIONS,

    { "set", lugl_obj_set },
    { "set_style", lugl_obj_set_style },
    { "align_to", lugl_obj_align_to },
    { "delete", lugl_obj_delete },
    { "clean", lugl_obj_clean },

    /* misc. functions */
    { "set_parent", lugl_obj_set_parent },
    { "get_parent", lugl_obj_get_parent },
    { "get_screen", lugl_obj_get_screen },
    { "get_state", lugl_obj_get_state },
    { "scroll_to", lugl_obj_scroll_to },
    { "is_scrolling", lugl_obj_is_scrolling },
    { "is_visible", lugl_obj_is_visible },
    { "add_flag", lugl_obj_add_flag },
    { "clear_flag", lugl_obj_clear_flag },
    { "add_style", lugl_obj_add_style },
    { "remove_style", lugl_obj_remove_style },
    { "remove_style_all", lugl_obj_remove_style_all },
    { "scroll_by", lugl_obj_scroll_by },
    { "scroll_by_bounded", lugl_obj_scroll_by_bounded },
    { "scroll_to_view", lugl_obj_scroll_to_view },
    { "scroll_to_view_recursive", lugl_obj_scroll_to_view_recursive },
    { "scroll_by_raw", lugl_obj_scroll_by_raw },
    { "scrollbar_invalidate", lugl_obj_scrollbar_invalidate },
    { "readjust_scroll", lugl_obj_readjust_scroll },
    { "is_editable", lugl_obj_is_editable },
    { "is_group_def", lugl_obj_is_group_def },
    { "is_layout_positioned", lugl_obj_is_layout_positioned },
    { "mark_layout_as_dirty", lugl_obj_mark_layout_as_dirty },
    { "center", lugl_obj_center },
    { "invalidate", lugl_obj_invalidate },
    { "set_flex_flow", lugl_obj_set_flex_flow },
    { "set_flex_align", lugl_obj_set_flex_align },
    { "set_flex_grow", lugl_obj_set_flex_grow },

    { "onevent", lugl_obj_on_event },
    { "onPressed", lugl_obj_on_pressed },
    { "onClicked", lugl_obj_on_clicked },
    { "anim", lugl_anim_create }, /* in lua, we only support add anim to obj */
    { "anims", lugl_anims_create }, /* create multiple anim */
    { "remove_all_anim", lugl_obj_remove_all_anim }, /* remove all */
    { NULL, NULL },
};

static void lugl_new_objlib(lua_State* L)
{
  luaL_newlib(L, lugl_obj_methods);
}

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

static void dumpstack(lua_State* L)
{
    int top = lua_gettop(L);
    printf("\n");
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            printf("number: %g\n", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            printf("string: %s\n", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf("boolean: %s\n", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("nil: %s\n", "nil");
            break;
        default:
            printf("pointer: %p\n", lua_topointer(L, i));
            break;
        }
    }
    fflush(stdout);
}

static void lugl_obj_init(lua_State* L)
{
    /* base lv_obj */
    luaL_newmetatable(L, "lv_obj");
    lua_pushcfunction(L, lugl_obj_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction(L, lugl_obj_gc);
    lua_setfield(L, -2, "__gc");
    luaL_newlib(L, lugl_obj_methods); /* methods belong to this type */
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

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

int luaopen_lugl(lua_State* L)
{
    lugl_ctx_t* ctx = lugl_context(L);

    luaL_newlib(L, lugl_functions);

    if (ctx->root == NULL) {
        lv_obj_t* root;
        debug("create root obj for lua.\n");
        root = lv_obj_create(lv_scr_act());

        /* similar to luv, setup a metatable to root, for __gc */
        luaL_newmetatable(L, "root.meta");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, root_gc);
        lua_settable(L, -3);
        lua_pop(L, 1);

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

        ctx->root = root;
    }

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
