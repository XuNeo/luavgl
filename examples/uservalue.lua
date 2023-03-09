collectgarbage("collect")
print("initial: ", collectgarbage("count"))

local str_t = {}
for i = 1, 1024 * 1024 do
    str_t[#str_t + 1] = 'a'
end

local str = table.concat(str_t)
str_t = nil

collectgarbage("collect")
collectgarbage("collect")
print("after string collect: ", collectgarbage("count"))

for i = 1, 10 do
    local label = lvgl.Label(nil)
    label:set_text_static(str)
    label:delete()
end
str = nil

print("after set text: ", collectgarbage("count"))
collectgarbage("collect")
print("after collect: ", collectgarbage("count"))
print("again: ", collectgarbage("count"))
collectgarbage("collect")
print("again2: ", collectgarbage("count"))
collectgarbage("collect")
print("again3: ", collectgarbage("count"))

local label = lvgl.Label(nil)
label:set({ text = "Test Done" })
label:center()
