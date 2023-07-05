#include "luavgl.h"
#include "private.h"

typedef struct {
  lv_style_t style;
} luavgl_style_t;

typedef enum {
  STYLE_TYPE_INT = 1,
  STYLE_TYPE_COLOR,
  STYLE_TYPE_POINTER, /* a pointer from light-userdata */
  STYLE_TYPE_IMGSRC,  /* string or light-userdata, similar to pointer */

  STYLE_TYPE_TABLE, /* value is in table */
  /* Flag of none-simple type. All extended styles are special case. */
  STYLE_TYPE_SPECIAL = 0x10,
} style_type_t;

enum {
  LV_STYLE_SIZE = _LV_STYLE_LAST_BUILT_IN_PROP + 1,
  LV_STYLE_PAD_ALL,
  LV_STYLE_PAD_VER,
  LV_STYLE_PAD_HOR,
  LV_STYLE_PAD_GAP,

  _LV_STYLE_FLEX,

  _LV_STYLE_FLEX_FLOW, /* style not const, need to call related API */
  _LV_STYLE_FLEX_MAIN_PLACE,
  _LV_STYLE_FLEX_CROSS_PLACE,
  _LV_STYLE_FLEX_TRACK_PLACE,
  _LV_STYLE_FLEX_GROW,
};

/* callback made when style matched. */
typedef void (*style_set_cb_t)(lv_style_prop_t prop, lv_style_value_t value,
                               void *args);

