
#include <lvgl.h>

#include "rotable.h"

#define rotable_setfiled(L, idx, field, reg)                                   \
  do {                                                                         \
    rotable_newlib(L, reg);                                                    \
    lua_setfield(L, idx, field);                                               \
  } while (0)

static const rotable_Reg event_const_table[] = {
    {.name = "ALL",                 .integer = LV_EVENT_ALL                },
    {.name = "PRESSED",             .integer = LV_EVENT_PRESSED            },
    {.name = "PRESSING",            .integer = LV_EVENT_PRESSING           },
    {.name = "PRESS_LOST",          .integer = LV_EVENT_PRESS_LOST         },
    {.name = "SHORT_CLICKED",       .integer = LV_EVENT_SHORT_CLICKED      },
    {.name = "LONG_PRESSED",        .integer = LV_EVENT_LONG_PRESSED       },
    {.name = "LONG_PRESSED_REPEAT", .integer = LV_EVENT_LONG_PRESSED_REPEAT},
    {.name = "CLICKED",             .integer = LV_EVENT_CLICKED            },
    {.name = "RELEASED",            .integer = LV_EVENT_RELEASED           },
    {.name = "SCROLL_BEGIN",        .integer = LV_EVENT_SCROLL_BEGIN       },
    {.name = "SCROLL_END",          .integer = LV_EVENT_SCROLL_END         },
    {.name = "SCROLL",              .integer = LV_EVENT_SCROLL             },
    {.name = "GESTURE",             .integer = LV_EVENT_GESTURE            },
    {.name = "KEY",                 .integer = LV_EVENT_KEY                },
    {.name = "FOCUSED",             .integer = LV_EVENT_FOCUSED            },
    {.name = "DEFOCUSED",           .integer = LV_EVENT_DEFOCUSED          },
    {.name = "LEAVE",               .integer = LV_EVENT_LEAVE              },
    {.name = "HIT_TEST",            .integer = LV_EVENT_HIT_TEST           },
    {.name = "COVER_CHECK",         .integer = LV_EVENT_COVER_CHECK        },
    {.name = "REFR_EXT_DRAW_SIZE",  .integer = LV_EVENT_REFR_EXT_DRAW_SIZE },
    {.name = "DRAW_MAIN_BEGIN",     .integer = LV_EVENT_DRAW_MAIN_BEGIN    },
    {.name = "DRAW_MAIN",           .integer = LV_EVENT_DRAW_MAIN          },
    {.name = "DRAW_MAIN_END",       .integer = LV_EVENT_DRAW_MAIN_END      },
    {.name = "DRAW_POST_BEGIN",     .integer = LV_EVENT_DRAW_POST_BEGIN    },
    {.name = "DRAW_POST",           .integer = LV_EVENT_DRAW_POST          },
    {.name = "DRAW_POST_END",       .integer = LV_EVENT_DRAW_POST_END      },
    {.name = "VALUE_CHANGED",       .integer = LV_EVENT_VALUE_CHANGED      },
    {.name = "INSERT",              .integer = LV_EVENT_INSERT             },
    {.name = "REFRESH",             .integer = LV_EVENT_REFRESH            },
    {.name = "READY",               .integer = LV_EVENT_READY              },
    {.name = "CANCEL",              .integer = LV_EVENT_CANCEL             },
    {.name = "DELETE",              .integer = LV_EVENT_DELETE             },
    {.name = "CHILD_CHANGED",       .integer = LV_EVENT_CHILD_CHANGED      },
    {.name = "CHILD_CREATED",       .integer = LV_EVENT_CHILD_CREATED      },
    {.name = "CHILD_DELETED",       .integer = LV_EVENT_CHILD_DELETED      },
    {.name = "SCREEN_UNLOAD_START", .integer = LV_EVENT_SCREEN_UNLOAD_START},
    {.name = "SCREEN_LOAD_START",   .integer = LV_EVENT_SCREEN_LOAD_START  },
    {.name = "SCREEN_LOADED",       .integer = LV_EVENT_SCREEN_LOADED      },
    {.name = "SCREEN_UNLOADED",     .integer = LV_EVENT_SCREEN_UNLOADED    },
    {.name = "SIZE_CHANGED",        .integer = LV_EVENT_SIZE_CHANGED       },
    {.name = "STYLE_CHANGED",       .integer = LV_EVENT_STYLE_CHANGED      },
    {.name = "LAYOUT_CHANGED",      .integer = LV_EVENT_LAYOUT_CHANGED     },
    {.name = "GET_SELF_SIZE",       .integer = LV_EVENT_GET_SELF_SIZE      },
    {0,                             0                                      },
};

