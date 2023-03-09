#include "private.h"

/* clang-format off */
static void luavgl_event_code_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "ALL"); lua_pushinteger(L, LV_EVENT_ALL); lua_settable(L, -3);
  lua_pushstring(L, "PRESSED"); lua_pushinteger(L, LV_EVENT_PRESSED); lua_settable(L, -3);
  lua_pushstring(L, "PRESSING"); lua_pushinteger(L, LV_EVENT_PRESSING); lua_settable(L, -3);
  lua_pushstring(L, "PRESS_LOST"); lua_pushinteger(L, LV_EVENT_PRESS_LOST); lua_settable(L, -3);
  lua_pushstring(L, "SHORT_CLICKED"); lua_pushinteger(L, LV_EVENT_SHORT_CLICKED); lua_settable(L, -3);
  lua_pushstring(L, "LONG_PRESSED"); lua_pushinteger(L, LV_EVENT_LONG_PRESSED); lua_settable(L, -3);
  lua_pushstring(L, "LONG_PRESSED_REPEAT"); lua_pushinteger(L, LV_EVENT_LONG_PRESSED_REPEAT); lua_settable(L, -3);
  lua_pushstring(L, "CLICKED"); lua_pushinteger(L, LV_EVENT_CLICKED); lua_settable(L, -3);
  lua_pushstring(L, "RELEASED"); lua_pushinteger(L, LV_EVENT_RELEASED); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_BEGIN"); lua_pushinteger(L, LV_EVENT_SCROLL_BEGIN); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_END"); lua_pushinteger(L, LV_EVENT_SCROLL_END); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL"); lua_pushinteger(L, LV_EVENT_SCROLL); lua_settable(L, -3);
  lua_pushstring(L, "GESTURE"); lua_pushinteger(L, LV_EVENT_GESTURE); lua_settable(L, -3);
  lua_pushstring(L, "KEY"); lua_pushinteger(L, LV_EVENT_KEY); lua_settable(L, -3);
  lua_pushstring(L, "FOCUSED"); lua_pushinteger(L, LV_EVENT_FOCUSED); lua_settable(L, -3);
  lua_pushstring(L, "DEFOCUSED"); lua_pushinteger(L, LV_EVENT_DEFOCUSED); lua_settable(L, -3);
  lua_pushstring(L, "LEAVE"); lua_pushinteger(L, LV_EVENT_LEAVE); lua_settable(L, -3);
  lua_pushstring(L, "HIT_TEST"); lua_pushinteger(L, LV_EVENT_HIT_TEST); lua_settable(L, -3);
  lua_pushstring(L, "COVER_CHECK"); lua_pushinteger(L, LV_EVENT_COVER_CHECK); lua_settable(L, -3);
  lua_pushstring(L, "REFR_EXT_DRAW_SIZE"); lua_pushinteger(L, LV_EVENT_REFR_EXT_DRAW_SIZE); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_MAIN_BEGIN"); lua_pushinteger(L, LV_EVENT_DRAW_MAIN_BEGIN); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_MAIN"); lua_pushinteger(L, LV_EVENT_DRAW_MAIN); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_MAIN_END"); lua_pushinteger(L, LV_EVENT_DRAW_MAIN_END); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_POST_BEGIN"); lua_pushinteger(L, LV_EVENT_DRAW_POST_BEGIN); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_POST"); lua_pushinteger(L, LV_EVENT_DRAW_POST); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_POST_END"); lua_pushinteger(L, LV_EVENT_DRAW_POST_END); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_PART_BEGIN"); lua_pushinteger(L, LV_EVENT_DRAW_PART_BEGIN); lua_settable(L, -3);
  lua_pushstring(L, "DRAW_PART_END"); lua_pushinteger(L, LV_EVENT_DRAW_PART_END); lua_settable(L, -3);
  lua_pushstring(L, "VALUE_CHANGED"); lua_pushinteger(L, LV_EVENT_VALUE_CHANGED); lua_settable(L, -3);
  lua_pushstring(L, "INSERT"); lua_pushinteger(L, LV_EVENT_INSERT); lua_settable(L, -3);
  lua_pushstring(L, "REFRESH"); lua_pushinteger(L, LV_EVENT_REFRESH); lua_settable(L, -3);
  lua_pushstring(L, "READY"); lua_pushinteger(L, LV_EVENT_READY); lua_settable(L, -3);
  lua_pushstring(L, "CANCEL"); lua_pushinteger(L, LV_EVENT_CANCEL); lua_settable(L, -3);
  lua_pushstring(L, "DELETE"); lua_pushinteger(L, LV_EVENT_DELETE); lua_settable(L, -3);
  lua_pushstring(L, "CHILD_CHANGED"); lua_pushinteger(L, LV_EVENT_CHILD_CHANGED); lua_settable(L, -3);
  lua_pushstring(L, "CHILD_CREATED"); lua_pushinteger(L, LV_EVENT_CHILD_CREATED); lua_settable(L, -3);
  lua_pushstring(L, "CHILD_DELETED"); lua_pushinteger(L, LV_EVENT_CHILD_DELETED); lua_settable(L, -3);
  lua_pushstring(L, "SCREEN_UNLOAD_START"); lua_pushinteger(L, LV_EVENT_SCREEN_UNLOAD_START); lua_settable(L, -3);
  lua_pushstring(L, "SCREEN_LOAD_START"); lua_pushinteger(L, LV_EVENT_SCREEN_LOAD_START); lua_settable(L, -3);
  lua_pushstring(L, "SCREEN_LOADED"); lua_pushinteger(L, LV_EVENT_SCREEN_LOADED); lua_settable(L, -3);
  lua_pushstring(L, "SCREEN_UNLOADED"); lua_pushinteger(L, LV_EVENT_SCREEN_UNLOADED); lua_settable(L, -3);
  lua_pushstring(L, "SIZE_CHANGED"); lua_pushinteger(L, LV_EVENT_SIZE_CHANGED); lua_settable(L, -3);
  lua_pushstring(L, "STYLE_CHANGED"); lua_pushinteger(L, LV_EVENT_STYLE_CHANGED); lua_settable(L, -3);
  lua_pushstring(L, "LAYOUT_CHANGED"); lua_pushinteger(L, LV_EVENT_LAYOUT_CHANGED); lua_settable(L, -3);
  lua_pushstring(L, "GET_SELF_SIZE"); lua_pushinteger(L, LV_EVENT_GET_SELF_SIZE); lua_settable(L, -3);
}