static const struct style_map_s {
  const char *name;
  lv_style_prop_t prop;
  style_type_t type;
} g_style_map[] = {
    {"width",              LV_STYLE_WIDTH,              STYLE_TYPE_INT                       },
    {"min_width",          LV_STYLE_MIN_WIDTH,          STYLE_TYPE_INT                       },
    {"max_width",          LV_STYLE_MAX_WIDTH,          STYLE_TYPE_INT                       },
    {"height",             LV_STYLE_HEIGHT,             STYLE_TYPE_INT                       },
    {"min_height",         LV_STYLE_MIN_HEIGHT,         STYLE_TYPE_INT                       },
    {"max_height",         LV_STYLE_MAX_HEIGHT,         STYLE_TYPE_INT                       },
    {"x",                  LV_STYLE_X,                  STYLE_TYPE_INT                       },
    {"y",                  LV_STYLE_Y,                  STYLE_TYPE_INT                       },
    {"align",              LV_STYLE_ALIGN,              STYLE_TYPE_INT                       },
    {"transform_width",    LV_STYLE_TRANSFORM_WIDTH,    STYLE_TYPE_INT                       },
    {"transform_height",   LV_STYLE_TRANSFORM_HEIGHT,   STYLE_TYPE_INT                       },
    {"translate_x",        LV_STYLE_TRANSLATE_X,        STYLE_TYPE_INT                       },
    {"translate_y",        LV_STYLE_TRANSLATE_Y,        STYLE_TYPE_INT                       },
    {"transform_zoom",     LV_STYLE_TRANSFORM_ZOOM,     STYLE_TYPE_INT                       },
    {"transform_angle",    LV_STYLE_TRANSFORM_ANGLE,    STYLE_TYPE_INT                       },
    {"transform_pivot_x",  LV_STYLE_TRANSFORM_PIVOT_X,  STYLE_TYPE_INT                       },
    {"transform_pivot_y",  LV_STYLE_TRANSFORM_PIVOT_Y,  STYLE_TYPE_INT                       },
    {"pad_top",            LV_STYLE_PAD_TOP,            STYLE_TYPE_INT                       },
    {"pad_bottom",         LV_STYLE_PAD_BOTTOM,         STYLE_TYPE_INT                       },
    {"pad_left",           LV_STYLE_PAD_LEFT,           STYLE_TYPE_INT                       },
    {"pad_right",          LV_STYLE_PAD_RIGHT,          STYLE_TYPE_INT                       },
    {"pad_row",            LV_STYLE_PAD_ROW,            STYLE_TYPE_INT                       },
    {"pad_column",         LV_STYLE_PAD_COLUMN,         STYLE_TYPE_INT                       },
    {"pad_gap",            LV_STYLE_PAD_GAP,            STYLE_TYPE_INT                       },
    {"bg_color",           LV_STYLE_BG_COLOR,           STYLE_TYPE_COLOR                     },
    {"bg_opa",             LV_STYLE_BG_OPA,             STYLE_TYPE_INT                       },
    {"bg_grad_color",      LV_STYLE_BG_GRAD_COLOR,      STYLE_TYPE_COLOR                     },
    {"bg_grad_dir",        LV_STYLE_BG_GRAD_DIR,        STYLE_TYPE_INT                       },
    {"bg_main_stop",       LV_STYLE_BG_MAIN_STOP,       STYLE_TYPE_INT                       },
    {"bg_grad_stop",       LV_STYLE_BG_GRAD_STOP,       STYLE_TYPE_INT                       },
    {"bg_dither_mode",     LV_STYLE_BG_DITHER_MODE,     STYLE_TYPE_INT                       },
    {"bg_img_src",         LV_STYLE_BG_IMG_SRC,         STYLE_TYPE_IMGSRC                    },
    {"bg_img_opa",         LV_STYLE_BG_IMG_OPA,         STYLE_TYPE_INT                       },
    {"bg_img_recolor",     LV_STYLE_BG_IMG_RECOLOR,     STYLE_TYPE_COLOR                     },
    {"bg_img_recolor_opa", LV_STYLE_BG_IMG_RECOLOR_OPA, STYLE_TYPE_INT                       },
    {"bg_img_tiled",       LV_STYLE_BG_IMG_TILED,       STYLE_TYPE_INT                       },
    {"border_color",       LV_STYLE_BORDER_COLOR,       STYLE_TYPE_COLOR                     },
    {"border_opa",         LV_STYLE_BORDER_OPA,         STYLE_TYPE_INT                       },
    {"border_width",       LV_STYLE_BORDER_WIDTH,       STYLE_TYPE_INT                       },
    {"border_side",        LV_STYLE_BORDER_SIDE,        STYLE_TYPE_INT                       },
    {"border_post",        LV_STYLE_BORDER_POST,        STYLE_TYPE_INT                       },
    {"outline_width",      LV_STYLE_OUTLINE_WIDTH,      STYLE_TYPE_INT                       },
    {"outline_color",      LV_STYLE_OUTLINE_COLOR,      STYLE_TYPE_COLOR                     },
    {"outline_opa",        LV_STYLE_OUTLINE_OPA,        STYLE_TYPE_INT                       },
    {"outline_pad",        LV_STYLE_OUTLINE_PAD,        STYLE_TYPE_INT                       },
    {"shadow_width",       LV_STYLE_SHADOW_WIDTH,       STYLE_TYPE_INT                       },
    {"shadow_ofs_x",       LV_STYLE_SHADOW_OFS_X,       STYLE_TYPE_INT                       },
    {"shadow_ofs_y",       LV_STYLE_SHADOW_OFS_Y,       STYLE_TYPE_INT                       },
    {"shadow_spread",      LV_STYLE_SHADOW_SPREAD,      STYLE_TYPE_INT                       },
    {"shadow_color",       LV_STYLE_SHADOW_COLOR,       STYLE_TYPE_COLOR                     },
    {"shadow_opa",         LV_STYLE_SHADOW_OPA,         STYLE_TYPE_INT                       },
    {"img_opa",            LV_STYLE_IMG_OPA,            STYLE_TYPE_INT                       },
    {"img_recolor",        LV_STYLE_IMG_RECOLOR,        STYLE_TYPE_COLOR                     },
    {"img_recolor_opa",    LV_STYLE_IMG_RECOLOR_OPA,    STYLE_TYPE_INT                       },
    {"line_width",         LV_STYLE_LINE_WIDTH,         STYLE_TYPE_INT                       },
    {"line_dash_width",    LV_STYLE_LINE_DASH_WIDTH,    STYLE_TYPE_INT                       },
    {"line_dash_gap",      LV_STYLE_LINE_DASH_GAP,      STYLE_TYPE_INT                       },
    {"line_rounded",       LV_STYLE_LINE_ROUNDED,       STYLE_TYPE_INT                       },
    {"line_color",         LV_STYLE_LINE_COLOR,         STYLE_TYPE_INT                       },
    {"line_opa",           LV_STYLE_LINE_OPA,           STYLE_TYPE_INT                       },
    {"arc_width",          LV_STYLE_ARC_WIDTH,          STYLE_TYPE_INT                       },
    {"arc_img_src",        LV_STYLE_ARC_IMG_SRC,        STYLE_TYPE_IMGSRC                    },
    {"arc_rounded",        LV_STYLE_ARC_ROUNDED,        STYLE_TYPE_INT                       },
    {"arc_color",          LV_STYLE_ARC_COLOR,          STYLE_TYPE_COLOR                     },
    {"arc_opa",            LV_STYLE_ARC_OPA,            STYLE_TYPE_INT                       },
    {"text_color",         LV_STYLE_TEXT_COLOR,         STYLE_TYPE_COLOR                     },
    {"text_opa",           LV_STYLE_TEXT_OPA,           STYLE_TYPE_INT                       },
    {"text_font",          LV_STYLE_TEXT_FONT,          STYLE_TYPE_POINTER                   }, /* light-userdata */
    {"text_letter_space",  LV_STYLE_TEXT_LETTER_SPACE,  STYLE_TYPE_INT                       },
    {"text_line_space",    LV_STYLE_TEXT_LINE_SPACE,    STYLE_TYPE_INT                       },
    {"text_decor",         LV_STYLE_TEXT_DECOR,         STYLE_TYPE_INT                       },
    {"text_align",         LV_STYLE_TEXT_ALIGN,         STYLE_TYPE_INT                       },
    {"radius",             LV_STYLE_RADIUS,             STYLE_TYPE_INT                       },
    {"clip_corner",        LV_STYLE_CLIP_CORNER,        STYLE_TYPE_INT                       },
    {"opa",                LV_STYLE_OPA,                STYLE_TYPE_INT                       },
    {"color_filter_opa",   LV_STYLE_COLOR_FILTER_OPA,   STYLE_TYPE_INT                       },
    {"anim_time",          LV_STYLE_ANIM_TIME,          STYLE_TYPE_INT                       },
    {"anim_speed",         LV_STYLE_ANIM_SPEED,         STYLE_TYPE_INT                       },
    {"blend_mode",         LV_STYLE_BLEND_MODE,         STYLE_TYPE_INT                       },
    {"layout",             LV_STYLE_LAYOUT,             STYLE_TYPE_INT                       },
    {"base_dir",           LV_STYLE_BASE_DIR,           STYLE_TYPE_INT                       },

 /* need to build pointer from table parameter */
    {"bg_grad",            LV_STYLE_BG_GRAD,            STYLE_TYPE_SPECIAL                   }, /* pointer from table */
    {"color_filter_dsc",   LV_STYLE_COLOR_FILTER_DSC,   STYLE_TYPE_SPECIAL                   }, /**/
    {"anim",               LV_STYLE_ANIM,               STYLE_TYPE_SPECIAL                   }, /* anim para */
    {"transition",         LV_STYLE_TRANSITION,         STYLE_TYPE_SPECIAL                   }, /* transition */

  /* styles combined */
    {"size",               LV_STYLE_SIZE,               STYLE_TYPE_SPECIAL | STYLE_TYPE_INT  },
    {"pad_all",            LV_STYLE_PAD_ALL,            STYLE_TYPE_SPECIAL | STYLE_TYPE_INT  },
    {"pad_ver",            LV_STYLE_PAD_VER,            STYLE_TYPE_SPECIAL | STYLE_TYPE_INT  },
    {"pad_hor",            LV_STYLE_PAD_HOR,            STYLE_TYPE_SPECIAL | STYLE_TYPE_INT  },

 /* styles for layout */
    {"flex",               _LV_STYLE_FLEX,              STYLE_TYPE_SPECIAL | STYLE_TYPE_TABLE},
    {"flex_flow",          _LV_STYLE_FLEX_FLOW,         STYLE_TYPE_SPECIAL | STYLE_TYPE_INT  },
    {"flex_main_place",    _LV_STYLE_FLEX_MAIN_PLACE,
     STYLE_TYPE_SPECIAL | STYLE_TYPE_INT                                                     },
    {"flex_cross_place",   _LV_STYLE_FLEX_CROSS_PLACE,
     STYLE_TYPE_SPECIAL | STYLE_TYPE_INT                                                     },
    {"flex_track_place",   _LV_STYLE_FLEX_TRACK_PLACE,
     STYLE_TYPE_SPECIAL | STYLE_TYPE_INT                                                     },
    {"flex_grow",          _LV_STYLE_FLEX_GROW,         STYLE_TYPE_SPECIAL | STYLE_TYPE_INT  },
};