static const rotable_Reg obj_flag_const_table[] = {
    {.name = "PRESSED",           .integer = LV_EVENT_PRESSED              },
    {.name = "HIDDEN",            .integer = LV_OBJ_FLAG_HIDDEN            },
    {.name = "CLICKABLE",         .integer = LV_OBJ_FLAG_CLICKABLE         },
    {.name = "CLICK_FOCUSABLE",   .integer = LV_OBJ_FLAG_CLICK_FOCUSABLE   },
    {.name = "CHECKABLE",         .integer = LV_OBJ_FLAG_CHECKABLE         },
    {.name = "SCROLLABLE",        .integer = LV_OBJ_FLAG_SCROLLABLE        },
    {.name = "SCROLL_ELASTIC",    .integer = LV_OBJ_FLAG_SCROLL_ELASTIC    },
    {.name = "SCROLL_MOMENTUM",   .integer = LV_OBJ_FLAG_SCROLL_MOMENTUM   },
    {.name = "SCROLL_ONE",        .integer = LV_OBJ_FLAG_SCROLL_ONE        },
    {.name = "SCROLL_CHAIN_HOR",  .integer = LV_OBJ_FLAG_SCROLL_CHAIN_HOR  },
    {.name = "SCROLL_CHAIN_VER",  .integer = LV_OBJ_FLAG_SCROLL_CHAIN_VER  },
    {.name = "SCROLL_CHAIN",
     .integer = LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER},
    {.name = "SCROLL_ON_FOCUS",   .integer = LV_OBJ_FLAG_SCROLL_ON_FOCUS   },
    {.name = "SCROLL_WITH_ARROW", .integer = LV_OBJ_FLAG_SCROLL_WITH_ARROW },
    {.name = "SNAPPABLE",         .integer = LV_OBJ_FLAG_SNAPPABLE         },
    {.name = "PRESS_LOCK",        .integer = LV_OBJ_FLAG_PRESS_LOCK        },
    {.name = "EVENT_BUBBLE",      .integer = LV_OBJ_FLAG_EVENT_BUBBLE      },
    {.name = "GESTURE_BUBBLE",    .integer = LV_OBJ_FLAG_GESTURE_BUBBLE    },
    {.name = "ADV_HITTEST",       .integer = LV_OBJ_FLAG_ADV_HITTEST       },
    {.name = "IGNORE_LAYOUT",     .integer = LV_OBJ_FLAG_IGNORE_LAYOUT     },
    {.name = "FLOATING",          .integer = LV_OBJ_FLAG_FLOATING          },
    {.name = "OVERFLOW_VISIBLE",  .integer = LV_OBJ_FLAG_OVERFLOW_VISIBLE  },
    {.name = "LAYOUT_1",          .integer = LV_OBJ_FLAG_LAYOUT_1          },
    {.name = "LAYOUT_2",          .integer = LV_OBJ_FLAG_LAYOUT_2          },
    {.name = "WIDGET_1",          .integer = LV_OBJ_FLAG_WIDGET_1          },
    {.name = "WIDGET_2",          .integer = LV_OBJ_FLAG_WIDGET_2          },
    {.name = "USER_1",            .integer = LV_OBJ_FLAG_USER_1            },
    {.name = "USER_2",            .integer = LV_OBJ_FLAG_USER_2            },
    {.name = "USER_3",            .integer = LV_OBJ_FLAG_USER_3            },
    {.name = "USER_4",            .integer = LV_OBJ_FLAG_USER_4            },
    {0,                           0                                        },
};

