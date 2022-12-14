#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

typedef enum {
  STYLE_TYPE_INT = 1,
  STYLE_TYPE_COLOR,
  STYLE_TYPE_POINTER, /* a pointer from light-userdata */
  STYLE_TYPE_IMGSRC,  /* string or light-userdata, similar to pointer */

  /* Flag of none-simple type. All extended styles are special case. */
  STYLE_TYPE_SPECIAL = 0x10,
} style_type_t;

enum {
  LV_STYLE_SIZE = _LV_STYLE_LAST_BUILT_IN_PROP + 1,
  LV_STYLE_PAD_ALL,
  LV_STYLE_PAD_VER,
  LV_STYLE_PAD_HOR,
  LV_STYLE_PAD_GAP,
};

static const struct style_map_s {
  const char *name;
  lv_style_prop_t style;
  style_type_t type;
} g_style_map[] = {
    {"width", LV_STYLE_WIDTH, STYLE_TYPE_INT},
    {"min_width", LV_STYLE_MIN_WIDTH, STYLE_TYPE_INT},
    {"max_width", LV_STYLE_MAX_WIDTH, STYLE_TYPE_INT},
    {"height", LV_STYLE_HEIGHT, STYLE_TYPE_INT},
    {"min_height", LV_STYLE_MIN_HEIGHT, STYLE_TYPE_INT},
    {"max_height", LV_STYLE_MAX_HEIGHT, STYLE_TYPE_INT},
    {"x", LV_STYLE_X, STYLE_TYPE_INT},
    {"y", LV_STYLE_Y, STYLE_TYPE_INT},
    {"align", LV_STYLE_ALIGN, STYLE_TYPE_INT},
    {"transform_width", LV_STYLE_TRANSFORM_WIDTH, STYLE_TYPE_INT},
    {"transform_height", LV_STYLE_TRANSFORM_HEIGHT, STYLE_TYPE_INT},
    {"translate_x", LV_STYLE_TRANSLATE_X, STYLE_TYPE_INT},
    {"translate_y", LV_STYLE_TRANSLATE_Y, STYLE_TYPE_INT},
    {"transform_zoom", LV_STYLE_TRANSFORM_ZOOM, STYLE_TYPE_INT},
    {"transform_angle", LV_STYLE_TRANSFORM_ANGLE, STYLE_TYPE_INT},
    {"transform_pivot_x", LV_STYLE_TRANSFORM_PIVOT_X, STYLE_TYPE_INT},
    {"transform_pivot_y", LV_STYLE_TRANSFORM_PIVOT_Y, STYLE_TYPE_INT},
    {"pad_top", LV_STYLE_PAD_TOP, STYLE_TYPE_INT},
    {"pad_bottom", LV_STYLE_PAD_BOTTOM, STYLE_TYPE_INT},
    {"pad_left", LV_STYLE_PAD_LEFT, STYLE_TYPE_INT},
    {"pad_right", LV_STYLE_PAD_RIGHT, STYLE_TYPE_INT},
    {"pad_row", LV_STYLE_PAD_ROW, STYLE_TYPE_INT},
    {"pad_column", LV_STYLE_PAD_COLUMN, STYLE_TYPE_INT},
    {"pad_gap", LV_STYLE_PAD_GAP, STYLE_TYPE_INT},
    {"bg_color", LV_STYLE_BG_COLOR, STYLE_TYPE_COLOR},
    {"bg_opa", LV_STYLE_BG_OPA, STYLE_TYPE_INT},
    {"bg_grad_color", LV_STYLE_BG_GRAD_COLOR, STYLE_TYPE_COLOR},
    {"bg_grad_dir", LV_STYLE_BG_GRAD_DIR, STYLE_TYPE_INT},
    {"bg_main_stop", LV_STYLE_BG_MAIN_STOP, STYLE_TYPE_INT},
    {"bg_grad_stop", LV_STYLE_BG_GRAD_STOP, STYLE_TYPE_INT},
    {"bg_dither_mode", LV_STYLE_BG_DITHER_MODE, STYLE_TYPE_INT},
    {"bg_img_src", LV_STYLE_BG_IMG_SRC, STYLE_TYPE_IMGSRC},
    {"bg_img_opa", LV_STYLE_BG_IMG_OPA, STYLE_TYPE_INT},
    {"bg_img_recolor", LV_STYLE_BG_IMG_RECOLOR, STYLE_TYPE_COLOR},
    {"bg_img_recolor_opa", LV_STYLE_BG_IMG_RECOLOR_OPA, STYLE_TYPE_INT},
    {"bg_img_tiled", LV_STYLE_BG_IMG_TILED, STYLE_TYPE_INT},
    {"border_color", LV_STYLE_BORDER_COLOR, STYLE_TYPE_COLOR},
    {"border_opa", LV_STYLE_BORDER_OPA, STYLE_TYPE_INT},
    {"border_width", LV_STYLE_BORDER_WIDTH, STYLE_TYPE_INT},
    {"border_side", LV_STYLE_BORDER_SIDE, STYLE_TYPE_INT},
    {"border_post", LV_STYLE_BORDER_POST, STYLE_TYPE_INT},
    {"outline_width", LV_STYLE_OUTLINE_WIDTH, STYLE_TYPE_INT},
    {"outline_color", LV_STYLE_OUTLINE_COLOR, STYLE_TYPE_COLOR},
    {"outline_opa", LV_STYLE_OUTLINE_OPA, STYLE_TYPE_INT},
    {"outline_pad", LV_STYLE_OUTLINE_PAD, STYLE_TYPE_INT},
    {"shadow_width", LV_STYLE_SHADOW_WIDTH, STYLE_TYPE_INT},
    {"shadow_ofs_x", LV_STYLE_SHADOW_OFS_X, STYLE_TYPE_INT},
    {"shadow_ofs_y", LV_STYLE_SHADOW_OFS_Y, STYLE_TYPE_INT},
    {"shadow_spread", LV_STYLE_SHADOW_SPREAD, STYLE_TYPE_INT},
    {"shadow_color", LV_STYLE_SHADOW_COLOR, STYLE_TYPE_COLOR},
    {"shadow_opa", LV_STYLE_SHADOW_OPA, STYLE_TYPE_INT},
    {"img_opa", LV_STYLE_IMG_OPA, STYLE_TYPE_INT},
    {"img_recolor", LV_STYLE_IMG_RECOLOR, STYLE_TYPE_COLOR},
    {"img_recolor_opa", LV_STYLE_IMG_RECOLOR_OPA, STYLE_TYPE_INT},
    {"line_width", LV_STYLE_LINE_WIDTH, STYLE_TYPE_INT},
    {"line_dash_width", LV_STYLE_LINE_DASH_WIDTH, STYLE_TYPE_INT},
    {"line_dash_gap", LV_STYLE_LINE_DASH_GAP, STYLE_TYPE_INT},
    {"line_rounded", LV_STYLE_LINE_ROUNDED, STYLE_TYPE_INT},
    {"line_color", LV_STYLE_LINE_COLOR, STYLE_TYPE_INT},
    {"line_opa", LV_STYLE_LINE_OPA, STYLE_TYPE_INT},
    {"arc_width", LV_STYLE_ARC_WIDTH, STYLE_TYPE_INT},
    {"arc_img_src", LV_STYLE_ARC_IMG_SRC, STYLE_TYPE_IMGSRC},
    {"arc_rounded", LV_STYLE_ARC_ROUNDED, STYLE_TYPE_INT},
    {"arc_color", LV_STYLE_ARC_COLOR, STYLE_TYPE_COLOR},
    {"arc_opa", LV_STYLE_ARC_OPA, STYLE_TYPE_INT},
    {"text_color", LV_STYLE_TEXT_COLOR, STYLE_TYPE_COLOR},
    {"text_opa", LV_STYLE_TEXT_OPA, STYLE_TYPE_INT},
    {"text_font", LV_STYLE_TEXT_FONT, STYLE_TYPE_POINTER}, /* light-userdata */
    {"text_letter_space", LV_STYLE_TEXT_LETTER_SPACE, STYLE_TYPE_INT},
    {"text_line_space", LV_STYLE_TEXT_LINE_SPACE, STYLE_TYPE_INT},
    {"text_decor", LV_STYLE_TEXT_DECOR, STYLE_TYPE_INT},
    {"text_align", LV_STYLE_TEXT_ALIGN, STYLE_TYPE_INT},
    {"radius", LV_STYLE_RADIUS, STYLE_TYPE_INT},
    {"clip_corner", LV_STYLE_CLIP_CORNER, STYLE_TYPE_INT},
    {"opa", LV_STYLE_OPA, STYLE_TYPE_INT},
    {"color_filter_opa", LV_STYLE_COLOR_FILTER_OPA, STYLE_TYPE_INT},
    {"anim_time", LV_STYLE_ANIM_TIME, STYLE_TYPE_INT},
    {"anim_speed", LV_STYLE_ANIM_SPEED, STYLE_TYPE_INT},
    {"blend_mode", LV_STYLE_BLEND_MODE, STYLE_TYPE_INT},
    {"layout", LV_STYLE_LAYOUT, STYLE_TYPE_INT},
    {"base_dir", LV_STYLE_BASE_DIR, STYLE_TYPE_INT},

    /* need to build pointer from table parameter */
    {"bg_grad", LV_STYLE_BG_GRAD, STYLE_TYPE_SPECIAL}, /* pointer from table */
    {"color_filter_dsc", LV_STYLE_COLOR_FILTER_DSC, STYLE_TYPE_SPECIAL}, /**/
    {"anim", LV_STYLE_ANIM, STYLE_TYPE_SPECIAL},             /* anim para */
    {"transition", LV_STYLE_TRANSITION, STYLE_TYPE_SPECIAL}, /* transition */

    /* styles combined */
    {"size", LV_STYLE_SIZE, STYLE_TYPE_SPECIAL | STYLE_TYPE_INT},
    {"pad_all", LV_STYLE_PAD_ALL, STYLE_TYPE_SPECIAL | STYLE_TYPE_INT},
    {"pad_ver", LV_STYLE_PAD_VER, STYLE_TYPE_SPECIAL | STYLE_TYPE_INT},
    {"pad_hor", LV_STYLE_PAD_HOR, STYLE_TYPE_SPECIAL | STYLE_TYPE_INT},
};

