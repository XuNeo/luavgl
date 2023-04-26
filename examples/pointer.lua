local root = lvgl.Object(nil, {
    w = 0,
    h = 0,
    pad_all = 0,
    align = lvgl.ALIGN.CENTER,
    border_width = 1,
    border_color = "#F00",
})

local pointer = root:Pointer {
    src = SCRIPT_PATH .. "/assets/second.png",
    x = -20,
    y = -233,
    border_width = 1,
    border_color = "#0F0",
    range = {
        valueStart = 0,
        valueRange = 60000,
        angleStart = 0,
        angleRange = -3600
    },
}

-- lvgl requires src set firstly, since lua cannot guarantee table order,
-- we set pivot after image src is set.
pointer:set {
    pivot = { x = 20, y = 233 },
}

print("img w,h: ", pointer:get_img_size())

pointer:Anim{
    run = true,
    start_value = 0,
    end_value = 60000,
    duration = 60000,
    path = "linear",
    repeat_count = lvgl.ANIM_REPEAT_INFINITE,
    exec_cb = function(obj, value)
        obj:set {
            value = value
        }
    end,
}

print("created pointer: ", pointer)