static void luavgl_obj_flag_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "PRESSED"); lua_pushinteger(L, LV_EVENT_PRESSED); lua_settable(L, -3);
  lua_pushstring(L, "HIDDEN"); lua_pushinteger(L, LV_OBJ_FLAG_HIDDEN); lua_settable(L, -3);
  lua_pushstring(L, "CLICKABLE"); lua_pushinteger(L, LV_OBJ_FLAG_CLICKABLE); lua_settable(L, -3);
  lua_pushstring(L, "CLICK_FOCUSABLE"); lua_pushinteger(L, LV_OBJ_FLAG_CLICK_FOCUSABLE); lua_settable(L, -3);
  lua_pushstring(L, "CHECKABLE"); lua_pushinteger(L, LV_OBJ_FLAG_CHECKABLE); lua_settable(L, -3);
  lua_pushstring(L, "SCROLLABLE"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLLABLE); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_ELASTIC"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_ELASTIC); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_MOMENTUM"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_MOMENTUM); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_ONE"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_ONE); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_CHAIN_HOR"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_CHAIN_HOR); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_CHAIN_VER"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_CHAIN_VER); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_CHAIN"); lua_pushinteger( L, LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_ON_FOCUS"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_ON_FOCUS); lua_settable(L, -3);
  lua_pushstring(L, "SCROLL_WITH_ARROW"); lua_pushinteger(L, LV_OBJ_FLAG_SCROLL_WITH_ARROW); lua_settable(L, -3);
  lua_pushstring(L, "SNAPPABLE"); lua_pushinteger(L, LV_OBJ_FLAG_SNAPPABLE); lua_settable(L, -3);
  lua_pushstring(L, "PRESS_LOCK"); lua_pushinteger(L, LV_OBJ_FLAG_PRESS_LOCK); lua_settable(L, -3);
  lua_pushstring(L, "EVENT_BUBBLE"); lua_pushinteger(L, LV_OBJ_FLAG_EVENT_BUBBLE); lua_settable(L, -3);
  lua_pushstring(L, "GESTURE_BUBBLE"); lua_pushinteger(L, LV_OBJ_FLAG_GESTURE_BUBBLE); lua_settable(L, -3);
  lua_pushstring(L, "ADV_HITTEST"); lua_pushinteger(L, LV_OBJ_FLAG_ADV_HITTEST); lua_settable(L, -3);
  lua_pushstring(L, "IGNORE_LAYOUT"); lua_pushinteger(L, LV_OBJ_FLAG_IGNORE_LAYOUT); lua_settable(L, -3);
  lua_pushstring(L, "FLOATING"); lua_pushinteger(L, LV_OBJ_FLAG_FLOATING); lua_settable(L, -3);
  lua_pushstring(L, "OVERFLOW_VISIBLE"); lua_pushinteger(L, LV_OBJ_FLAG_OVERFLOW_VISIBLE); lua_settable(L, -3);
  lua_pushstring(L, "LAYOUT_1"); lua_pushinteger(L, LV_OBJ_FLAG_LAYOUT_1); lua_settable(L, -3);
  lua_pushstring(L, "LAYOUT_2"); lua_pushinteger(L, LV_OBJ_FLAG_LAYOUT_2); lua_settable(L, -3);
  lua_pushstring(L, "WIDGET_1"); lua_pushinteger(L, LV_OBJ_FLAG_WIDGET_1); lua_settable(L, -3);
  lua_pushstring(L, "WIDGET_2"); lua_pushinteger(L, LV_OBJ_FLAG_WIDGET_2); lua_settable(L, -3);
  lua_pushstring(L, "USER_1"); lua_pushinteger(L, LV_OBJ_FLAG_USER_1); lua_settable(L, -3);
  lua_pushstring(L, "USER_2"); lua_pushinteger(L, LV_OBJ_FLAG_USER_2); lua_settable(L, -3);
  lua_pushstring(L, "USER_3"); lua_pushinteger(L, LV_OBJ_FLAG_USER_3); lua_settable(L, -3);
  lua_pushstring(L, "USER_4"); lua_pushinteger(L, LV_OBJ_FLAG_USER_4); lua_settable(L, -3);
}

