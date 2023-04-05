#include "luavgl.h"
#include "private.h"

#if LV_USE_CALENDAR
#include "calendar.c"
#endif

#if LV_USE_CHECKBOX
#include "checkbox.c"
#endif

#if LV_USE_DROPDOWN
#include "dropdown.c"
#endif

#if LV_USE_IMG
#include "img.c"
#endif

#if LV_USE_KEYBOARD
#include "keyboard.c"
#endif

#if LV_USE_LABEL
#include "label.c"
#endif

#if LV_USE_LED
#include "led.c"
#endif

#if LV_USE_LIST
#include "list.c"
#endif

#if LV_USE_ROLLER
#include "roller.c"
#endif

#if LV_USE_TEXTAREA
#include "textarea.c"
#endif

static int luavgl_obj_create(lua_State *L);

static const luaL_Reg widget_create_methods[] = {
    {"Object",   luavgl_obj_create     },

#if LV_USE_CALENDAR
    {"Calendar", luavgl_calendar_create},
#endif

#if LV_USE_CHECKBOX
    {"Checkbox", luavgl_checkbox_create},
#endif

#if LV_USE_DROPDOWN
    {"Dropdown", luavgl_dropdown_create},
#endif

#if LV_USE_IMG
    {"Image",    luavgl_img_create     },
#endif

#if LV_USE_KEYBOARD
    {"Keyboard", luavgl_keyboard_create},
#endif

#if LV_USE_LABEL
    {"Label",    luavgl_label_create   },
#endif

#if LV_USE_LED
    {"Led",      luavgl_led_create     },
#endif

#if LV_USE_LIST
    {"List",     luavgl_list_create    },
#endif

#if LV_USE_ROLLER
    {"Roller",   luavgl_roller_create  },
#endif

#if LV_USE_TEXTAREA
    {"Textarea", luavgl_textarea_create},
#endif
    {NULL,       NULL                  }
};

static void luavgl_widgets_init(lua_State *L)
{
#if LV_USE_IMG
  luavgl_img_init(L);
#endif

#if LV_USE_LABEL
  luavgl_label_init(L);
#endif

#if LV_USE_LED
  luavgl_led_init(L);
#endif

#if LV_USE_LIST
  luavgl_list_init(L);
#endif

#if LV_USE_TEXTAREA
  luavgl_textarea_init(L);
#endif

#if LV_USE_KEYBOARD
  luavgl_keyboard_init(L);
#endif

#if LV_USE_CHECKBOX
  luavgl_checkbox_init(L);
#endif

#if LV_USE_CALENDAR
  luavgl_calendar_init(L);
#endif

#if LV_USE_ROLLER
  luavgl_roller_init(L);
#endif

#if LV_USE_DROPDOWN
  luavgl_dropdown_init(L);
#endif

}
