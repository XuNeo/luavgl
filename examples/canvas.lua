local root = lvgl.Object(nil, {
    w = 120,
    h = 120,
    border_width = 1,
    border_color = "#000",
    radius = 10,
    align = lvgl.ALIGN.CENTER,
})

local canvas = lvgl.Canvas(root, {
    border_width = 1,
    border_color = "#F00",
    align = lvgl.ALIGN.CENTER,
})

canvas:set {
    buffer = {
        w = 100,
        h = 100,
        cf = 5,
    },
}

canvas:set {
    img_recolor = 0xff0000ff,
}

canvas:fill_bg("#000", lvgl.OPA(100))

canvas:blur_ver(
    {
        x1 = 10,
        y1 = 10,
        x2 = 90,
        y2 = 90,
    },
    80
)
