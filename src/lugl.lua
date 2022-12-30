---@meta
---
--- lugl comments
---

lugl = {}

--- constans table. Note that value listed here is only for linter.

--- @enum ObjEventCode
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

--- @enum ObjAlignType
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

--- @enum BuiltinFont
lugl.BUILTIN_FONT = {
    MONTSERRAT_8 = 0,
    MONTSERRAT_10 = 0,
    MONTSERRAT_12 = 0,
    MONTSERRAT_14 = 0,
    MONTSERRAT_16 = 0,
    MONTSERRAT_18 = 0,
    MONTSERRAT_20 = 0,
    MONTSERRAT_22 = 0,
    MONTSERRAT_24 = 0,
    MONTSERRAT_26 = 0,
    MONTSERRAT_28 = 0,
    MONTSERRAT_30 = 0,
    MONTSERRAT_32 = 0,
    MONTSERRAT_34 = 0,
    MONTSERRAT_36 = 0,
    MONTSERRAT_38 = 0,
    MONTSERRAT_40 = 0,
    MONTSERRAT_42 = 0,
    MONTSERRAT_44 = 0,
    MONTSERRAT_46 = 0,
    MONTSERRAT_48 = 0,
    MONTSERRAT_12_SUBPX = 0,
    MONTSERRAT_28_COMPRESSED = 0,
    DEJAVU_16_PERSIAN_HEBREW = 0,
    SIMSUN_16_CJK = 0,
    UNSCII_8 = 0,
    UNSCII_16 = 0,
}

--- @enum LABEL
lugl.LABEL = {
    LONG_WRAP = 0,
    LONG_DOT = 0,
    LONG_SCROLL = 0,
    LONG_SCROLL_CIRCULAR = 0,
    LONG_CLIP = 0,
}

--- @enum SCR_LOAD_ANIM
lugl.SCR_LOAD_ANIM = {
    NONE = 0,
    OVER_LEFT = 0,
    OVER_RIGHT = 0,
    OVER_TOP = 0,
    OVER_BOTTOM = 0,
    MOVE_LEFT = 0,
    MOVE_RIGHT = 0,
    MOVE_TOP = 0,
    MOVE_BOTTOM = 0,
    FADE_IN = 0,
    FADE_ON = 0,
    FADE_OUT = 0,
    OUT_LEFT = 0,
    OUT_RIGHT = 0,
    OUT_TOP = 0,
    OUT_BOTTOM = 0,
}

--- @enum ScrollbarMode
lugl.SCROLLBAR_MODE = {
    OFF = 0,
    ON = 0,
    ACTIVE = 0,
    AUTO = 0,
}

--- @enum Dir
lugl.DIR = {
    NONE = 0,
    LEFT = 0,
    RIGHT = 0,
    TOP = 0,
    BOTTOM = 0,
    HOR = 0,
    VER = 0,
    ALL = 0,
}

--- @enum KeyboardMode
lugl.KEYBOARD_MODE = {
    TEXT_LOWER = 0,
    TEXT_UPPER = 0,
    SPECIAL = 0,
    NUMBER = 0,
    USER_1 = 0,
    USER_2 = 0,
    USER_3 = 0,
    USER_4 = 0,
    TEXT_ARABIC = 0,
}

--- @enum FlexFlow
lugl.FLEX_FLOW = {
    ROW = 0,
    COLUMN = 0,
    ROW_WRAP = 0,
    ROW_REVERSE = 0,
    ROW_WRAP_REVERSE = 0,
    COLUMN_WRAP = 0,
    COLUMN_REVERSE = 0,
    COLUMN_WRAP_REVERSE = 0,
}

--- @enum FlexAlign
lugl.FLEX_FLOW = {
    START = 0,
    END = 0,
    CENTER = 0,
    SPACE_EVENLY = 0,
    SPACE_AROUND = 0,
    SPACE_BETWEEN = 0,
}

