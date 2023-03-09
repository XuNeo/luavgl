
local function testCrash()
    print(nil .. "")
end

local function testCrash2()
    testCrash()
end
local function testCrash3()
    testCrash2()
end
local function testCrash4()
    testCrash3()
end

local function testCrash5()
    testCrash4()
end

local function testCrash6()
    testCrash5()
end

lvgl.Label(nil, {
    text = "crash in 1 second.",
    align = lvgl.ALIGN.CENTER
})

lvgl.Timer {
    period = 1000,
    cb = function(t)
        t:delete()
        -- crash in callback function is protected.
        testCrash6()
    end
}