static void luavgl_state_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "DEFAULT"); lua_pushinteger(L, LV_STATE_DEFAULT); lua_settable(L, -3);
  lua_pushstring(L, "CHECKED"); lua_pushinteger(L, LV_STATE_CHECKED); lua_settable(L, -3);
  lua_pushstring(L, "FOCUSED"); lua_pushinteger(L, LV_STATE_FOCUSED); lua_settable(L, -3);
  lua_pushstring(L, "FOCUS_KEY"); lua_pushinteger(L, LV_STATE_FOCUS_KEY); lua_settable(L, -3);
  lua_pushstring(L, "EDITED"); lua_pushinteger(L, LV_STATE_EDITED); lua_settable(L, -3);
  lua_pushstring(L, "HOVERED"); lua_pushinteger(L, LV_STATE_HOVERED); lua_settable(L, -3);
  lua_pushstring(L, "PRESSED"); lua_pushinteger(L, LV_STATE_PRESSED); lua_settable(L, -3);
  lua_pushstring(L, "SCROLLED"); lua_pushinteger(L, LV_STATE_SCROLLED); lua_settable(L, -3);
  lua_pushstring(L, "DISABLED"); lua_pushinteger(L, LV_STATE_DISABLED); lua_settable(L, -3);
  lua_pushstring(L, "USER_1"); lua_pushinteger(L, LV_STATE_USER_1); lua_settable(L, -3);
  lua_pushstring(L, "USER_2"); lua_pushinteger(L, LV_STATE_USER_2); lua_settable(L, -3);
  lua_pushstring(L, "USER_3"); lua_pushinteger(L, LV_STATE_USER_3); lua_settable(L, -3);
  lua_pushstring(L, "USER_4"); lua_pushinteger(L, LV_STATE_USER_4); lua_settable(L, -3);
  lua_pushstring(L, "ANY"); lua_pushinteger(L, LV_STATE_ANY); lua_settable(L, -3);
}

static void luavgl_part_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "MAIN"); lua_pushinteger(L, LV_PART_MAIN); lua_settable(L, -3);
  lua_pushstring(L, "SCROLLBAR"); lua_pushinteger(L, LV_PART_SCROLLBAR); lua_settable(L, -3);
  lua_pushstring(L, "INDICATOR"); lua_pushinteger(L, LV_PART_INDICATOR); lua_settable(L, -3);
  lua_pushstring(L, "KNOB"); lua_pushinteger(L, LV_PART_KNOB); lua_settable(L, -3);
  lua_pushstring(L, "SELECTED"); lua_pushinteger(L, LV_PART_SELECTED); lua_settable(L, -3);
  lua_pushstring(L, "ITEMS"); lua_pushinteger(L, LV_PART_ITEMS); lua_settable(L, -3);
  lua_pushstring(L, "TICKS"); lua_pushinteger(L, LV_PART_TICKS); lua_settable(L, -3);
  lua_pushstring(L, "CURSOR"); lua_pushinteger(L, LV_PART_CURSOR); lua_settable(L, -3);
  lua_pushstring(L, "CUSTOM_FIRST"); lua_pushinteger(L, LV_PART_CUSTOM_FIRST); lua_settable(L, -3);
  lua_pushstring(L, "ANY"); lua_pushinteger(L, LV_PART_ANY); lua_settable(L, -3);
}

