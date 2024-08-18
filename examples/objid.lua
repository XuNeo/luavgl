local lvgl = require("lvgl")

local container = lvgl.Label {
    w = lvgl.HOR_RES(),
    h = lvgl.VER_RES(),
    bg_color = "#888",
    bg_opa = lvgl.OPA(100),
    border_width = 0,
    radius = 0,
    flex = {
        flex_direction = "row",
        flex_wrap = "wrap"
    }
}

local obj1 = container:Object()
obj1.id = "Obj1"

local obj2 = obj1:Object()
obj2.id = "child"

local obj3 = container:Object()
obj3.id = "Obj3"

local obj4 = obj3:Object()
obj4.id = "child"

local failed = false
local obj = lvgl.get_child_by_id("Obj1") -- obj1
failed = failed or obj ~= obj1
print("obj id", obj.id)

local obj = obj1:get_child_by_id("child") -- obj1
print("obj id", obj.id, obj == obj2)
failed = failed or obj ~= obj2


local obj = obj3:get_child_by_id("child") -- obj1
print("obj id", obj.id, obj == obj4)
failed = failed or obj ~= obj4

lvgl.Label{
    text_color = "#333",
    align = lvgl.ALIGN.CENTER,
    text = "Test " .. (failed and "Failed" or "Success")
}