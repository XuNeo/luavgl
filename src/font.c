#include "luavgl.h"
#include "private.h"

#define FONT_DEFAULT_SIZE 12

#define _ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

/**
 * Follow css style, specify the name by name family, name size,
 * name weight. Font weight can be numeric value or 'bold'. Alls strings
 * are converted to lower-case before matching with local supported name.
 *
 * weight   named weight
 * 100      thin
 * 200      extra light
 * 300      light
 * 400      normal (*default)
 * 500      medium
 * 600      semi bold
 * 700      bold
 * 800      extra bold
 * 900      ultra bold
 *
 * Only normal weight is supported to builtin name.
 *
 * Font returned to lua is a userdata, it's used by object style, or style
 * directly. If name is no longer ref'ed by any object or style, it's
 * gc'ed.
 */

/**
 * lvgl.Font("montserrat, unscii", 8, bold)
 * lvgl.Font("montserrat", 8)
 * lvgl.Font("montserrat")
 *
 * return nil if failed
 */

typedef enum {
  FONT_WEIGHT_THIN = 100,
  FONT_WEIGHT_EXTRA_LIGHT = 200,
  FONT_WEIGHT_LIGHT = 300,
  FONT_WEIGHT_NORMAL = 400,
  FONT_WEIGHT_MEDIUM = 500,
  FONT_WEIGHT_SEMI_BOLD = 600,
  FONT_WEIGHT_BOLD = 700,
  FONT_WEIGHT_EXTRA_BOLD = 800,
  FONT_WEIGHT_ULTRA_BOLD = 900,
} font_weight_t;

static const struct {
  char *name;
  int value;
} g_named_weight[] = {
    {"thin",        100},
    {"extra light", 200},
    {"light",       300},
    {"normal",      400},
    {"medium",      500},
    {"semi bold",   600},
    {"bold",        700},
    {"extra bold",  800},
    {"ultra bold",  900},
};

static const struct {
  int size;
  const lv_font_t *font;
} g_builtin_montserrat[] = {
#if LV_FONT_MONTSERRAT_8
    {8,  &lv_font_montserrat_8 },
#endif

#if LV_FONT_MONTSERRAT_10
    {10, &lv_font_montserrat_10},
#endif

#if LV_FONT_MONTSERRAT_12
    {12, &lv_font_montserrat_12},
#endif

#if LV_FONT_MONTSERRAT_14
    {14, &lv_font_montserrat_14},
#endif

#if LV_FONT_MONTSERRAT_16
    {16, &lv_font_montserrat_16},
#endif

#if LV_FONT_MONTSERRAT_18
    {18, &lv_font_montserrat_18},
#endif

#if LV_FONT_MONTSERRAT_20
    {20, &lv_font_montserrat_20},
#endif

#if LV_FONT_MONTSERRAT_22
    {22, &lv_font_montserrat_22},
#endif

#if LV_FONT_MONTSERRAT_24
    {24, &lv_font_montserrat_24},
#endif

#if LV_FONT_MONTSERRAT_26
    {26, &lv_font_montserrat_26},
#endif

#if LV_FONT_MONTSERRAT_28
    {28, &lv_font_montserrat_28},
#endif

#if LV_FONT_MONTSERRAT_30
    {30, &lv_font_montserrat_30},
#endif

#if LV_FONT_MONTSERRAT_32
    {32, &lv_font_montserrat_32},
#endif

#if LV_FONT_MONTSERRAT_34
    {34, &lv_font_montserrat_34},
#endif

#if LV_FONT_MONTSERRAT_36
    {36, &lv_font_montserrat_36},
#endif

#if LV_FONT_MONTSERRAT_38
    {38, &lv_font_montserrat_38},
#endif

#if LV_FONT_MONTSERRAT_40
    {40, &lv_font_montserrat_40},
#endif

#if LV_FONT_MONTSERRAT_42
    {42, &lv_font_montserrat_42},
#endif

#if LV_FONT_MONTSERRAT_44
    {44, &lv_font_montserrat_44},
#endif

#if LV_FONT_MONTSERRAT_46
    {46, &lv_font_montserrat_46},
#endif

#if LV_FONT_MONTSERRAT_48
    {48, &lv_font_montserrat_48},
#endif
};