--- @enum GridAlign
lugl.GRID_ALIGN = {
    START = 0,
    CENTER = 0,
    END = 0,
    STRETCH = 0,
    SPACE_EVENLY = 0,
    SPACE_AROUND = 0,
    SPACE_BETWEEN = 0,
}

--- @enum RollerMode
lugl.ROLLER_MODE = {
    NORMAL = 0,
    INFINITE = 0,
}

lugl.ANIM_REPEAT_INFINITE = 0
lugl.ANIM_PLAYTIME_INFINITE = 0
lugl.SIZE_CONTENT = 0
lugl.RADIUS_CIRCLE = 0
lugl.COORD_MAX = 0
lugl.COORD_MIN = 0
lugl.IMG_ZOOM_NONE = 0
lugl.BTNMATRIX_BTN_NONE = 0
lugl.CHART_POINT_NONE = 0
lugl.DROPDOWN_POS_LAST = 0
lugl.LABEL_DOT_NUM = 0
lugl.LABEL_POS_LAST = 0
lugl.LABEL_TEXT_SELECTION_OFF = 0
lugl.TABLE_CELL_NONE = 0
lugl.TEXTAREA_CURSOR_LAST = 0

--- return a opacity value in lvgl unit(0..255)
---@param p integer opacity value in range of 0..100
---@return integer
function lugl.OPA(p)
end

--- return a layout in percent
---@param p integer
---@return integer
function lugl.PCT(p)
end

---
--[[
    ### Create basic object
    - when parent is nil, object is created on lugl root.
    - property can be used to set any object style, like using `lv_obj_set_style`
]]
--- @param parent? Object | nil
--- @param property? StyleProp
--- @return Object
function lugl.Object(parent, property)
end

---
--- Create Label on parent
--- @param parent? Object | nil
--- @param property? CheckboxStyle
--- @return Checkbox
function lugl.Checkbox(parent, property)
end

---
--- Create Dropdown on parent
--- @param parent? Object | nil
--- @param property? DropdownStyle
--- @return Dropdown
function lugl.Dropdown(parent, property)
end

---
--- Create Image on parent
--- @param parent? Object | nil
--- @param property? ImageStyle
--- @return Image
function lugl.Image(parent, property)
end

---
--- Create Label on parent
--- @param parent? Object | nil
--- @param property? LabelStyle
--- @return Label
function lugl.Label(parent, property)
end

---
--- Create Textarea Widget on parent
--- @param parent? Object | nil
--- @param property? TextareaStyle
--- @return Textarea
function lugl.Textarea(parent, property)
end

---
--- Create Keyboard Widget on parent
--- @param parent? Object | nil
--- @param property? KeyboardStyle
--- @return Keyboard
function lugl.Keyboard(parent, property)
end


---
--- Create Led Widget on parent
--- @param parent? Object | nil
--- @param property? LedStyle
--- @return Led
function lugl.Led(parent, property)
end

---
--- Create List Widget on parent
--- @param parent? Object | nil
--- @param property? LabelStyle
--- @return List
function lugl.List(parent, property)
end

---
--- Create Roller Widget on parent
--- @param parent? Object | nil
--- @param property? RollerStyle
--- @return Roller
function lugl.Roller(parent, property)
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
--- Create style
--- @param p? StyleProp
--- @return Style
function lugl.Style(p)
end

---
--- Basic lvgl object
--- @class Object
local obj = {}

---
--- Create object on object
--- @param property? StyleProp
--- @return Object
function obj:Object(property)
end

---
--- Create checkbox on object
--- @param property? CheckboxStyle
--- @return Checkbox
function obj:Checkbox(property)
end

---
--- Create Dropdown on parent
--- @param property? DropdownStyle
--- @return Dropdown
function obj.Dropdown(parent, property)
end

---
--- Create image on object
--- @param property? ImageStyle
--- @return Image
function obj:Image(property)
end

---
--- Create image on object
--- @param property? LabelStyle
--- @return Label
function obj:Label(property)
end

---
--- Create Textarea Widget on parent
--- @param property? TextareaStyle
--- @return Textarea
function obj:Textarea(property)
end