static void luavgl_align_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "DEFAULT"); lua_pushinteger(L, LV_ALIGN_DEFAULT); lua_settable(L, -3);
  lua_pushstring(L, "TOP_LEFT"); lua_pushinteger(L, LV_ALIGN_TOP_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "TOP_MID"); lua_pushinteger(L, LV_ALIGN_TOP_MID); lua_settable(L, -3);
  lua_pushstring(L, "TOP_RIGHT"); lua_pushinteger(L, LV_ALIGN_TOP_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "BOTTOM_LEFT"); lua_pushinteger(L, LV_ALIGN_BOTTOM_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "BOTTOM_MID"); lua_pushinteger(L, LV_ALIGN_BOTTOM_MID); lua_settable(L, -3);
  lua_pushstring(L, "BOTTOM_RIGHT"); lua_pushinteger(L, LV_ALIGN_BOTTOM_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "LEFT_MID"); lua_pushinteger(L, LV_ALIGN_LEFT_MID); lua_settable(L, -3);
  lua_pushstring(L, "RIGHT_MID"); lua_pushinteger(L, LV_ALIGN_RIGHT_MID); lua_settable(L, -3);
  lua_pushstring(L, "CENTER"); lua_pushinteger(L, LV_ALIGN_CENTER); lua_settable(L, -3);
  lua_pushstring(L, "OUT_TOP_LEFT"); lua_pushinteger(L, LV_ALIGN_OUT_TOP_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_TOP_MID"); lua_pushinteger(L, LV_ALIGN_OUT_TOP_MID); lua_settable(L, -3);
  lua_pushstring(L, "OUT_TOP_RIGHT"); lua_pushinteger(L, LV_ALIGN_OUT_TOP_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_BOTTOM_LEFT"); lua_pushinteger(L, LV_ALIGN_OUT_BOTTOM_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_BOTTOM_MID"); lua_pushinteger(L, LV_ALIGN_OUT_BOTTOM_MID); lua_settable(L, -3);
  lua_pushstring(L, "OUT_BOTTOM_RIGHT"); lua_pushinteger(L, LV_ALIGN_OUT_BOTTOM_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_LEFT_TOP"); lua_pushinteger(L, LV_ALIGN_OUT_LEFT_TOP); lua_settable(L, -3);
  lua_pushstring(L, "OUT_LEFT_MID"); lua_pushinteger(L, LV_ALIGN_OUT_LEFT_MID); lua_settable(L, -3);
  lua_pushstring(L, "OUT_LEFT_BOTTOM"); lua_pushinteger(L, LV_ALIGN_OUT_LEFT_BOTTOM); lua_settable(L, -3);
  lua_pushstring(L, "OUT_RIGHT_TOP"); lua_pushinteger(L, LV_ALIGN_OUT_RIGHT_TOP); lua_settable(L, -3);
  lua_pushstring(L, "OUT_RIGHT_MID"); lua_pushinteger(L, LV_ALIGN_OUT_RIGHT_MID); lua_settable(L, -3);
  lua_pushstring(L, "OUT_RIGHT_BOTTOM"); lua_pushinteger(L, LV_ALIGN_OUT_RIGHT_BOTTOM); lua_settable(L, -3);
}

static void luavgl_label_const_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "LONG_WRAP"); lua_pushinteger(L, LV_LABEL_LONG_WRAP); lua_settable(L, -3);
  lua_pushstring(L, "LONG_DOT"); lua_pushinteger(L, LV_LABEL_LONG_DOT); lua_settable(L, -3);
  lua_pushstring(L, "LONG_SCROLL"); lua_pushinteger(L, LV_LABEL_LONG_SCROLL); lua_settable(L, -3);
  lua_pushstring(L, "LONG_SCROLL_CIRCULAR"); lua_pushinteger(L, LV_LABEL_LONG_SCROLL_CIRCULAR); lua_settable(L, -3);
  lua_pushstring(L, "LONG_CLIP"); lua_pushinteger(L, LV_LABEL_LONG_CLIP); lua_settable(L, -3);

}
static void luavgl_builtin_font_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "DEFAULT"); lua_pushlightuserdata(L, (void*)LV_FONT_DEFAULT); lua_settable(L, -3);
#if LV_FONT_MONTSERRAT_8
  LV_FONT_DECLARE(lv_font_montserrat_8)
  lua_pushstring(L, "MONTSERRAT_8"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_8); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_10
  LV_FONT_DECLARE(lv_font_montserrat_10)
  lua_pushstring(L, "MONTSERRAT_10"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_10); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_12
  LV_FONT_DECLARE(lv_font_montserrat_12)
  lua_pushstring(L, "MONTSERRAT_12"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_12); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_14
  LV_FONT_DECLARE(lv_font_montserrat_14)
  lua_pushstring(L, "MONTSERRAT_14"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_14); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_16
  LV_FONT_DECLARE(lv_font_montserrat_16)
  lua_pushstring(L, "MONTSERRAT_16"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_16); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_18
  LV_FONT_DECLARE(lv_font_montserrat_18)
  lua_pushstring(L, "MONTSERRAT_18"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_18); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_20
  LV_FONT_DECLARE(lv_font_montserrat_20)
  lua_pushstring(L, "MONTSERRAT_20"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_20); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_22
  LV_FONT_DECLARE(lv_font_montserrat_22)
  lua_pushstring(L, "MONTSERRAT_22"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_22); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_24
  LV_FONT_DECLARE(lv_font_montserrat_24)
  lua_pushstring(L, "MONTSERRAT_24"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_24); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_26
  LV_FONT_DECLARE(lv_font_montserrat_26)
  lua_pushstring(L, "MONTSERRAT_26"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_26); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_28
  LV_FONT_DECLARE(lv_font_montserrat_28)
  lua_pushstring(L, "MONTSERRAT_28"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_28); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_30
  LV_FONT_DECLARE(lv_font_montserrat_30)
  lua_pushstring(L, "MONTSERRAT_30"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_30); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_32
  LV_FONT_DECLARE(lv_font_montserrat_32)
  lua_pushstring(L, "MONTSERRAT_32"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_32); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_34
  LV_FONT_DECLARE(lv_font_montserrat_34)
  lua_pushstring(L, "MONTSERRAT_34"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_34); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_36
  LV_FONT_DECLARE(lv_font_montserrat_36)
  lua_pushstring(L, "MONTSERRAT_36"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_36); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_38
  LV_FONT_DECLARE(lv_font_montserrat_38)
  lua_pushstring(L, "MONTSERRAT_38"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_38); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_40
  LV_FONT_DECLARE(lv_font_montserrat_40)
  lua_pushstring(L, "MONTSERRAT_40"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_40); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_42
  LV_FONT_DECLARE(lv_font_montserrat_42)
  lua_pushstring(L, "MONTSERRAT_42"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_42); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_44
  LV_FONT_DECLARE(lv_font_montserrat_44)
  lua_pushstring(L, "MONTSERRAT_44"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_44); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_46
  LV_FONT_DECLARE(lv_font_montserrat_46)
  lua_pushstring(L, "MONTSERRAT_46"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_46); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_48
  LV_FONT_DECLARE(lv_font_montserrat_48)
  lua_pushstring(L, "MONTSERRAT_48"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_48); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_12_SUBPX
  LV_FONT_DECLARE(lv_font_montserrat_12_subpx)
  lua_pushstring(L, "MONTSERRAT_12_SUBPX"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_12_subpx); lua_settable(L, -3);