static const rotable_Reg state_const_table[] = {
    {.name = "DEFAULT",   .integer = LV_STATE_DEFAULT  },
    {.name = "CHECKED",   .integer = LV_STATE_CHECKED  },
    {.name = "FOCUSED",   .integer = LV_STATE_FOCUSED  },
    {.name = "FOCUS_KEY", .integer = LV_STATE_FOCUS_KEY},
    {.name = "EDITED",    .integer = LV_STATE_EDITED   },
    {.name = "HOVERED",   .integer = LV_STATE_HOVERED  },
    {.name = "PRESSED",   .integer = LV_STATE_PRESSED  },
    {.name = "SCROLLED",  .integer = LV_STATE_SCROLLED },
    {.name = "DISABLED",  .integer = LV_STATE_DISABLED },
    {.name = "USER_1",    .integer = LV_STATE_USER_1   },
    {.name = "USER_2",    .integer = LV_STATE_USER_2   },
    {.name = "USER_3",    .integer = LV_STATE_USER_3   },
    {.name = "USER_4",    .integer = LV_STATE_USER_4   },
    {.name = "ANY",       .integer = LV_STATE_ANY      },
    {0,                   0                            }
};

static const rotable_Reg part_const_table[] = {
    {.name = "MAIN",         .integer = LV_PART_MAIN        },
    {.name = "SCROLLBAR",    .integer = LV_PART_SCROLLBAR   },
    {.name = "INDICATOR",    .integer = LV_PART_INDICATOR   },
    {.name = "KNOB",         .integer = LV_PART_KNOB        },
    {.name = "SELECTED",     .integer = LV_PART_SELECTED    },
    {.name = "ITEMS",        .integer = LV_PART_ITEMS       },
    {.name = "CURSOR",       .integer = LV_PART_CURSOR      },
    {.name = "CUSTOM_FIRST", .integer = LV_PART_CUSTOM_FIRST},
    {.name = "ANY",          .integer = LV_PART_ANY         },
    {0,                      0                              },
};

static const rotable_Reg align_const_table[] = {

    {.name = "DEFAULT",          .integer = LV_ALIGN_DEFAULT         },
    {.name = "TOP_LEFT",         .integer = LV_ALIGN_TOP_LEFT        },
    {.name = "TOP_MID",          .integer = LV_ALIGN_TOP_MID         },
    {.name = "TOP_RIGHT",        .integer = LV_ALIGN_TOP_RIGHT       },
    {.name = "BOTTOM_LEFT",      .integer = LV_ALIGN_BOTTOM_LEFT     },
    {.name = "BOTTOM_MID",       .integer = LV_ALIGN_BOTTOM_MID      },
    {.name = "BOTTOM_RIGHT",     .integer = LV_ALIGN_BOTTOM_RIGHT    },
    {.name = "LEFT_MID",         .integer = LV_ALIGN_LEFT_MID        },
    {.name = "RIGHT_MID",        .integer = LV_ALIGN_RIGHT_MID       },
    {.name = "CENTER",           .integer = LV_ALIGN_CENTER          },
    {.name = "OUT_TOP_LEFT",     .integer = LV_ALIGN_OUT_TOP_LEFT    },
    {.name = "OUT_TOP_MID",      .integer = LV_ALIGN_OUT_TOP_MID     },
    {.name = "OUT_TOP_RIGHT",    .integer = LV_ALIGN_OUT_TOP_RIGHT   },
    {.name = "OUT_BOTTOM_LEFT",  .integer = LV_ALIGN_OUT_BOTTOM_LEFT },
    {.name = "OUT_BOTTOM_MID",   .integer = LV_ALIGN_OUT_BOTTOM_MID  },
    {.name = "OUT_BOTTOM_RIGHT", .integer = LV_ALIGN_OUT_BOTTOM_RIGHT},
    {.name = "OUT_LEFT_TOP",     .integer = LV_ALIGN_OUT_LEFT_TOP    },
    {.name = "OUT_LEFT_MID",     .integer = LV_ALIGN_OUT_LEFT_MID    },
    {.name = "OUT_LEFT_BOTTOM",  .integer = LV_ALIGN_OUT_LEFT_BOTTOM },
    {.name = "OUT_RIGHT_TOP",    .integer = LV_ALIGN_OUT_RIGHT_TOP   },
    {.name = "OUT_RIGHT_MID",    .integer = LV_ALIGN_OUT_RIGHT_MID   },
    {.name = "OUT_RIGHT_BOTTOM", .integer = LV_ALIGN_OUT_RIGHT_BOTTOM},
    {0,                          0                                   },
};