#define STYLE_MAP_LEN (sizeof(g_style_map) / sizeof(g_style_map[0]))

/**
 * lv_style
 */

static luavgl_style_t *luavgl_check_style(lua_State *L, int index)
{
  luavgl_style_t *v = *(luavgl_style_t **)luaL_checkudata(L, index, "lv_style");
  return v;
}

static void lv_style_set_cb(lv_style_prop_t prop, lv_style_value_t value,
                            void *args)
{
  lv_style_t *s = args;
  lv_style_set_prop(s, prop, value);
}

static void lv_style_set_inherit_cb(lv_style_prop_t prop,
                                    lv_style_value_t value, void *args)
{
  lv_style_t *s = args;
  lv_style_set_prop_meta(s, prop, LV_STYLE_PROP_META_INHERIT);
}

static uint8_t to_int(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  return -1;
}

static lv_flex_align_t luavgl_to_flex_align(lua_State *L, int idx)
{
  if (lua_type(L, idx) != LUA_TSTRING)
    return LV_FLEX_ALIGN_START;

  const char *str = lua_tostring(L, idx);
  if (strcmp("flex-start", str) == 0)
    return LV_FLEX_ALIGN_START;

  if (strcmp("flex-end", str) == 0)
    return LV_FLEX_ALIGN_END;

  if (strcmp("center", str) == 0)
    return LV_FLEX_ALIGN_CENTER;

  if (strcmp("space-evenly", str) == 0)
    return LV_FLEX_ALIGN_SPACE_EVENLY;

  if (strcmp("space-around", str) == 0)
    return LV_FLEX_ALIGN_SPACE_AROUND;

  if (strcmp("space-between", str) == 0)
    return LV_FLEX_ALIGN_SPACE_BETWEEN;

  return LV_FLEX_ALIGN_START;
}

