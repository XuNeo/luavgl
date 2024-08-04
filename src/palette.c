#include "luavgl.h"
#include "private.h"
#include "rotable.h"

static int luavgl_palette_main(lua_State *L)
{
  lv_palette_t p = luaL_checkinteger(L, 1);

  lv_color_t c = lv_palette_main(p);
  luavgl_pushcolor(L, c);
  return 1;
}

static int luavgl_palette_lighten(lua_State *L)
{
  lv_palette_t p = luaL_checkinteger(L, 1);
  uint8_t lvl = luaL_checkinteger(L, 2);

  lv_color_t c = lv_palette_lighten(p, lvl);
  luavgl_pushcolor(L, c);
  return 1;
}

static int luavgl_palette_darken(lua_State *L)
{
  lv_palette_t p = luaL_checkinteger(L, 1);
  uint8_t lvl = luaL_checkinteger(L, 2);

  lv_color_t c = lv_palette_darken(p, lvl);
  luavgl_pushcolor(L, c);
  return 1;
}

static const rotable_Reg palette_constants[] = {
    {"RED",         0,             {.integer = LV_PALETTE_RED}        },
    {"PINK",        0,             {.integer = LV_PALETTE_PINK}       },
    {"PURPLE",      0,             {.integer = LV_PALETTE_PURPLE}     },
    {"DEEP_PURPLE", 0,             {.integer = LV_PALETTE_DEEP_PURPLE}},
    {"INDIGO",      0,             {.integer = LV_PALETTE_INDIGO}     },
    {"BLUE",        0,             {.integer = LV_PALETTE_BLUE}       },
    {"LIGHT_BLUE",  0,             {.integer = LV_PALETTE_LIGHT_BLUE} },
    {"CYAN",        0,             {.integer = LV_PALETTE_CYAN}       },
    {"TEAL",        0,             {.integer = LV_PALETTE_TEAL}       },
    {"GREEN",       0,             {.integer = LV_PALETTE_GREEN}      },
    {"LIGHT_GREEN", 0,             {.integer = LV_PALETTE_LIGHT_GREEN}},
    {"LIME",        0,             {.integer = LV_PALETTE_LIME}       },
    {"YELLOW",      0,             {.integer = LV_PALETTE_YELLOW}     },
    {"AMBER",       0,             {.integer = LV_PALETTE_AMBER}      },
    {"ORANGE",      0,             {.integer = LV_PALETTE_ORANGE}     },
    {"DEEP_ORANGE", 0,             {.integer = LV_PALETTE_DEEP_ORANGE}},
    {"BROWN",       0,             {.integer = LV_PALETTE_BROWN}      },
    {"BLUE_GREY",   0,             {.integer = LV_PALETTE_BLUE_GREY}  },
    {"GREY",        0,             {.integer = LV_PALETTE_GREY}       },
    {"LAST",        0,             {.integer = LV_PALETTE_LAST}       },
    {"NONE",        0,             {.integer = LV_PALETTE_NONE}       },

    {"main",        LUA_TFUNCTION, {luavgl_palette_main}              },
    {"lighten",     LUA_TFUNCTION, {luavgl_palette_lighten}           },
    {"darken",      LUA_TFUNCTION, {luavgl_palette_darken}            },

    {0,             0,             {0}                                },
};

static void luavgl_palette_init(lua_State *L)
{
  rotable_newlib(L, palette_constants);
  lua_setfield(L, -2, "palette");
}
