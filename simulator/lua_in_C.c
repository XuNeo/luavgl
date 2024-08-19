#include "lua.h"
#include <luavgl.h>
#include <lvgl.h>
#include <src/core/lv_obj.h>
#include <src/core/lv_obj_event.h>
#include <src/lv_api_map_v8.h>
#include <src/misc/lv_event.h>

#define _STRINGIZE(...) #__VA_ARGS__
#define STRINGIZE(...) _STRINGIZE(__VA_ARGS__)

static const char lua_code_string[] = {
    /* Must start with -- */
    "--"
#include "ui.lua"
};

static const char button_lua_code[] = STRINGIZE(
    local btn = lvgl.get_child_by_id('Button in Lua')
    local label = btn:get_child(0)
    label.text = 'Button clicked'
);

static void button_clicked(lv_event_t *e)
{
  (void)e;
  lua_State *L = lv_event_get_user_data(e);
  LV_LOG_USER("Button clicked");
  luaL_dostring(L, button_lua_code);
}

static int embed_lua_in_c(lua_State *L)
{
  /* We create a root obj in C and create other UIs in lua. */
  lv_obj_t *root = lv_obj_create(lv_screen_active());
  luavgl_add_lobj(L, root)->lua_created = false;
  lv_obj_set_id(root, "luaUIroot");
  int ret = luaL_dostring(L, lua_code_string);
  if (ret != 0) {
    LV_LOG_USER("luaL_dostring error: %d", ret);
    return -1;
  }

  lv_obj_t *btn = lv_obj_get_child_by_id(root, "Button in Lua");
  if (btn) {
    lv_obj_add_event_cb(btn, button_clicked, LV_EVENT_CLICKED, L);
  }

  return 0;
}

void lua_c_lvgl_mix_example(lua_State *L)
{
  lua_pushcfunction(L, embed_lua_in_c);
  lua_setglobal(L, "embed_lua_in_c");
}