static int luavgl_set_flex_layout_kv(lua_State *L, style_set_cb_t cb,
                                     void *args)
{
  if (!lua_istable(L, -1)) {
    debug("para should be table.");
    return luaL_argerror(L, -1, "should be table.");
  }

  const char *str;
  lv_flex_flow_t flow = LV_FLEX_FLOW_ROW;
  lv_flex_align_t align;

  /**
   * flex-direction:
   * row | row-reverse | column | column-reverse
   */
  lua_getfield(L, -1, "flex_direction");
  if (lua_type(L, -1) == LUA_TSTRING) {
    str = lua_tostring(L, -1);
    /* starts with  */
    if (strncmp("row", str, 3) == 0) {
      flow = LV_FLEX_FLOW_ROW;
    } else if (strncmp("column", str, 3) == 0) {
      flow = LV_FLEX_FLOW_COLUMN;
    }

    /* if reverse presents */
    if (strstr(str, "-reverse")) {
      flow |= _LV_FLEX_REVERSE;
    }
  }
  lua_pop(L, 1);

  /**
   * flex-wrap:
   * nowrap | wrap | wrap-reverse;
   */
  lua_getfield(L, -1, "flex_wrap");
  if (lua_type(L, -1) == LUA_TSTRING) {
    str = lua_tostring(L, -1);
    if (strcmp("wrap", str) == 0) {
      flow |= _LV_FLEX_WRAP;
    } else if (strcmp("wrap-reverse", str) == 0) {
      flow |= _LV_FLEX_WRAP | _LV_FLEX_REVERSE;
    }
    /* else: normal */
  }
  lua_pop(L, 1);

  lv_style_value_t value = {0};
  value.num = LV_LAYOUT_FLEX;
  cb(LV_STYLE_LAYOUT, value, args);
  value.num = flow;
  cb(LV_STYLE_FLEX_FLOW, value, args);

  /**
   * justify-content
   * flex-start | flex-end | center |
   * space-between | space-around | space-evenly
   */
  lua_getfield(L, -1, "justify_content");
  if (lua_type(L, -1) == LUA_TSTRING) {
    align = luavgl_to_flex_align(L, -1);
    value.num = align;
    cb(LV_STYLE_FLEX_MAIN_PLACE, value, args);
  }
  lua_pop(L, 1);

  /**
   * align-items
   * flex-start | flex-end | center |
   * space-between | space-around | space-evenly
   */
  lua_getfield(L, -1, "align_items");
  if (lua_type(L, -1) == LUA_TSTRING) {
    align = luavgl_to_flex_align(L, -1);
    value.num = align;
    cb(LV_STYLE_FLEX_CROSS_PLACE, value, args);
  }
  lua_pop(L, 1);

  /**
   * align-content
   * flex-start | flex-end | center |
   * space-between | space-around | space-evenly
   */
  lua_getfield(L, -1, "align_content");
  if (lua_type(L, -1) == LUA_TSTRING) {
    align = luavgl_to_flex_align(L, -1);
    value.num = align;
    cb(LV_STYLE_FLEX_TRACK_PLACE, value, args);
  }
  lua_pop(L, 1);

  return 0;
}

