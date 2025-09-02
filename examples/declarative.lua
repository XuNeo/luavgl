local lvgl = require("lvgl")

local Object = lvgl.Object
local Button = lvgl.Button
local Label = lvgl.Label

Object {
    flex = {
        flex_direction = "row",
        flex_wrap = "wrap",
        justify_content = "center",
        align_items = "center",
        align_content = "center",
    },
    w = 400,
    h = 100,
    align = lvgl.ALIGN.CENTER,

    -- Button with label, inside a container
    Object {
        w = 150,
        h = lvgl.PCT(80),
        bg_color = "#aa0",

        Button {
            Label {
                text = string.format("BUTTON %d", 1),
                align = lvgl.ALIGN.CENTER
            }
        }:center()
    }:clear_flag(lvgl.FLAG.SCROLLABLE),

    -- Label inside a container
    Object {
        w = 150,
        h = lvgl.PCT(80),
        Label {
            text = string.format("label %d", 2)
        }:center()
    }:clear_flag(lvgl.FLAG.SCROLLABLE)
}