static const rotable_Reg label_const_const_table[] = {

    {.name = "LONG_WRAP",            .integer = LV_LABEL_LONG_WRAP           },
    {.name = "LONG_DOT",             .integer = LV_LABEL_LONG_DOT            },
    {.name = "LONG_SCROLL",          .integer = LV_LABEL_LONG_SCROLL         },
    {.name = "LONG_SCROLL_CIRCULAR", .integer = LV_LABEL_LONG_SCROLL_CIRCULAR},
    {.name = "LONG_CLIP",            .integer = LV_LABEL_LONG_CLIP           },
    {0,                              0                                       },
};

#if LV_FONT_MONTSERRAT_8
LV_FONT_DECLARE(lv_font_montserrat_8);
#endif

#if LV_FONT_MONTSERRAT_10
LV_FONT_DECLARE(lv_font_montserrat_10);
#endif

#if LV_FONT_MONTSERRAT_12
LV_FONT_DECLARE(lv_font_montserrat_12);
#endif

#if LV_FONT_MONTSERRAT_14
LV_FONT_DECLARE(lv_font_montserrat_14);
#endif

#if LV_FONT_MONTSERRAT_16
LV_FONT_DECLARE(lv_font_montserrat_16);
#endif

#if LV_FONT_MONTSERRAT_18
LV_FONT_DECLARE(lv_font_montserrat_18);
#endif

#if LV_FONT_MONTSERRAT_20
LV_FONT_DECLARE(lv_font_montserrat_20);
#endif

#if LV_FONT_MONTSERRAT_22
LV_FONT_DECLARE(lv_font_montserrat_22);
#endif

#if LV_FONT_MONTSERRAT_24
LV_FONT_DECLARE(lv_font_montserrat_24);
#endif

#if LV_FONT_MONTSERRAT_26
LV_FONT_DECLARE(lv_font_montserrat_26);
#endif

#if LV_FONT_MONTSERRAT_28
LV_FONT_DECLARE(lv_font_montserrat_28);
#endif

#if LV_FONT_MONTSERRAT_30
LV_FONT_DECLARE(lv_font_montserrat_30);
#endif

#if LV_FONT_MONTSERRAT_32
LV_FONT_DECLARE(lv_font_montserrat_32);
#endif

#if LV_FONT_MONTSERRAT_34
LV_FONT_DECLARE(lv_font_montserrat_34);
#endif

#if LV_FONT_MONTSERRAT_36
LV_FONT_DECLARE(lv_font_montserrat_36);
#endif

#if LV_FONT_MONTSERRAT_38
LV_FONT_DECLARE(lv_font_montserrat_38);
#endif

#if LV_FONT_MONTSERRAT_40
LV_FONT_DECLARE(lv_font_montserrat_40);
#endif

#if LV_FONT_MONTSERRAT_42
LV_FONT_DECLARE(lv_font_montserrat_42);
#endif

#if LV_FONT_MONTSERRAT_44
LV_FONT_DECLARE(lv_font_montserrat_44);
#endif

#if LV_FONT_MONTSERRAT_46
LV_FONT_DECLARE(lv_font_montserrat_46);
#endif

#if LV_FONT_MONTSERRAT_48
LV_FONT_DECLARE(lv_font_montserrat_48);
#endif

