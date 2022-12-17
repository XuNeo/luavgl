---@meta
---
--- lugl comments
---

lugl = {}

--- constans table. Note that value listed here is only for linter.

--- object flag for obj:add_flag obj:clear_flag
--- @enum ObjFlag
lugl.FLAG = {
    PRESSED = 0,
    HIDDEN = 0,
    CLICKABLE = 0,
    CLICK_FOCUSABLE = 0,
    CHECKABLE = 0,
    SCROLLABLE = 0,
    SCROLL_ELASTIC = 0,
    SCROLL_MOMENTUM = 0,
    SCROLL_ONE = 0,
    SCROLL_CHAIN_HOR = 0,
    SCROLL_CHAIN_VER = 0,
    SCROLL_CHAIN = 0,
    SCROLL_ON_FOCUS = 0,
    SCROLL_WITH_ARROW = 0,
    SNAPPABLE = 0,
    PRESS_LOCK = 0,
    EVENT_BUBBLE = 0,
    GESTURE_BUBBLE = 0,
    ADV_HITTEST = 0,
    IGNORE_LAYOUT = 0,
    FLOATING = 0,
    OVERFLOW_VISIBLE = 0,
    LAYOUT_1 = 0,
    LAYOUT_2 = 0,
    WIDGET_1 = 0,
    WIDGET_2 = 0,
    USER_1 = 0,
    USER_2 = 0,
    USER_3 = 0,
    USER_4 = 0,
}

--- @enum ObjEvent
lugl.EVENT = {
    ALL = 0,
    PRESSED = 0,
    PRESSING = 0,
    PRESS_LOST = 0,
    SHORT_CLICKED = 0,
    LONG_PRESSED = 0,
    LONG_PRESSED_REPEAT = 0,
    CLICKED = 0,
    RELEASED = 0,
    SCROLL_BEGIN = 0,
    SCROLL_END = 0,
    SCROLL = 0,
    GESTURE = 0,
    KEY = 0,
    FOCUSED = 0,
    DEFOCUSED = 0,
    LEAVE = 0,
    HIT_TEST = 0,
    COVER_CHECK = 0,
    REFR_EXT_DRAW_SIZE = 0,
    DRAW_MAIN_BEGIN = 0,
    DRAW_MAIN = 0,
    DRAW_MAIN_END = 0,
    DRAW_POST_BEGIN = 0,
    DRAW_POST = 0,
    DRAW_POST_END = 0,
    DRAW_PART_BEGIN = 0,
    DRAW_PART_END = 0,
    VALUE_CHANGED = 0,
    INSERT = 0,
    REFRESH = 0,
    READY = 0,
    CANCEL = 0,
    DELETE = 0,
    CHILD_CHANGED = 0,
    CHILD_CREATED = 0,
    CHILD_DELETED = 0,
    SCREEN_UNLOAD_START = 0,
    SCREEN_LOAD_START = 0,
    SCREEN_LOADED = 0,
    SCREEN_UNLOADED = 0,
    SIZE_CHANGED = 0,
    STYLE_CHANGED = 0,
    LAYOUT_CHANGED = 0,
    GET_SELF_SIZE = 0,
}

--- @enum ObjAlignMode
lugl.ALIGN = {
    DEFAULT = 0,
    TOP_LEFT = 0,
    TOP_MID = 0,
    TOP_RIGHT = 0,
    BOTTOM_LEFT = 0,
    BOTTOM_MID = 0,
    BOTTOM_RIGHT = 0,
    LEFT_MID = 0,
    RIGHT_MID = 0,
    CENTER = 0,
    OUT_TOP_LEFT = 0,
    OUT_TOP_MID = 0,
    OUT_TOP_RIGHT = 0,
    OUT_BOTTOM_LEFT = 0,
    OUT_BOTTOM_MID = 0,
    OUT_BOTTOM_RIGHT = 0,
    OUT_LEFT_TOP = 0,
    OUT_LEFT_MID = 0,
    OUT_LEFT_BOTTOM = 0,
    OUT_RIGHT_TOP = 0,
    OUT_RIGHT_MID = 0,
    OUT_RIGHT_BOTTOM = 0,
}

--- @enum ObjState
lugl.STATE = {
    DEFAULT = 0,
    CHECKED = 0,
    FOCUSED = 0,
    FOCUS_KEY = 0,
    EDITED = 0,
    HOVERED = 0,
    PRESSED = 0,
    SCROLLED = 0,
    DISABLED = 0,
    USER_1 = 0,
    USER_2 = 0,
    USER_3 = 0,
    USER_4 = 0,
    ANY = 0,
}