#define STYLE_MAP_LEN (sizeof(g_style_map) / sizeof(g_style_map[0]))

/**
 * internal used API, called from obj:set_style
 * obj:set_property(key, value)
 * key: stack[-2]
 * value: stack[-1]
 *
 * @return 0 if succeed, -1 if failed.
 */
static int lugl_obj_set_style_kv(lua_State *L, lv_obj_t *obj, int selector)
{
  const char *key = lua_tostring(L, -2);
  if (key == NULL) {
    debug("Null key, ignored.\n");
    return -1;
  }

  /* map name to style value. */
  int i = 0;
  lv_style_value_t value = {0};
  for (; i < STYLE_MAP_LEN; i++) {
    const struct style_map_s *p = &g_style_map[i];
    if (strcmp(key, p->name))
      continue;

    style_type_t type = p->type & 0x0f;
    int v;

    switch (type) {
    case STYLE_TYPE_INT:
      v = lugl_tointeger(L, -1);
      value.num = v;
      break;
    case STYLE_TYPE_COLOR:
      if (lua_type(L, -1) == LUA_TSTRING) {
        /* support #RGB and #RRGGBB */
        const char *c = lua_tostring(L, -1);
        (void)c; /* @todo */
      } else {
        v = lugl_tointeger(L, -1);
        value.color = lv_color_hex(v); /* make to lv_color_t */
      }
      break;
    case STYLE_TYPE_POINTER:
      value.ptr = lua_touserdata(L, -1);
      break;
    case STYLE_TYPE_IMGSRC:
      value.ptr = lugl_get_img_src(L, -1);
      break;
    default:
      /* error, unkown type */
      return luaL_error(L, "unknown style");
    }

    if (p->type & STYLE_TYPE_SPECIAL) {
      switch (p->style) {
        /* style combinations */
      case LV_STYLE_SIZE:
        lv_obj_set_local_style_prop(obj, LV_STYLE_WIDTH, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_HEIGHT, value, selector);
        break;

      case LV_STYLE_PAD_ALL:
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, selector);
        break;

      case LV_STYLE_PAD_VER:
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, value, selector);
        break;

      case LV_STYLE_PAD_HOR:
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, value, selector);
        break;

      case LV_STYLE_PAD_GAP:
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_ROW, value, selector);
        lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_COLUMN, value, selector);
        break;

        /* pointers needs to build from lua stack table */
      case LV_STYLE_BG_GRAD:
        break;

      case LV_STYLE_COLOR_FILTER_DSC:
        break;

      case LV_STYLE_ANIM:
        break;

      case LV_STYLE_TRANSITION:
        break;

      default:
        break;
      }

      continue;
    } else if (p->style < _LV_STYLE_LAST_BUILT_IN_PROP) {
      lv_obj_set_local_style_prop(obj, p->style, value, selector);
    } else {
      return luaL_error(L, "unknown style");
    }
  }

  /* OK */
  return 0;
}

/**
 * obj:set_style(0, {x = 0, y = 0, bg_opa = 123})
 *
 */
static int lugl_obj_set_style(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been deleted.");
    return 0;
  }

  if (!lua_istable(L, 2)) {
    luaL_argerror(L, 2, "expect a table on 2nd para.");
    return 0;
  }

  int selector = 0;
  if (!lua_isnoneornil(L, 3)) {
    selector = lua_tointeger(L, 3);
    lua_pop(L, 1); /* later we use stack[-1] to get table. */
  }

  if (!lua_istable(L, 2)) {
    luaL_argerror(L, 2, "expect a table on 2nd para.");
    return 0;
  }

  lua_pushnil(L); /* nil as initial key to iterate through table */
  while (lua_next(L, -2)) {
    /* -1: value, -2: key */
    if (!lua_isstring(L, -2)) {
      /* we expect string as key, ignore it if not */
      debug("ignore non-string key in table.\n");
      lua_pop(L, 1);
      continue;
    }
    lugl_obj_set_style_kv(L, obj, selector);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }

  return 0;
}