#endif

#if LV_FONT_MONTSERRAT_28_COMPRESSED
  LV_FONT_DECLARE(lv_font_montserrat_28_compressed)
  lua_pushstring(L, "MONTSERRAT_28_COMPRESSED"); lua_pushlightuserdata(L, (void*)&lv_font_montserrat_28_compressed); lua_settable(L, -3);
#endif

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
  LV_FONT_DECLARE(lv_font_dejavu_16_persian_hebrew)
  lua_pushstring(L, "DEJAVU_16_PERSIAN_HEBREW"); lua_pushlightuserdata(L, (void*)&lv_font_dejavu_16_persian_hebrew); lua_settable(L, -3);
#endif

#if LV_FONT_SIMSUN_16_CJK
  LV_FONT_DECLARE(lv_font_simsun_16_cjk)
  lua_pushstring(L, "SIMSUN_16_CJK"); lua_pushlightuserdata(L, (void*)&lv_font_simsun_16_cjk); lua_settable(L, -3);
#endif

#if LV_FONT_UNSCII_8
  LV_FONT_DECLARE(lv_font_unscii_8)
  lua_pushstring(L, "UNSCII_8"); lua_pushlightuserdata(L, (void*)&lv_font_unscii_8); lua_settable(L, -3);
#endif

#if LV_FONT_UNSCII_16
  LV_FONT_DECLARE(lv_font_unscii_16)
  lua_pushstring(L, "UNSCII_16"); lua_pushlightuserdata(L, (void*)&lv_font_unscii_16); lua_settable(L, -3);
#endif
}