/* is the style value on stack top is inherit special value */
static inline bool luavgl_is_style_inherit(lua_State *L)
{
  const char *str;
  return (lua_type(L, -1) == LUA_TSTRING) && (str = lua_tostring(L, -1)) &&
         (strcmp(str, "inherit") == 0);
}

/**
 * internal used API, called from style:set()
 * key: stack[-2]
 * value: stack[-1]
 *
 * @return 0 if succeed, -1 if failed.
 */
static int luavgl_set_style_kv(lua_State *L, style_set_cb_t cb, void *args)
{
  const char *key = lua_tostring(L, -2);
  if (key == NULL) {
    debug("Null key, ignored.\n");
    return -1;
  }

  /* map name to style value. */
  lv_style_value_t value = {0};
  const struct style_map_s *p = NULL;
  for (int i = 0; i < STYLE_MAP_LEN; i++) {
    if (strcmp(key, g_style_map[i].name) == 0) {
      p = &g_style_map[i];
      break;
    }
  }

  if (p == NULL) /* not found */
    return -1;

  style_type_t type = p->type & 0x0f;
  int v;

  if (!luavgl_is_style_inherit(L)) {
    /* get normal values */
    switch (type) {
    case STYLE_TYPE_INT:
      v = luavgl_tointeger(L, -1);
      value.num = v;
      break;
    case STYLE_TYPE_COLOR:
      value.color = luavgl_tocolor(L, -1);
      break;
    case STYLE_TYPE_POINTER:
      value.ptr = lua_touserdata(L, -1);
      break;
    case STYLE_TYPE_IMGSRC:
      value.ptr = luavgl_toimgsrc(L, -1);
      break;
    case STYLE_TYPE_TABLE:
      break;
    default:
      /* error, unkown type */
      return luaL_error(L, "unknown style");
    }
  }

  if (p->type & STYLE_TYPE_SPECIAL) {
    switch ((int)p->prop) {
      /* style combinations */
    case LV_STYLE_SIZE:
      cb(LV_STYLE_WIDTH, value, args);
      cb(LV_STYLE_HEIGHT, value, args);
      break;

    case LV_STYLE_PAD_ALL:
      cb(LV_STYLE_PAD_TOP, value, args);
      cb(LV_STYLE_PAD_BOTTOM, value, args);
      cb(LV_STYLE_PAD_LEFT, value, args);
      cb(LV_STYLE_PAD_RIGHT, value, args);
      break;

    case LV_STYLE_PAD_VER:
      cb(LV_STYLE_PAD_TOP, value, args);
      cb(LV_STYLE_PAD_BOTTOM, value, args);
      break;

    case LV_STYLE_PAD_HOR:
      cb(LV_STYLE_PAD_LEFT, value, args);
      cb(LV_STYLE_PAD_RIGHT, value, args);
      break;

    case LV_STYLE_PAD_GAP:
      cb(LV_STYLE_PAD_ROW, value, args);
      cb(LV_STYLE_PAD_COLUMN, value, args);
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

    /* layout styles that not constant */
    case _LV_STYLE_FLEX_FLOW:
      cb(LV_STYLE_FLEX_FLOW, value, args);
      break;
    case _LV_STYLE_FLEX_MAIN_PLACE:
      cb(LV_STYLE_FLEX_MAIN_PLACE, value, args);
      break;
    case _LV_STYLE_FLEX_CROSS_PLACE:
      cb(LV_STYLE_FLEX_CROSS_PLACE, value, args);
      break;
    case _LV_STYLE_FLEX_TRACK_PLACE:
      cb(LV_STYLE_FLEX_TRACK_PLACE, value, args);
      break;
    case _LV_STYLE_FLEX_GROW:
      cb(LV_STYLE_FLEX_GROW, value, args);
      break;

    case _LV_STYLE_FLEX: {
      /* value is all on table */
      luavgl_set_flex_layout_kv(L, cb, args);
      break;
    }
    default:
      break;
    }
  } else if (p->prop <= _LV_STYLE_LAST_BUILT_IN_PROP) {
    cb(p->prop, value, args);
  } else {
    return luaL_error(L, "unknown style");
  }

  return 0;
}