static const rotable_Reg builtin_font_const_table[] = {
    {
     .name = "DEFAULT",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = LV_FONT_DEFAULT,
     },
#if LV_FONT_MONTSERRAT_8
    {
     .name = "MONTSERRAT_8",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_8,
     },
#endif

#if LV_FONT_MONTSERRAT_10
    {
     .name = "MONTSERRAT_10",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_10,
     },
#endif

#if LV_FONT_MONTSERRAT_12
    {
     .name = "MONTSERRAT_12",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_12,
     },
#endif

#if LV_FONT_MONTSERRAT_14
    {
     .name = "MONTSERRAT_14",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_14,
     },
#endif

#if LV_FONT_MONTSERRAT_16
    {
     .name = "MONTSERRAT_16",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_16,
     },
#endif

#if LV_FONT_MONTSERRAT_18
    {
     .name = "MONTSERRAT_18",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_18,
     },
#endif

#if LV_FONT_MONTSERRAT_20
    {
     .name = "MONTSERRAT_20",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_20,
     },
#endif

#if LV_FONT_MONTSERRAT_22
    {
     .name = "MONTSERRAT_22",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_22,
     },
#endif

#if LV_FONT_MONTSERRAT_24
    {
     .name = "MONTSERRAT_24",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_24,
     },
#endif

#if LV_FONT_MONTSERRAT_26
    {
     .name = "MONTSERRAT_26",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_26,
     },
#endif

#if LV_FONT_MONTSERRAT_28
    {
     .name = "MONTSERRAT_28",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_28,
     },
#endif

#if LV_FONT_MONTSERRAT_30
    {
     .name = "MONTSERRAT_30",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_30,
     },
#endif

#if LV_FONT_MONTSERRAT_32
    {
     .name = "MONTSERRAT_32",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_32,
     },
#endif

#if LV_FONT_MONTSERRAT_34
    {
     .name = "MONTSERRAT_34",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_34,
     },
#endif

#if LV_FONT_MONTSERRAT_36
    {
     .name = "MONTSERRAT_36",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_36,
     },
#endif

#if LV_FONT_MONTSERRAT_38
    {
     .name = "MONTSERRAT_38",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_38,
     },
#endif

#if LV_FONT_MONTSERRAT_40
    {
     .name = "MONTSERRAT_40",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_40,
     },
#endif

#if LV_FONT_MONTSERRAT_42
    {
     .name = "MONTSERRAT_42",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_42,
     },
#endif

#if LV_FONT_MONTSERRAT_44
    {
     .name = "MONTSERRAT_44",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_44,
     },
#endif

#if LV_FONT_MONTSERRAT_46
    {
     .name = "MONTSERRAT_46",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_46,
     },
#endif

#if LV_FONT_MONTSERRAT_48
    {
     .name = "MONTSERRAT_48",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_48,
     },
#endif

#if LV_FONT_MONTSERRAT_28_COMPRESSED
    {
     .name = "MONTSERRAT_28_COMPRESSED",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_montserrat_28_compressed,
     },
#endif

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    {
     .name = "DEJAVU_16_PERSIAN_HEBREW",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_dejavu_16_persian_hebrew,
     },
#endif

#if LV_FONT_SIMSUN_16_CJK
    {
     .name = "SIMSUN_16_CJK",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_simsun_16_cjk,
     },
#endif

#if LV_FONT_UNSCII_8
    {
     .name = "UNSCII_8",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_unscii_8,
     },
#endif

#if LV_FONT_UNSCII_16
    {
     .name = "UNSCII_16",
     .type = LUA_TLIGHTUSERDATA,
     .ptr = &lv_font_unscii_16,
     },
#endif
    {
     .name = 0,
     },
};

static const rotable_Reg scr_load_anim_const_table[] = {
    {.name = "NONE",        .integer = LV_SCR_LOAD_ANIM_NONE       },
    {.name = "OVER_LEFT",   .integer = LV_SCR_LOAD_ANIM_OVER_LEFT  },
    {.name = "OVER_RIGHT",  .integer = LV_SCR_LOAD_ANIM_OVER_RIGHT },
    {.name = "OVER_TOP",    .integer = LV_SCR_LOAD_ANIM_OVER_TOP   },
    {.name = "OVER_BOTTOM", .integer = LV_SCR_LOAD_ANIM_OVER_BOTTOM},
    {.name = "MOVE_LEFT",   .integer = LV_SCR_LOAD_ANIM_MOVE_LEFT  },
    {.name = "MOVE_RIGHT",  .integer = LV_SCR_LOAD_ANIM_MOVE_RIGHT },
    {.name = "MOVE_TOP",    .integer = LV_SCR_LOAD_ANIM_MOVE_TOP   },
    {.name = "MOVE_BOTTOM", .integer = LV_SCR_LOAD_ANIM_MOVE_BOTTOM},
    {.name = "FADE_ON",     .integer = LV_SCR_LOAD_ANIM_FADE_ON    },
#if LV_VERSION_CHECK(8, 3, 0)
    {.name = "FADE_IN",     .integer = LV_SCR_LOAD_ANIM_FADE_IN    },
    {.name = "FADE_OUT",    .integer = LV_SCR_LOAD_ANIM_FADE_OUT   },
    {.name = "OUT_LEFT",    .integer = LV_SCR_LOAD_ANIM_OUT_LEFT   },
    {.name = "OUT_RIGHT",   .integer = LV_SCR_LOAD_ANIM_OUT_RIGHT  },
    {.name = "OUT_TOP",     .integer = LV_SCR_LOAD_ANIM_OUT_TOP    },
    {.name = "OUT_BOTTOM",  .integer = LV_SCR_LOAD_ANIM_OUT_BOTTOM },
#endif
    {0,                     0                                      },
};