lugl.ANIM_REPEAT_INFINITE = 0xFFFF

---
--[[
    ### Create basic object
    - when parent is nil, object is created on lugl root.
    - property can be used to set any object style, like using `lv_obj_set_style`
]]
--- @param parent? Object | nil
--- @param property? ObjStyle
--- @return Object
function lugl.Object(parent, property)
end

---
--- Create Image on parent
--- @param parent? Object | nil
--- @param property? ObjStyle
--- @return Image
function lugl.Image(parent, property)
end

---
--- Create Label on parent
--- @param parent? Object | nil
--- @param property? ObjStyle
--- @return Label
function lugl.Label(parent, property)
end

---
--- Create Timer
--- @param p TimerPara
--- @return Timer
function lugl.Timer(p)
end

--[[
Return a font which is lightuserdata, to be used for label etc.
]]
--- @param family string | "montserrat" | "unscii" | "Your Extension Font Name"
--- @param size number the font size in px
--- @param weight @see built-in font only support "normal" weight
---             | number # default to normal, weight 400
---             | nil # default to normal, weight 400
---             | "thin" # weight 100
---             | "extra light" # weight 200
---             | "light" # weight 300
---             | "normal" # weight 400
---             | "medium" # weight 500
---             | "semi bold" # weight 600
---             | "bold" # weight 700
---             | "extra bold" # weight 800
---             | "ultra bold" # weight 900
--- @return Font
---
--- [View Wiki](https://github.com/sumneko/lua-language-server/wiki/Annotations#class)
function lugl.Font(family, size, weight)
end

---
--- Basic lvgl object
--- @class Object
local obj = {}

---
--- Create object on object
--- @overload fun():Object
--- @param property ObjStyle
--- @return Object
function obj:Object(property)
end

---
--- Create image on object
--- @overload fun():Object
--- @param property ImageStyle
--- @return Image
function obj:Image(property)
end

---
--- Create image on object
--- @overload fun():Object
--- @param property ImageStyle
--- @return Label
function obj:Label(property)
end

---
--- Set object property
--- @param p ObjStyle
---
function obj:set(p)
end

---
--- Delete obj
--- @return nil
function obj:delete()
end

---
--- Set parent, note parent should also created by lua, native object may not work.
--- @param p Object
--- @return nil
function obj:set_parent(p)
end

---
--- get screen object where this one created on(non-native object)
--- @return Object
function obj:get_screen(p)
end

---
--- get the state of this object
--- @return ObjState
function obj:get_state(p)
end

---
--- Scroll to a given coordinate on an object.
--- @class ScrollToPara
--- @field x    integer position x
--- @field y    integer
--- @field anim boolean
---
--- @param p ScrollToPara
--- @return ObjState
function obj:scroll_to(p)
end

---
--- Tell whether an object is being scrolled or not at this moment
--- @return boolean
function obj:is_scrolling()
end

---
--- Tell whether an object is visible (even partially) now or not
--- @return boolean
function obj:is_visible()
end

---
--- Set object property
--- @param p ObjFlag
--- @return nil
function obj:add_flag(p)
end

---
--- Set object property
--- @param p ObjFlag
--- @return nil
function obj:clear_flag(p)
end

---
--- Object event callback. `para` is not used for now.
--- @alias EventCallback fun(event: ObjEvent, para: any): nil
---
--- Set object property
--- @param code ObjEvent
--- @param cb EventCallback
--- @return nil
function obj:onevent(code, cb)
end

---
--- Create anim for object
--- @param p AnimPara
--- @return Anim
function obj:anim(p)
end

---
--- Create multiple animation for object
--- @param p AnimPara[]
--- @return Anim[] anim handlers in array
function obj:anims(p)
end

---
--- Image widget
---@class Image:Object
---
local img = {}

--- Image set method
--- @param p ImageStyle
--- @return nil
function img:set(p)
end

--- set image source
--- @param src string image source path
--- @return nil
function img:set_src(src)
end

--- set image offset
--- img:set_offset{x = 0, y = 100}
--- @param p table
--- @return nil
function img:set_offset(p)
end

--- set image pivot
--- img:set_pivot{x = 0, y = 100}
--- @param p table
--- @return nil
function img:set_pivot(p)
end

--- get image size, return w,h
--- w, h = img:get_img_size()
--- w, h = img:get_img_size("/path/to/this/image.png")
--- @param src ? string
--- @return integer[]
function img:get_img_size(src)
end

---
--- Label widget
---@class Label: Object
---
local label = {}

--- Image set method
--- @param p LabelStyle
--- @return nil
function label:set(p)
end

---
--- Anim
---@class Anim
---
local anim = {}

--- start animation
--- @return nil
function anim:start()
end

--- stop animation
--- @return nil
function anim:stop()
end

--- delete animation
--- @return nil
function anim:delete()
end

---
--- Timer
---@class Timer
---
local timer = {}

--- set timer property
--- @param p TimerPara
--- @return nil
function timer:set(p)
end

--- resume timer
--- @return nil
function timer:resume()
end

--- pause timer
--- @return nil
function timer:pause()
end

--- delete timer
--- @return nil
function timer:delete()
end

--- make timer ready now, cb will be made soon on next loop
--- @return nil
function timer:ready()
end

--[[
Font is a light userdata that can be uset to set style text_font.
]]
--- @class Font : lightuserdata
---

local font = {}

---
--- ObjStyle definition
---

---
--- Align parameter
--- @class Align
--- @field align number
--- @field x_ofs number
--- @field y_ofs number

--- AlignTo parameter
--- @class AlignTo
--- @field align number
--- @field base Object
--- @field x_ofs number

--- Object style
--- @class ObjStyle
--- @field w number
--- @field width number
--- @field min_width number
--- @field max_width number
--- @field height number
--- @field min_height number
--- @field max_height number
--- @field x number
--- @field y number
--- @field size number set size is equilent to set w/h to same value
--- @field align Align
--- @field align_to AlignTo
--- @field transform_width number
--- @field transform_height number
--- @field translate_x number
--- @field translate_y number
--- @field transform_zoom number
--- @field transform_angle number
--- @field pad_all number
--- @field pad_top number
--- @field pad_bottom number
--- @field pad_ver number
--- @field pad_left number
--- @field pad_right number
--- @field pad_hor number
--- @field pad_row number
--- @field pad_column number
--- @field pad_gap number
--- @field bg_color number
--- @field bg_opa number
--- @field bg_grad_color number
--- @field bg_grad_dir number
--- @field bg_main_stop number
--- @field bg_grad_stop number
--- @field bg_dither_mode number
--- @field bg_img_src number
--- @field bg_img_opa number
--- @field bg_img_recolor number
--- @field bg_img_recolor_opa number
--- @field bg_img_tiled number
--- @field border_color number
--- @field border_opa number
--- @field border_width number
--- @field border_side number
--- @field border_post number
--- @field outline_width number
--- @field outline_color number
--- @field outline_opa number
--- @field outline_pad number
--- @field shadow_width number
--- @field shadow_ofs_x number
--- @field shadow_ofs_y number
--- @field shadow_spread number
--- @field shadow_color number
--- @field shadow_opa number
--- @field img_opa number
--- @field img_recolor number
--- @field img_recolor_opa number
--- @field line_width number
--- @field line_dash_width number
--- @field line_dash_gap number
--- @field line_rounded number
--- @field line_color number
--- @field line_opa number
--- @field arc_width number
--- @field arc_rounded number
--- @field arc_color number
--- @field arc_opa number
--- @field arc_img_src number
--- @field text_color number | string text color in hex number or #RGB or #RRGGBB format
--- @field text_opa number
--- @field text_font Font
--- @field text_letter_space number
--- @field text_line_space number
--- @field text_decor number
--- @field text_align number
--- @field radius number
--- @field clip_corner number
--- @field opa number
--- @field color_filter_opa number
--- @field anim_time number
--- @field anim_speed number
--- @field blend_mode number
--- @field layout number
--- @field base_dir number
---

--- Image style
--- @class ImageStyle :ObjStyle
--- @field src string
--- @field offset_x number offset of image
--- @field offset_y number
--- @field angle number
--- @field zoom number
--- @field antialias boolean
--- @field pivot table
---

--- Label style
--- @class LabelStyle :ObjStyle
--- @field text string

---
--- Anim(for object) parameter
--- @class AnimPara
--- @field run boolean run this anim right now, or later using anim:start(). default: false
--- @field start_value integer start value
--- @field end_value integer
--- @field duration integer Anim duration in milisecond
--- @field repeat_count integer Anim repeat count, default: 1, set to 0 to disable repeat, set to lugl.ANIM_REPEAT_INFINITE for infinite repeat, set to any other integer for specified repeate count
--- @field early_apply boolean set start_value right now or not. default: true


---
--- Timer para
--- @class TimerPara
--- @field paused boolean Do not start timer immediaely
--- @field period number timer period in ms unit
--- @field repeat_count number | -1 |
---

return lugl