/**
 * style:set({x = 0, y = 0, bg_opa = 123})
 */
static int luavgl_style_set(lua_State *L)
{
  luavgl_style_t *s = luavgl_check_style(L, 1);

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

    /* special value check */
    bool inherit = luavgl_is_style_inherit(L);

    luavgl_set_style_kv(L, inherit ? lv_style_set_inherit_cb : lv_style_set_cb,
                        s);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }

  return 0;
}

/**
 * luavgl.Style({
 *  bg_color = 0xff0000,
 *  border_width = 1,
 * })
 *
 * For simplicity, style need to be manually deleted `style:delete()` in order
 * to be gc'ed.
 */
static int luavgl_style_create(lua_State *L)
{
  luavgl_style_t *s = malloc(sizeof(luavgl_style_t));
  if (s == NULL) {
    return luaL_error(L, "No memory.");
  }

  lv_style_init(&s->style);

  *(void **)lua_newuserdata(L, sizeof(void *)) = s;
  luaL_getmetatable(L, "lv_style");
  lua_setmetatable(L, -2);

  lua_pushlightuserdata(L, s);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  lua_rotate(L, 1, 1); /* stack after: style-user-data, para */

  luavgl_style_set(L);

  lua_pop(L, 1); /* remove parameter table */
  return 1;
}

/**
 * style:remove_prop("width")
 */