static const rotable_Reg scrollbar_mode_const_table[] = {
    {.name = "OFF",    .integer = LV_SCROLLBAR_MODE_OFF   },
    {.name = "ON",     .integer = LV_SCROLLBAR_MODE_ON    },
    {.name = "ACTIVE", .integer = LV_SCROLLBAR_MODE_ACTIVE},
    {.name = "AUTO",   .integer = LV_SCROLLBAR_MODE_AUTO  },
    {0,                0                                  },
};

static const rotable_Reg dir_const_table[] = {
    {.name = "NONE",   .integer = LV_DIR_NONE  },
    {.name = "LEFT",   .integer = LV_DIR_LEFT  },
    {.name = "RIGHT",  .integer = LV_DIR_RIGHT },
    {.name = "TOP",    .integer = LV_DIR_TOP   },
    {.name = "BOTTOM", .integer = LV_DIR_BOTTOM},
    {.name = "HOR",    .integer = LV_DIR_HOR   },
    {.name = "VER",    .integer = LV_DIR_VER   },
    {.name = "ALL",    .integer = LV_DIR_ALL   },
    {0,                0                       },
};

#if LV_USE_KEYBOARD
static const rotable_Reg keyboard_mode_const_table[] = {
    {.name = "TEXT_LOWER",  .integer = LV_KEYBOARD_MODE_TEXT_LOWER },
    {.name = "TEXT_UPPER",  .integer = LV_KEYBOARD_MODE_TEXT_UPPER },
    {.name = "SPECIAL",     .integer = LV_KEYBOARD_MODE_SPECIAL    },
    {.name = "NUMBER",      .integer = LV_KEYBOARD_MODE_NUMBER     },
    {.name = "USER_1",      .integer = LV_KEYBOARD_MODE_USER_1     },
    {.name = "USER_2",      .integer = LV_KEYBOARD_MODE_USER_2     },
    {.name = "USER_3",      .integer = LV_KEYBOARD_MODE_USER_3     },
    {.name = "USER_4",      .integer = LV_KEYBOARD_MODE_USER_4     },

#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    {.name = "TEXT_ARABIC", .integer = LV_KEYBOARD_MODE_TEXT_ARABIC},
#endif
    {0,                     0                                      },
};
#endif

static const rotable_Reg flex_flow_const_table[] = {
    {.name = "ROW",                 .integer = LV_FLEX_FLOW_ROW             },
    {.name = "COLUMN",              .integer = LV_FLEX_FLOW_COLUMN          },
    {.name = "ROW_WRAP",            .integer = LV_FLEX_FLOW_ROW_WRAP        },
    {.name = "ROW_REVERSE",         .integer = LV_FLEX_FLOW_ROW_REVERSE     },
    {.name = "ROW_WRAP_REVERSE",    .integer = LV_FLEX_FLOW_ROW_WRAP_REVERSE},
    {.name = "COLUMN_WRAP",         .integer = LV_FLEX_FLOW_COLUMN_WRAP     },
    {.name = "COLUMN_REVERSE",      .integer = LV_FLEX_FLOW_COLUMN_REVERSE  },
    {.name = "COLUMN_WRAP_REVERSE",
     .integer = LV_FLEX_FLOW_COLUMN_WRAP_REVERSE                            },
    {0,                             0                                       },
};

