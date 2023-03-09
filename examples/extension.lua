-- demo of external widget added to lvgl. See simulator/extension.c

local extension = lvgl.Extension(nil, {
    border_width = 1,
    w = lvgl.PCT(20),
    h = lvgl.PCT(20),
    align = lvgl.ALIGN.CENTER,
})

extension:onClicked(function ()
    print("clicked")
end)

print("extension: ", extension)
