local container = lugl.Object(nil, {
    w = lugl.HOR_RES(),
    h = lugl.VER_RES(),
    bg_color = "#888",
    bg_opa = lugl.OPA(100),
    border_width = 0,
    radius = 0,
    flex = {
        flex_direction = "row",
        flex_wrap = "wrap"
    }
})

local function createBtn(parent, name)
    local root = parent:Object{
        w = lugl.SIZE_CONTENT,
        h = lugl.SIZE_CONTENT,
        bg_color = "#ccc",
        bg_opa = lugl.OPA(100),
        border_width = 0,
        radius = 10,
        pad_all = 20,
    }

    root:onClicked(function ()
        container:delete()
        require(name)
    end)

    root:Label {
        text= name,
        text_color = "#333",
        align = {
            type = lugl.ALIGN.CENTER
        }
    }
end


createBtn(container, "font")
createBtn(container, "uservalue")
createBtn(container, "roller")
createBtn(container, "FlappyBird/FlappyBird")