static int luavgl_style_remove_prop(lua_State *L)
{
  luavgl_style_t *s = luavgl_check_style(L, 1);
  const char *name = lua_tostring(L, 2);

  for (int i = 0; i < STYLE_MAP_LEN; i++) {
    const struct style_map_s *p = &g_style_map[i];
    if (strcmp(name, p->name) == 0) {
      lv_style_remove_prop(&s->style, p->prop);
      return 0;
    }
  }

  return luaL_error(L, "unknown prop name: %s", name);
}

static int luavgl_style_delete(lua_State *L)
{
  luavgl_style_t *s = luavgl_check_style(L, 1);

  lua_pushlightuserdata(L, s);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  return 1;
}

static int luavgl_style_gc(lua_State *L)
{
  luavgl_style_t *s = luavgl_check_style(L, 1);
  lv_style_reset(&s->style);
  free(s);
  debug("gc style:%p\n", s);
  return 0;
}

/**
 * lv_obj_style
 */

struct obj_style_s {
  lv_obj_t *obj;
  int selector;
};

static void obj_style_set_cb(lv_style_prop_t prop, lv_style_value_t value,
                             void *args)
{
  struct obj_style_s *info = args;
  lv_obj_set_local_style_prop(info->obj, prop, value, info->selector);
}

static void obj_style_inherit_set_cb(lv_style_prop_t prop,
                                     lv_style_value_t value, void *args)
{
  struct obj_style_s *info = args;
  lv_obj_set_local_style_prop_meta(info->obj, prop, LV_STYLE_PROP_META_INHERIT,
                                   info->selector);
}

static int luavgl_obj_set_style_kv(lua_State *L, lv_obj_t *obj, int selector)
{
  struct obj_style_s info = {
      .obj = obj,
      .selector = selector,
  };

  /* special value check */
  bool inherit = luavgl_is_style_inherit(L);

  return luavgl_set_style_kv(
      L, inherit ? obj_style_inherit_set_cb : obj_style_set_cb, &info);
}

/**
 * obj:set_style({x = 0, y = 0, bg_opa = 123}, 0)
 */
static int luavgl_obj_set_style(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
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

  lua_pushnil(L); /* nil as initial key to iterate through table */
  while (lua_next(L, -2)) {
    /* -1: value, -2: key */
    if (!lua_isstring(L, -2)) {
      /* we expect string as key, ignore it if not */
      debug("ignore non-string key in table.\n");
      lua_pop(L, 1);
      continue;
    }

    luavgl_obj_set_style_kv(L, obj, selector);
    lua_pop(L, 1); /* remove value, keep the key to continue. */
  }

  return 0;
}

/**
 * obj:add_style(style, 0)
 */
static int luavgl_obj_add_style(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  luavgl_style_t *s = luavgl_check_style(L, 2);

  int selector = 0;
  if (!lua_isnoneornil(L, 3)) {
    selector = lua_tointeger(L, 3);
  }

  lv_obj_add_style(obj, &s->style, selector);

  return 0;
}

/**
 * obj:remove_style(style, 0)
 */
static int luavgl_obj_remove_style(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  luavgl_style_t *s = luavgl_check_style(L, 2);

  int selector = 0;
  if (!lua_isnoneornil(L, 3)) {
    selector = lua_tointeger(L, 3);
  }

  lv_obj_remove_style(obj, &s->style, selector);
  return 0;
}

/**
 * obj:remove_style_all()
 */
static int luavgl_obj_remove_style_all(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  lv_obj_remove_style_all(obj);
  return 0;
}

static const luaL_Reg luavgl_style_methods[] = {
    {"set",         luavgl_style_set        },
    {"delete",      luavgl_style_delete     },
    {"remove_prop", luavgl_style_remove_prop},

    {NULL,          NULL                    }
};

static void luavgl_style_init(lua_State *L)
{
  luaL_newmetatable(L, "lv_style");

  lua_pushcfunction(L, luavgl_style_gc);
  lua_setfield(L, -2, "__gc");

  luaL_newlib(L, luavgl_style_methods); /* methods belong to this type */
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
