local lugl = require("lugl")
local CONST = lugl.const

local label1 = lugl.Label(nil, {
    x = 0, y = 0,
    text_font = lugl.Font("montserrat", 32, "normal"),
    text = "Hello Font",
    align = {
        type = lugl.ALIGN.CENTER
    }
})

print("label1: ", label1)

lugl.Label(nil, {
    -- x= 0, y = 0,
    text_font = lugl.Font("MiSansW medium, montserrat", 24),
    text = "Hello Font2",
}):align_to({
    type = lugl.ALIGN.OUT_BOTTOM_LEFT,
    base = label1,
})
