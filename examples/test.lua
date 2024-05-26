local function Object(t)
    local info = debug.getinfo(2, "nS") -- 2 refers to the caller of the function
    print("Called from: " .. info.short_src .. ", function " .. (info.name or 'unknown'))
    print("table content:")
    obj = {table.unpack(t)}
    for k, v in pairs(t) do
        print(k, v)
    end
    return obj
end

Object({
    a = "789",
    Object({
        a = "101112",
    })
})
