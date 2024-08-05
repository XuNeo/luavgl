local container = lvgl.Object {
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

-- change color directly using property
container.bg_color = lvgl.palette.main(lvgl.palette.BLUE_GREY)

print("created container", container)

local function createBtn(parent, name)
    local root = parent:Object {
        w = lvgl.SIZE_CONTENT,
        h = lvgl.SIZE_CONTENT,
        bg_color = "#ccc",
        bg_opa = lvgl.OPA(100),
        border_width = 0,
        radius = 10,
        pad_all = 20,
    }

    root:onClicked(function()
        container:delete()
        require(name)
    end)

    root:Label {
        text = name,
        text_color = "#333",
        align = lvgl.ALIGN.CENTER,
    }
end

createBtn(container, "keyboard")
createBtn(container, "animation")
createBtn(container, "pointer")
createBtn(container, "analogTime")
createBtn(container, "userdata")
createBtn(container, "flappyBird/flappyBird")
createBtn(container, "tests")
