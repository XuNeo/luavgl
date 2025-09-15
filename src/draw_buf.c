#include "luavgl.h"
#include "private.h"
#include "rotable.h"

typedef struct luavgl_draw_buf_s {
  lv_draw_buf_t *buf;
} luavgl_draw_buf_t;

static int luavgl_draw_buf_gc(lua_State *L)
{
  luavgl_draw_buf_t *db = luaL_checkudata(L, 1, "lv_draw_buf");
  lv_draw_buf_destroy(db->buf);
  return 0;
}

static int luavgl_draw_buf_create(lua_State *L, lv_draw_buf_t *buf)
{
  luavgl_draw_buf_t *db = lua_newuserdata(L, sizeof(luavgl_draw_buf_t));
  db->buf = buf;
  luaL_setmetatable(L, "lv_draw_buf");
  return 1;
}

static const luaL_Reg draw_buf_meta[] = {
    {"__gc", luavgl_draw_buf_gc},
    {},
};

static void luavgl_draw_buf_init(lua_State *L)
{
  luaL_newmetatable(L, "lv_draw_buf");
  luaL_setfuncs(L, draw_buf_meta, 0);
  lua_pop(L, 1);
}