static void luavgl_scr_load_anim_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "NONE"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_NONE); lua_settable(L, -3);
  lua_pushstring(L, "OVER_LEFT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OVER_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "OVER_RIGHT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OVER_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "OVER_TOP"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OVER_TOP); lua_settable(L, -3);
  lua_pushstring(L, "OVER_BOTTOM"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OVER_BOTTOM); lua_settable(L, -3);
  lua_pushstring(L, "MOVE_LEFT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_MOVE_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "MOVE_RIGHT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_MOVE_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "MOVE_TOP"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_MOVE_TOP); lua_settable(L, -3);
  lua_pushstring(L, "MOVE_BOTTOM"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_MOVE_BOTTOM); lua_settable(L, -3);
  lua_pushstring(L, "FADE_IN"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_FADE_IN); lua_settable(L, -3);
  lua_pushstring(L, "FADE_ON"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_FADE_ON); lua_settable(L, -3);
  lua_pushstring(L, "FADE_OUT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_FADE_OUT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_LEFT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OUT_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_RIGHT"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OUT_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "OUT_TOP"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OUT_TOP); lua_settable(L, -3);
  lua_pushstring(L, "OUT_BOTTOM"); lua_pushinteger(L, LV_SCR_LOAD_ANIM_OUT_BOTTOM); lua_settable(L, -3);
}

static void luavgl_scrollbar_mode_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "OFF"); lua_pushinteger(L, LV_SCROLLBAR_MODE_OFF); lua_settable(L, -3);
  lua_pushstring(L, "ON"); lua_pushinteger(L, LV_SCROLLBAR_MODE_ON); lua_settable(L, -3);
  lua_pushstring(L, "ACTIVE"); lua_pushinteger(L, LV_SCROLLBAR_MODE_ACTIVE); lua_settable(L, -3);
  lua_pushstring(L, "AUTO"); lua_pushinteger(L, LV_SCROLLBAR_MODE_AUTO); lua_settable(L, -3);
}

static void luavgl_dir_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "NONE"); lua_pushinteger(L, LV_DIR_NONE); lua_settable(L, -3);
  lua_pushstring(L, "LEFT"); lua_pushinteger(L, LV_DIR_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "RIGHT"); lua_pushinteger(L, LV_DIR_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "TOP"); lua_pushinteger(L, LV_DIR_TOP); lua_settable(L, -3);
  lua_pushstring(L, "BOTTOM"); lua_pushinteger(L, LV_DIR_BOTTOM); lua_settable(L, -3);
  lua_pushstring(L, "HOR"); lua_pushinteger(L, LV_DIR_HOR); lua_settable(L, -3);
  lua_pushstring(L, "VER"); lua_pushinteger(L, LV_DIR_VER); lua_settable(L, -3);
  lua_pushstring(L, "ALL"); lua_pushinteger(L, LV_DIR_ALL); lua_settable(L, -3);
}

static void luavgl_keyboard_mode_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "TEXT_LOWER"); lua_pushinteger(L, LV_KEYBOARD_MODE_TEXT_LOWER); lua_settable(L, -3);
  lua_pushstring(L, "TEXT_UPPER"); lua_pushinteger(L, LV_KEYBOARD_MODE_TEXT_UPPER); lua_settable(L, -3);
  lua_pushstring(L, "SPECIAL"); lua_pushinteger(L, LV_KEYBOARD_MODE_SPECIAL); lua_settable(L, -3);
  lua_pushstring(L, "NUMBER"); lua_pushinteger(L, LV_KEYBOARD_MODE_NUMBER); lua_settable(L, -3);
  lua_pushstring(L, "USER_1"); lua_pushinteger(L, LV_KEYBOARD_MODE_USER_1); lua_settable(L, -3);
  lua_pushstring(L, "USER_2"); lua_pushinteger(L, LV_KEYBOARD_MODE_USER_2); lua_settable(L, -3);
  lua_pushstring(L, "USER_3"); lua_pushinteger(L, LV_KEYBOARD_MODE_USER_3); lua_settable(L, -3);
  lua_pushstring(L, "USER_4"); lua_pushinteger(L, LV_KEYBOARD_MODE_USER_4); lua_settable(L, -3);

#if LV_USE_ARABIC_PERSIAN_CHARS == 1
  lua_pushstring(L, "TEXT_ARABIC"); lua_pushinteger(L, LV_KEYBOARD_MODE_TEXT_ARABIC); lua_settable(L, -3);
#endif
}