static const rotable_Reg flex_align_const_table[] = {
    {.name = "START",         .integer = LV_FLEX_ALIGN_START        },
    {.name = "END",           .integer = LV_FLEX_ALIGN_END          },
    {.name = "CENTER",        .integer = LV_FLEX_ALIGN_CENTER       },
    {.name = "SPACE_EVENLY",  .integer = LV_FLEX_ALIGN_SPACE_EVENLY },
    {.name = "SPACE_AROUND",  .integer = LV_FLEX_ALIGN_SPACE_AROUND },
    {.name = "SPACE_BETWEEN", .integer = LV_FLEX_ALIGN_SPACE_BETWEEN},
    {0,                       0                                     },
};

static const rotable_Reg grid_align_const_table[] = {
    {.name = "START",         .integer = LV_GRID_ALIGN_START        },
    {.name = "CENTER",        .integer = LV_GRID_ALIGN_CENTER       },
    {.name = "END",           .integer = LV_GRID_ALIGN_END          },
    {.name = "STRETCH",       .integer = LV_GRID_ALIGN_STRETCH      },
    {.name = "SPACE_EVENLY",  .integer = LV_GRID_ALIGN_SPACE_EVENLY },
    {.name = "SPACE_AROUND",  .integer = LV_GRID_ALIGN_SPACE_AROUND },
    {.name = "SPACE_BETWEEN", .integer = LV_GRID_ALIGN_SPACE_BETWEEN},
    {0,                       0                                     },
};

#if LV_USE_ROLLER
static const rotable_Reg roller_mode_const_table[] = {
    {.name = "NORMAL",   .integer = LV_ROLLER_MODE_NORMAL  },
    {.name = "INFINITE", .integer = LV_ROLLER_MODE_INFINITE},
    {0,                  0                                 },
};
#endif

static const rotable_Reg key_const_table[] = {
    {.name = "UP",        .integer = LV_KEY_UP       },
    {.name = "DOWN",      .integer = LV_KEY_DOWN     },
    {.name = "RIGHT",     .integer = LV_KEY_RIGHT    },
    {.name = "LEFT",      .integer = LV_KEY_LEFT     },
    {.name = "ESC",       .integer = LV_KEY_ESC      },
    {.name = "DEL",       .integer = LV_KEY_DEL      },
    {.name = "BACKSPACE", .integer = LV_KEY_BACKSPACE},
    {.name = "ENTER",     .integer = LV_KEY_ENTER    },
    {.name = "NEXT",      .integer = LV_KEY_NEXT     },
    {.name = "PREV",      .integer = LV_KEY_PREV     },
    {.name = "HOME",      .integer = LV_KEY_HOME     },
    {.name = "END",       .integer = LV_KEY_END      },
    {0,                   0                          },
};

static int luavgl_LV_PCT(lua_State *L)
{
  int pct = lua_tointeger(L, 1);
  lua_pushinteger(L, LV_PCT(pct));
  return 1;
}

static int luavgl_LV_OPA(lua_State *L)
{
  int opa = luavgl_tointeger(L, 1) * LV_OPA_100 / 100;
  if (opa > 255)
    opa = 255;
  if (opa < 0)
    opa = 0;

  lua_pushinteger(L, opa);
  return 1;
}

static int luavgl_LV_HOR_RES(lua_State *L)
{
  lua_pushinteger(L, LV_HOR_RES);
  return 1;
}

static int luavgl_LV_VER_RES(lua_State *L)
{
  lua_pushinteger(L, LV_VER_RES);
  return 1;
}

/* clang-format on */

