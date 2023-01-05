-- demo of external widget added to lugl. See simulator/extension.c

local extension = lugl.Extension(nil, {
    border_width = 1,
    w = lugl.PCT(20),
    h = lugl.PCT(20),
    align = lugl.ALIGN.CENTER,
})

extension:onClicked(function ()
    print("clicked")
end)

print("extension: ", extension)
