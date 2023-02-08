-- demo of external widget added to lugl. See simulator/extension.c

local function fs_example()
    -- local f <close>, msg, code = lugl.open_file("/tmp.txt") -- for lua 5.4
    local f, msg, code = lugl.fs.open_file("/tmp.txt", "rw")
    if not f then
        print("failed: ", msg, code)
        return
    end

    print("f: ", f)

    f:write("0123456789", 123, "\n", "the remaining text")
    f:seek("set", 0) -- go back
    local header, remaining = f:read(10, "*a")
    f:close()
    if not header then
        print("read failed or EOF")
        return
    end

    print("header len:", #header, ": ", header)
    print("remaining: ", remaining)

    lugl.Label(nil, {
        x = 0,
        y = 0,
        text_font = lugl.Font("montserrat", 20, "normal"),
        text = header .. remaining,
        align = lugl.ALIGN.TOP_LEFT
    })

    local list = lugl.List(nil, {
        align = lugl.ALIGN.TOP_RIGHT,
        pad_all = 10,
        text_font = lugl.BUILTIN_FONT.MONTSERRAT_12
    })
    list:add_text("Directory list:")

    -- local dir <close>, msg, code = lugl.fs.open_dir("/")
    local dir, msg, code = lugl.fs.open_dir("/")
    if not dir then
        print("open dir failed: ", msg, code)
        return
    end

    while true do
        local d = dir:read()
        if not d then break end
        local is_dir = string.byte(d, 1) == string.byte("/", 1)
        local str = (is_dir and "dir: " or "file: ") .. d
        print(str)

        list:add_text(str):set{
            border_width = 1,
        }
    end
    dir:close()
end

fs_example()
