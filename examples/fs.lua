-- demo of external widget added to lugl. See simulator/extension.c

local function fs_example()
    -- local f <close>, msg = lugl.open("/tmp.txt") -- for lua 5.4
    local f, msg = lugl.open("/tmp.txt", "rw")
    if not f then
        print("failed: ", msg)
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
        x = 0, y = 0,
        text_font = lugl.Font("montserrat", 20, "normal"),
        text = header .. remaining,
        align = lugl.ALIGN.CENTER
    })


end

fs_example()
