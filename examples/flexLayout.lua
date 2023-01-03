local lugl = require("lugl")

local flex = {}
flex["flex-direction"] = 0

local root = lugl.Object(nil, {
    flex = {
        flex_direction = "row",
        flex_wrap = "wrap",
        justify_content = "center",
        align_items = "center",
        align_content = "center",
    },
    w = 300,
    h = 75,
    align = lugl.ALIGN.CENTER
})

for i = 1, 10 do
    local item = root:Object {
        w = 100,
        h = lugl.PCT(100),
    }
    item:clear_flag(lugl.FLAG.SCROLLABLE)

    local label = item:Label {
        text = string.format("label %d", i)
    }
    label:center()
end