static int luavgl_get_named_weight(const char *name)
{
  if (name == NULL) {
    return FONT_DEFAULT_SIZE;
  }

  for (int i = 0; i < _ARRAY_LEN(g_named_weight); i++) {
    if (strcmp(name, g_named_weight[i].name) == 0) {
      return g_named_weight[i].value;
    }
  }

  return FONT_DEFAULT_SIZE;
}

static char *to_lower(char *str)
{
  for (char *s = str; *s; ++s)
    *s = *s >= 'A' && *s <= 'Z' ? *s | 0x60 : *s;
  return str;
}

static const lv_font_t *_luavgl_font_create(lua_State *L, const char *name,
                                            int size, int weight)
{
  /* check builtin font firstly. */
  if (strcmp(name, "montserrat") == 0) {
    if (FONT_WEIGHT_NORMAL != weight)
      return NULL;

    for (int i = 0; i < _ARRAY_LEN(g_builtin_montserrat); i++) {
      if (size == g_builtin_montserrat[i].size) {
        return g_builtin_montserrat[i].font;
      }
    }
  } else if (strcmp(name, "unscii") == 0) {
    if (FONT_WEIGHT_NORMAL != weight)
      return NULL;
#if LV_FONT_UNSCII_8
    if (size == 8)
      return &lv_font_unscii_8;
#endif

#if LV_FONT_UNSCII_16
    if (size == 16)
      return &lv_font_unscii_16;
#endif
  }
#if LV_FONT_MONTSERRAT_12_SUBPX
  else if (strcmp(name, "montserrat_subpx") == 0) {
    if (size == 12)
      return &lv_font_montserrat_12_subpx;
  }
#endif
#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
  else if (strcmp(name, "dejavu_persian_hebrew") == 0) {
    if (size == 16)
      return &lv_font_dejavu_16_persian_hebrew;
  }
#endif

#if LV_FONT_SIMSUN_16_CJK
  else if (strcmp(name, "dejavu_persian_hebrew") == 0) {
    if (size == 16)
      return &lv_font_simsun_16_cjk;
  }
#endif

  /* not built-in font, check extension  */
  luavgl_ctx_t *ctx = luavgl_context(L);
  if (ctx->make_font) {
    return ctx->make_font(name, size, weight);
  }

  return NULL;
}

/**
 * Dynamic font family fallback is not supported.
 * The fallback only happen when font creation fails and continue to try next
 * one. Fallback logic in lvgl is supposed to be system wide.
 *
 * lvgl.Font("MiSansW medium, montserrat", 24, "normal")
 */
static int luavgl_font_create(lua_State *L)
{
  int weight;
  int size;
  char *str, *name;
  const lv_font_t *font = NULL;

  if (!lua_isstring(L, 1)) {
    return luaL_argerror(L, 1, "expect string");
  }

  /* size is optional, default to FONT_DEFAULT_SIZE */
  size = lua_tointeger(L, 2);
  if (size == 0) {
    size = FONT_DEFAULT_SIZE;
  }

  if (!lua_isnoneornil(L, 3)) {
    if (lua_isinteger(L, 3)) {
      weight = lua_tointeger(L, 3);
    } else {
      char *luastr = (char *)lua_tostring(L, 3);
      int len = strlen(luastr);
      if (len > 128) {
        /* not likely to happen */
        return luaL_argerror(L, 3, "too long");
      }
      char s[len + 1];
      strcpy(s, luastr);
      weight = luavgl_get_named_weight(to_lower(s));
    }
  } else {
    weight = FONT_WEIGHT_NORMAL;
  }

  str = strdup(lua_tostring(L, 1));
  if (str == NULL) {
    return luaL_error(L, "no memory.");
  }

  name = to_lower(str);
  while (*name) {
    if (*name == ' ') {
      name++;
      continue;
    }

    char *end = strchr(name, ',');
    if (end != NULL) {
      *end = '\0';
    } else {
      end = name + strlen(name);
    }

    char *trim = end - 1;
    while (*trim == ' ') {
      *trim-- = '\0'; /* trailing space. */
    }

    font = _luavgl_font_create(L, name, size, weight);
    if (font) {
      break;
    }

    name = end + 1; /* next */
  }

  free(str);
  if (font) {
    lua_pushlightuserdata(L, (void *)font);
    return 1;
  }

  return luaL_error(L, "cannot create font.");
}