---
--- Create Keyboard Widget on parent
--- @param property? KeyboardStyle
--- @return Keyboard
function obj:Keyboard(property)
end

---
--- Create Led Widget on parent
--- @param property? LedStyle
--- @return Led
function obj:Led(property)
end

---
--- Create List on object
--- @param property? ListStyle
--- @return List
function obj:List(property)
end


---
--- Create Roller Widget on parent
--- @param property? RollerStyle
--- @return Roller
function obj:Roller(parent, property)
end
---
--- Set object property
--- @param p StyleProp
---
function obj:set(p)
end

---
--- Set object property
--- @param p StyleProp
--- @param state ObjState
---
function obj:set_style(p, state)
end

---
--- Set object property
--- @param p AlignToPara
---
function obj:align_to(p)
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
--- get parent object
--- @return Object
function obj:get_parent()
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
--- add style to object
--- @param s Style
--- @param selector? integer
--- @return nil
function obj:add_style(s, selector)
end

---
--- remove style from object
--- @param s Style
--- @param selector? integer
--- @return nil
function obj:remove_style(s, selector)
end

---
--- remove all style from object
--- @return nil
function obj:remove_style_all()
end

---scroll obj by x,y
---@param x integer
---@param y integer
---@param anim_en? boolean
function obj:scroll_by(x, y, anim_en)
end

---scroll obj by x,y
---@param x integer
---@param y integer
---@param anim_en boolean
function obj:scroll_by_bounded(x, y, anim_en)
end

--- Scroll to an object until it becomes visible on its parent
---@param anim_en? boolean
function obj:scroll_to_view(anim_en)
end

--- Scroll to an object until it becomes visible on its parent
--- Do the same on the parent's parent, and so on.
--- Therefore the object will be scrolled into view even it has nested scrollable parents
---@param anim_en? boolean
function obj:scroll_to_view_recursive(anim_en)
end

---scroll obj by x,y, low level APIs
---@param x integer
---@param y integer
---@param anim_en boolean
function obj:scroll_by_raw(x, y, anim_en)
end

---Invalidate the area of the scrollbars
function obj:scrollbar_invalidate()
end

---Checked if the content is scrolled "in" and adjusts it to a normal position.
---@param anim_en boolean
function obj:readjust_scroll(anim_en)
end

---If object is editable
---@return boolean
function obj:is_editable()
end

--- class group def
---@return boolean
function obj:is_group_def()
end

--- Test whether the and object is positioned by a layout or not
---@return boolean
function obj:is_layout_positioned()
end

--- Mark the object for layout update.
---@return nil
function obj:mark_layout_as_dirty()
end

--- Align an object to the center on its parent. same as obj:set{align={type = lugl.ALIGN.CENTER}}
---@return nil
function obj:center()
end

--- Align an object to the center on its parent. same as obj:set{align={type = lugl.ALIGN.CENTER}}
---@return nil
function obj:invalidate()
end

---
--- Object event callback. `para` is not used for now.
--- @alias EventCallback fun(obj:Object, code: ObjEventCode): nil
---
--- set object event callback
--- @param code ObjEventCode
--- @param cb EventCallback
--- @return nil
function obj:onevent(code, cb)
end

---
--- set object pressed event callback, same as obj:onevent(lugl.EVENT.PRESSED, cb)
--- @param cb EventCallback
--- @return nil
function obj:onPressed(cb)
end

---
--- set object clicked event callback, same as obj:onevent(lugl.EVENT.CLICKED, cb)
--- @param cb EventCallback
--- @return nil
function obj:onClicked(cb)
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
--- Checkbox widget
---@class Checkbox:Object
---
local checkbox = {}

--- set method
--- @param p CheckboxStyle
--- @return nil
function checkbox:set(p)
end

---
--- Get the text of a label
--- @return string
function checkbox:get_text()
end

---
--- Dropdown widget
---@class Dropdown:Object
---
local dropdown = {}