static void luavgl_constants_init(lua_State *L)
{
  rotable_setfiled(L, -2, "EVENT", event_const_table);
  rotable_setfiled(L, -2, "FLAG", obj_flag_const_table);
  rotable_setfiled(L, -2, "STATE", state_const_table);
  rotable_setfiled(L, -2, "PART", part_const_table);

  rotable_setfiled(L, -2, "ALIGN", align_const_table);
  rotable_setfiled(L, -2, "BUILTIN_FONT", builtin_font_const_table);
  rotable_setfiled(L, -2, "LABEL_CONST", label_const_const_table);
  rotable_setfiled(L, -2, "SCR_LOAD_ANIM", scr_load_anim_const_table);
  rotable_setfiled(L, -2, "SCROLLBAR_MODE", scrollbar_mode_const_table);
  rotable_setfiled(L, -2, "DIR", dir_const_table);

#if LV_USE_KEYBOARD
  rotable_setfiled(L, -2, "KEYBOARD_MODE", keyboard_mode_const_table);
#endif

  rotable_setfiled(L, -2, "FLEX_FLOW", flex_flow_const_table);
  rotable_setfiled(L, -2, "FLEX_ALIGN", flex_align_const_table);
  rotable_setfiled(L, -2, "GRID_ALIGN", grid_align_const_table);
#if LV_USE_ROLLER
  rotable_setfiled(L, -2, "ROLLER_MODE", roller_mode_const_table);
#endif
  rotable_setfiled(L, -2, "KEY", key_const_table);
  /* miscellaneous. */

  lua_pushinteger(L, LV_ANIM_REPEAT_INFINITE);
  lua_setfield(L, -2, "ANIM_REPEAT_INFINITE");
  lua_pushinteger(L, LV_ANIM_PLAYTIME_INFINITE);
  lua_setfield(L, -2, "ANIM_PLAYTIME_INFINITE");

  lua_pushcfunction(L, luavgl_LV_OPA);
  lua_setfield(L, -2, "OPA");

  lua_pushcfunction(L, luavgl_LV_PCT);
  lua_setfield(L, -2, "PCT");

  lua_pushinteger(L, LV_SIZE_CONTENT);
  lua_setfield(L, -2, "SIZE_CONTENT");

  lua_pushinteger(L, LV_RADIUS_CIRCLE);
  lua_setfield(L, -2, "RADIUS_CIRCLE");

  lua_pushinteger(L, LV_COORD_MAX);
  lua_setfield(L, -2, "COORD_MAX");
  lua_pushinteger(L, LV_COORD_MIN);
  lua_setfield(L, -2, "COORD_MIN");

  lua_pushinteger(L, LV_ZOOM_NONE);
  lua_setfield(L, -2, "LV_ZOOM_NONE");

  lua_pushinteger(L, LV_BTNMATRIX_BTN_NONE);
  lua_setfield(L, -2, "BTNMATRIX_BTN_NONE");

  lua_pushinteger(L, LV_CHART_POINT_NONE);
  lua_setfield(L, -2, "CHART_POINT_NONE");

#if LV_USE_DROPDOWN
  lua_pushinteger(L, LV_DROPDOWN_POS_LAST);
  lua_setfield(L, -2, "DROPDOWN_POS_LAST");
#endif

  lua_pushinteger(L, LV_LABEL_DOT_NUM);
  lua_setfield(L, -2, "LABEL_DOT_NUM");
  lua_pushinteger(L, LV_LABEL_POS_LAST);
  lua_setfield(L, -2, "LABEL_POS_LAST");
  lua_pushinteger(L, LV_LABEL_TEXT_SELECTION_OFF);
  lua_setfield(L, -2, "LABEL_TEXT_SELECTION_OFF");

  lua_pushinteger(L, LV_TABLE_CELL_NONE);
  lua_setfield(L, -2, "TABLE_CELL_NONE");

#if LV_USE_TEXTAREA
  lua_pushinteger(L, LV_TEXTAREA_CURSOR_LAST);
  lua_setfield(L, -2, "TEXTAREA_CURSOR_LAST");
#endif

  lua_pushinteger(L, LV_LAYOUT_FLEX);
  lua_setfield(L, -2, "LAYOUT_FLEX");

  lua_pushinteger(L, LV_LAYOUT_GRID);
  lua_setfield(L, -2, "LAYOUT_GRID");

  lua_pushcfunction(L, luavgl_LV_HOR_RES);
  lua_setfield(L, -2, "HOR_RES");

  lua_pushcfunction(L, luavgl_LV_VER_RES);
  lua_setfield(L, -2, "VER_RES");
}
