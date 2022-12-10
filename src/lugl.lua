---@meta
---
--- lugl comments
---

lugl = {}

---
--[[
    ### Create basic object
    - when parent is nil, object is created on lugl root.
    - property can be used to set any object style, like using `lv_obj_set_style`
]]
--- @overload fun(): Object
--- @overload fun(parent: Object | nil): Object
--- @param parent Object | nil
--- @param property ObjStyle
--- @return Object
function lugl.Object(parent, property)
end

---
--- Create Image on parent
--- @overload fun():Image
--- @overload fun(parent: Object|nil):Image
--- @param parent Object | nil
--- @param property ObjStyle
--- @return Image
function lugl.Image(parent, property)
end

---
--- Create Label on parent
--- @overload fun(): Label
--- @overload fun(parent: Object | nil): Label
--- @param parent Object | nil
--- @param property ObjStyle
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
--- Set object property
--- @param p ObjStyle
---
function obj:set(p)
end

---
--- Image widget
---@class Image:Object
---
local img = {}

--- Image set method
--- @param p ImageStyle
--- @return nil
function img:imgset(p)
end

---
--- Label widget
---@class Label: Object
---
local label = {}

---
--- Timer
---@class Timer
---
local time = {}

--- @return Timer

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
--- @field text_color number
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

---
--- Timer para
--- @class TimerPara
--- @field paused boolean Do not start timer immediaely
--- @field period number timer period in ms unit
--- @field repeat_count number | -1 |
---

return lugl
