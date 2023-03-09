
#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "luavgl.h"
#include "private.h"

#include "calendar.c"
#include "checkbox.c"
#include "dropdown.c"
#include "img.c"
#include "keyboard.c"
#include "label.c"
#include "led.c"
#include "list.c"
#include "roller.c"
#include "textarea.c"

static int luavgl_obj_create(lua_State *L);

static const luaL_Reg widget_create_methods[] = {
    {"Object",   luavgl_obj_create     },
    {"Calendar", luavgl_calendar_create},
    {"Checkbox", luavgl_checkbox_create},
    {"Dropdown", luavgl_dropdown_create},
    {"Image",    luavgl_img_create     },
    {"Textarea", luavgl_textarea_create},
    {"Keyboard", luavgl_keyboard_create},
    {"Label",    luavgl_label_create   },
    {"Led",      luavgl_led_create     },
    {"List",     luavgl_list_create    },
    {"Roller",   luavgl_roller_create  },
    {NULL,       NULL                }
};

static void luavgl_widgets_init(lua_State *L)
{
  luavgl_img_init(L);
  luavgl_label_init(L);
  luavgl_led_init(L);
  luavgl_list_init(L);
  luavgl_textarea_init(L);
  luavgl_keyboard_init(L);
  luavgl_checkbox_init(L);
  luavgl_calendar_init(L);
  luavgl_roller_init(L);
  luavgl_dropdown_init(L);
}