--- set method
--- @param p DropdownStyle
--- @return nil
function dropdown:set(p)
end

--- get method
--- @param which "list" | "options" | "selected" | "option_cnt" | "selected_str" | "option_index" | "symbol" | "dir"
--- @param arg ? string
--- @return string | Dir | Object
function dropdown:get(which, arg)
end

--- Open the drop down list
function dropdown:open()
end

--- Close (Collapse) the drop-down list
function dropdown:close()
end

--- Tells whether the list is opened or not
function dropdown:is_open()
end

--- Add an options to a drop-down list from a string
--- @param option string
--- @param pos integer
function dropdown:add_option(option, pos)
end

--- Tells whether the list is opened or not
function dropdown:clear_option()
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
--- Get the text of a label
--- @return string
function label:get_text()
end

---
--- Get the long mode of a label
--- @return string
function label:get_long_mode()
end

---
--- Get the recoloring attribute
--- @return string
function label:get_recolor()
end

---
--- Insert a text to a label.
--- @param pos integer
--- @param txt string
--- @return nil
function label:ins_text(pos, txt)
end

---
--- Delete characters from a label.
--- @param pos integer
--- @param cnt integer
--- @return nil
function label:cut_text(pos, cnt)
end

---
--- Textarea widget
---@class Textarea: Object
---
local textarea = {}

--- Textarea set method
--- @param p TextareaStyle
--- @return nil
function textarea:set(p)
end

--- get textarea text
--- @return string
function textarea:get_text(p)
end

---
--- Keyboard widget
---@class Keyboard: Object based on btnmatrix object
---
local keyboard = {}

--- Keyboard set method
--- @param p KeyboardStyle
--- @return nil
function keyboard:set(p)
end

---
--- LED widget
---@class Led: Object
---
local led = {}

--- LED set method
--- @param p LedStyle
--- @return nil
function led:set(p)
end

--- LED set to ON
--- @return nil
function led:on()
end

--- LED set to OFF
--- @return nil
function led:off()
end

--- toggle LED status
--- @return nil
function led:toggle()
end

--- get LED brightness
--- @return integer
function led:get_brightness()
end

---
--- List widget
---@class List: Object
---
local list = {}

--- List set method
--- @param p ListStyle
--- @return nil
function list:set(p)
end

--- add text to list
--- @param text string
--- @return Label
function list:add_text(text)
end

--- add button to list
--- @param icon ImgSrc | nil
--- @param text? string
--- @return Object a button object
function list:add_btn(icon, text)
end

--- get list button text
--- @param btn Object
--- @return string
function list:get_btn_text(btn)
end

---
--- Roller widget
---@class Roller: Object
---
local roller = {}

--- Roller set method
--- @param p RollerStyle
--- @return nil
function roller:set(p)
end

--- Get the options of a roller
--- @return string
function roller:get_options()
end

--- Get the index of the selected option
--- @return integer
function roller:get_selected()
end

--- Get the current selected option as a string.
--- @return string
function roller:get_selected_str()
end

--- Get the total number of options
--- @return integer
function roller:get_options_cnt()
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
--- @class Font : BuiltinFont
---

local font = {}

---
--- @class Style : lightuserdata
---
local style = {}

--- update style properties
--- @param p StyleProp
--- @return nil
function style:set(p)
end

--- delete style, only delted style could be gc'ed
--- @return nil
function style:delete()
end

--- remove specified property from style
--- @param p string property name from field of StyleProp
--- @return nil
function style:remove_prop(p)
end

---
--- Align parameter
--- @class Align
--- @field type ObjAlignType
--- @field x_ofs number
--- @field y_ofs number

--- AlignTo parameter
--- @class AlignToPara
--- @field type ObjAlignType
--- @field base Object
--- @field x_ofs number

