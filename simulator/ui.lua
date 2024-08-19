STRINGIZE((function() --\n
    local root = lvgl.get_child_by_id("luaUIroot")
    root.w = lvgl.VER_RES()
    root.h = lvgl.HOR_RES()

    Button(root, {
        id = "Button in Lua",
        Label {
            text = "Hello, lua, C and lvgl!",
            align = lvgl.ALIGN_CENTER
        }
    }):center()

-- Keep this comment
end)())
