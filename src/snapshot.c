#include "luavgl.h"

static int luavgl_draw_buf_create(lua_State *L, lv_draw_buf_t *buf);

static int luavgl_obj_snapshot(lua_State *L)
{
  luavgl_obj_t *lobj = luavgl_to_lobj(L, 1);

  lv_color_format_t cf = LV_COLOR_FORMAT_ARGB8888;
  if (!lua_isnoneornil(L, 2)) {
    cf = (lv_color_format_t)lua_tointeger(L, 2);
  }
  lv_draw_buf_t *buf = lv_snapshot_take(lobj->obj, cf);
  return luavgl_draw_buf_create(L, buf);
}