--- Style properties
--- @class StyleProp
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
--- @field bg_color number | string text color in hex number or #RGB or #RRGGBB format
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
--- @field border_color number | string
--- @field border_opa number
--- @field border_width number
--- @field border_side number
--- @field border_post number
--- @field outline_width number
--- @field outline_color number | string
--- @field outline_opa number
--- @field outline_pad number
--- @field shadow_width number
--- @field shadow_ofs_x number
--- @field shadow_ofs_y number
--- @field shadow_spread number
--- @field shadow_color number | string
--- @field shadow_opa number
--- @field img_opa number
--- @field img_recolor number
--- @field img_recolor_opa number
--- @field line_width number
--- @field line_dash_width number
--- @field line_dash_gap number
--- @field line_rounded number
--- @field line_color number | string
--- @field line_opa number
--- @field arc_width number
--- @field arc_rounded number
--- @field arc_color number | string
--- @field arc_opa number
--- @field arc_img_src number
--- @field text_color number | string
--- @field text_opa number
--- @field text_font Font | BuiltinFont
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
--- @field flex_flow FlexFlow
--- @field flex_main_place FlexAlign
--- @field flex_cross_place FlexAlign
--- @field flex_track_place FlexAlign
--- @field flex_grow integer 0..255
--- @field flex FlexLayoutPara

---

--- Object style
--- @class ObjectStyle :StyleProp
--- @field x integer
--- @field y integer
--- @field w integer
--- @field h integer
--- @field align Align
--- @field align_to AlignToPara
--- @field scrollbar_mode ScrollbarMode
--- @field scroll_dir Dir
--- @field scroll_snap_x integer
--- @field scroll_snap_y integer
---

--- Image style
--- @class ImageStyle :StyleProp
--- @field src string
--- @field offset_x number offset of image
--- @field offset_y number
--- @field angle number
--- @field zoom number
--- @field antialias boolean
--- @field pivot table
---

--- Label style
--- @class LabelStyle :StyleProp
--- @field text string

--- Checkbox style
--- @class CheckboxStyle :StyleProp
--- @field text string

--- Dropdown style
--- @class DropdownStyle :StyleProp
--- @field text string | nil
--- @field options string
--- @field selected integer
--- @field dir Dir
--- @field symbol lightuserdata | string
--- @field highlight boolean

--- Textarea style
--- @class TextareaStyle :StyleProp
--- @field text string
--- @field placeholder string
--- @field cursor integer cursor position
--- @field password_mode boolean enable password
--- @field one_line boolean enable one line mode
--- @field password_bullet string Set the replacement characters to show in password mode
--- @field accepted_chars string DO NOT USE. Set a list of characters. Only these characters will be accepted by the text area E.g. "+-.,0123456789"
--- @field max_length integer Set max length of a Text Area.
--- @field password_show_time integer Set how long show the password before changing it to '*'

--- Keyboard style
--- @class KeyboardStyle :StyleProp
--- @field textarea Textarea textarea object
--- @field mode KeyboardMode
--- @field popovers boolean Show the button title in a popover when pressed.

--- Led style
--- @class LedStyle :StyleProp
--- @field color integer|string color of led
--- @field brightness integer brightness in range of 0..255

--- List style
--- @class ListStyle :StyleProp

--- Roller style
--- @class RollerStyle :StyleProp
--- @field options table | string
--- @field selected table | integer
--- @field visible_cnt integer

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
--- @alias TimerCallback fun(t:Timer): nil
--- @class TimerPara
--- @field paused boolean Do not start timer immediaely
--- @field period number timer period in ms unit
--- @field repeat_count number | -1 |
--- @field cb TimerCallback
---


---
--- @alias ImgSrc string | lightuserdata

--- @alias flexAlignOptions "flex-start" | "flex-end" | "center" | "space-between" | "space-around" | "space-evenly"
---
--- @class FlexLayoutPara
--- @field flex_direction "row" | "column" | "row-reverse" | "column-reverse"
--- @field flex_wrap "nowrap" | "wrap" | "wrap-reverse"
--- @field justify_content flexAlignOptions
--- @field align_items flexAlignOptions
--- @field align_content flexAlignOptions
return lugl
