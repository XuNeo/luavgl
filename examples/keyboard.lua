local root = lugl.Object(nil, {
    w = lugl.HOR_RES(),
    h = lugl.VER_RES(),
    pad_all = 0,
    bg_color = "#333",
    bg_opa = lugl.OPA(50),
})
root:add_flag(lugl.FLAG.CLICKABLE)

local ta = root:Textarea {
    password_mode = false,
    one_line = true,
    text_font = lugl.Font("montserrat", 22),
    text_color = "#FFF",
    text = "Input text here",
    w = lugl.SIZE_CONTENT,
    h = lugl.SIZE_CONTENT,
    bg_opa = 0,
    border_width = 2,
    pad_all = 2,
    align = lugl.ALIGN.TOP_MID,
}

print("created textarea: ", ta)

local keyboard = root:Keyboard {
    textarea = ta,
    align = lugl.ALIGN.BOTTOM_MID,
}
print("created keyboard: ", keyboard)

ta:onevent(lugl.EVENT.PRESSED, function(obj, code)
    keyboard:clear_flag(lugl.FLAG.HIDDEN)
end)

ta:onevent(lugl.EVENT.READY, function()
    keyboard:add_flag(lugl.FLAG.HIDDEN)
end)

ta:onevent(lugl.EVENT.CANCEL, function(obj, code)
    keyboard:add_flag(lugl.FLAG.HIDDEN)
end)

root:onClicked(function (obj, code)
    keyboard:add_flag(lugl.FLAG.HIDDEN)
end)