static void luavgl_flex_flow_init(lua_State* L)
{
  lua_newtable(L);

  lua_pushstring(L, "ROW"); lua_pushinteger(L, LV_FLEX_FLOW_ROW); lua_settable(L, -3);
  lua_pushstring(L, "COLUMN"); lua_pushinteger(L, LV_FLEX_FLOW_COLUMN); lua_settable(L, -3);
  lua_pushstring(L, "ROW_WRAP"); lua_pushinteger(L, LV_FLEX_FLOW_ROW_WRAP); lua_settable(L, -3);
  lua_pushstring(L, "ROW_REVERSE"); lua_pushinteger(L, LV_FLEX_FLOW_ROW_REVERSE); lua_settable(L, -3);
  lua_pushstring(L, "ROW_WRAP_REVERSE"); lua_pushinteger(L, LV_FLEX_FLOW_ROW_WRAP_REVERSE); lua_settable(L, -3);
  lua_pushstring(L, "COLUMN_WRAP"); lua_pushinteger(L, LV_FLEX_FLOW_COLUMN_WRAP); lua_settable(L, -3);
  lua_pushstring(L, "COLUMN_REVERSE"); lua_pushinteger(L, LV_FLEX_FLOW_COLUMN_REVERSE); lua_settable(L, -3);
  lua_pushstring(L, "COLUMN_WRAP_REVERSE"); lua_pushinteger(L, LV_FLEX_FLOW_COLUMN_WRAP_REVERSE); lua_settable(L, -3);
}

static void luavgl_flex_align_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "START"); lua_pushinteger(L, LV_FLEX_ALIGN_START); lua_settable(L, -3);
  lua_pushstring(L, "END"); lua_pushinteger(L, LV_FLEX_ALIGN_END); lua_settable(L, -3);
  lua_pushstring(L, "CENTER"); lua_pushinteger(L, LV_FLEX_ALIGN_CENTER); lua_settable(L, -3);
  lua_pushstring(L, "SPACE_EVENLY"); lua_pushinteger(L, LV_FLEX_ALIGN_SPACE_EVENLY); lua_settable(L, -3);
  lua_pushstring(L, "SPACE_AROUND"); lua_pushinteger(L, LV_FLEX_ALIGN_SPACE_AROUND); lua_settable(L, -3);
  lua_pushstring(L, "SPACE_BETWEEN"); lua_pushinteger(L, LV_FLEX_ALIGN_SPACE_BETWEEN); lua_settable(L, -3);
}

static void luavgl_grid_align_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "START"); lua_pushinteger(L, LV_GRID_ALIGN_START); lua_settable(L, -3);
  lua_pushstring(L, "CENTER"); lua_pushinteger(L, LV_GRID_ALIGN_CENTER); lua_settable(L, -3);
  lua_pushstring(L, "END"); lua_pushinteger(L, LV_GRID_ALIGN_END); lua_settable(L, -3);
  lua_pushstring(L, "STRETCH"); lua_pushinteger(L, LV_GRID_ALIGN_STRETCH); lua_settable(L, -3);
  lua_pushstring(L, "SPACE_EVENLY"); lua_pushinteger(L, LV_GRID_ALIGN_SPACE_EVENLY); lua_settable(L, -3);
  lua_pushstring(L, "SPACE_AROUND"); lua_pushinteger(L, LV_GRID_ALIGN_SPACE_AROUND); lua_settable(L, -3);
  lua_pushstring(L, "SPACE_BETWEEN"); lua_pushinteger(L, LV_GRID_ALIGN_SPACE_BETWEEN); lua_settable(L, -3);
}

static void luavgl_roller_mode_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "NORMAL"); lua_pushinteger(L, LV_ROLLER_MODE_NORMAL); lua_settable(L, -3);
  lua_pushstring(L, "INFINITE"); lua_pushinteger(L, LV_ROLLER_MODE_INFINITE); lua_settable(L, -3);
}

static void luavgl_key_init(lua_State* L)
{
  lua_newtable(L);
  lua_pushstring(L, "UP"); lua_pushinteger(L, LV_KEY_UP); lua_settable(L, -3);
  lua_pushstring(L, "DOWN"); lua_pushinteger(L, LV_KEY_DOWN); lua_settable(L, -3);
  lua_pushstring(L, "RIGHT"); lua_pushinteger(L, LV_KEY_RIGHT); lua_settable(L, -3);
  lua_pushstring(L, "LEFT"); lua_pushinteger(L, LV_KEY_LEFT); lua_settable(L, -3);
  lua_pushstring(L, "ESC"); lua_pushinteger(L, LV_KEY_ESC); lua_settable(L, -3);
  lua_pushstring(L, "DEL"); lua_pushinteger(L, LV_KEY_DEL); lua_settable(L, -3);
  lua_pushstring(L, "BACKSPACE"); lua_pushinteger(L, LV_KEY_BACKSPACE); lua_settable(L, -3);
  lua_pushstring(L, "ENTER"); lua_pushinteger(L, LV_KEY_ENTER); lua_settable(L, -3);
  lua_pushstring(L, "NEXT"); lua_pushinteger(L, LV_KEY_NEXT); lua_settable(L, -3);
  lua_pushstring(L, "PREV"); lua_pushinteger(L, LV_KEY_PREV); lua_settable(L, -3);
  lua_pushstring(L, "HOME"); lua_pushinteger(L, LV_KEY_HOME); lua_settable(L, -3);
  lua_pushstring(L, "END"); lua_pushinteger(L, LV_KEY_END); lua_settable(L, -3);
}

static int luavgl_LV_PCT(lua_State*L)
{
  int pct = lua_tointeger(L, 1);
  lua_pushinteger(L, LV_PCT(pct));
  return 1;
}

static int luavgl_LV_OPA(lua_State*L)
{
  int opa = luavgl_tointeger(L, 1) * LV_OPA_100 / 100;
  if (opa > 255)
    opa = 255;
  if (opa < 0)
    opa = 0;

  lua_pushinteger(L, opa);
  return 1;
}

static int luavgl_LV_HOR_RES(lua_State*L)
{
  lua_pushinteger(L, LV_HOR_RES);
  return 1;
}

static int luavgl_LV_VER_RES(lua_State*L)
{
  lua_pushinteger(L, LV_VER_RES);
  return 1;
}

/* clang-format on */
static void luavgl_constants_init(lua_State *L)
{
  luavgl_event_code_init(L);
  lua_setfield(L, -2, "EVENT");
  luavgl_obj_flag_init(L);
  lua_setfield(L, -2, "FLAG");
  luavgl_state_init(L);
  lua_setfield(L, -2, "STATE");
  luavgl_part_init(L);
  lua_setfield(L, -2, "PART");
  luavgl_align_init(L);
  lua_setfield(L, -2, "ALIGN");
  luavgl_builtin_font_init(L);
  lua_setfield(L, -2, "BUILTIN_FONT");
  luavgl_label_const_init(L);
  lua_setfield(L, -2, "LABEL");
  luavgl_scr_load_anim_init(L);
  lua_setfield(L, -2, "SCR_LOAD_ANIM");
  luavgl_scrollbar_mode_init(L);
  lua_setfield(L, -2, "SCROLLBAR_MODE");
  luavgl_dir_init(L);
  lua_setfield(L, -2, "DIR");
  luavgl_keyboard_mode_init(L);
  lua_setfield(L, -2, "KEYBOARD_MODE");
  luavgl_flex_flow_init(L);
  lua_setfield(L, -2, "FLEX_FLOW");
  luavgl_flex_align_init(L);
  lua_setfield(L, -2, "FLEX_ALIGN");
  luavgl_grid_align_init(L);
  lua_setfield(L, -2, "GRID_ALIGN");
  luavgl_roller_mode_init(L);
  lua_setfield(L, -2, "ROLLER_MODE");
  luavgl_key_init(L);
  lua_setfield(L, -2, "KEY");
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

  lua_pushinteger(L, LV_IMG_ZOOM_NONE);
  lua_setfield(L, -2, "IMG_ZOOM_NONE");

  lua_pushinteger(L, LV_BTNMATRIX_BTN_NONE);
  lua_setfield(L, -2, "BTNMATRIX_BTN_NONE");

  lua_pushinteger(L, LV_CHART_POINT_NONE);
  lua_setfield(L, -2, "CHART_POINT_NONE");

  lua_pushinteger(L, LV_DROPDOWN_POS_LAST);
  lua_setfield(L, -2, "DROPDOWN_POS_LAST");

  lua_pushinteger(L, LV_LABEL_DOT_NUM);
  lua_setfield(L, -2, "LABEL_DOT_NUM");
  lua_pushinteger(L, LV_LABEL_POS_LAST);
  lua_setfield(L, -2, "LABEL_POS_LAST");
  lua_pushinteger(L, LV_LABEL_TEXT_SELECTION_OFF);
  lua_setfield(L, -2, "LABEL_TEXT_SELECTION_OFF");

  lua_pushinteger(L, LV_TABLE_CELL_NONE);
  lua_setfield(L, -2, "TABLE_CELL_NONE");

  lua_pushinteger(L, LV_TEXTAREA_CURSOR_LAST);
  lua_setfield(L, -2, "TEXTAREA_CURSOR_LAST");

  lua_pushinteger(L, LV_LAYOUT_FLEX);
  lua_setfield(L, -2, "LAYOUT_FLEX");

  lua_pushinteger(L, LV_LAYOUT_GRID);
  lua_setfield(L, -2, "LAYOUT_GRID");

  lua_pushcfunction(L, luavgl_LV_HOR_RES);
  lua_setfield(L, -2, "HOR_RES");

  lua_pushcfunction(L, luavgl_LV_VER_RES);
  lua_setfield(L, -2, "VER_RES");
}
