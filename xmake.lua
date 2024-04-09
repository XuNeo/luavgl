add_rules("mode.debug", "mode.release")

add_requires("lua 5.4", "libsdl")

add_includedirs("simulator") -- need lv_conf.h and lv_drv_conf.h

target("lvgl")
    set_kind("static")
    add_includedirs("deps/lvgl", {public=true})
    add_packages("libsdl")
    on_load(function (target)
        local ProjectLetter = os.projectdir():sub(1, 1)
        local LV_FS_STDIO_PLAT_LETTER = string.format("LV_FS_STDIO_PLAT_LETTER='%s'", ProjectLetter)
        target:add("defines", LV_FS_STDIO_PLAT_LETTER)
        if is_plat("windows") then 
            target:add("defines", "WIN32")
        end
    end)
    add_files("deps/lvgl/**.c|demos/**.c|examples/**.c|tests/**.c")

target("luavgl")
    set_kind("static")
    add_deps("lvgl")
    add_packages("lua")
    add_includedirs("src/", {public=true})
    on_load(function (target) 
        local ProjectLetter = os.projectdir():sub(1, 1)
        local LV_FS_STDIO_PLAT_LETTER = string.format("LV_FS_STDIO_PLAT_LETTER='%s'", ProjectLetter)
        target:add("defines", "LV_CONF_INCLUDE_SIMPLE", "PATH_MAX=1024", LV_FS_STDIO_PLAT_LETTER)
    end)
    add_files("src/luavgl.c")

target("simulator")
    set_kind("binary")
    add_deps("luavgl")
    add_packages("lua")
    on_load(function (target) 
        local LUAVGL_EXAMPLE_DIR = string.format("%s/examples", string.gsub(os.projectdir(), "\\", "/"))
        LUAVGL_EXAMPLE_DIR = string.format("LUAVGL_EXAMPLE_DIR=\"%s\"", LUAVGL_EXAMPLE_DIR)
        target:add("defines", LUAVGL_EXAMPLE_DIR)
    end)
    add_defines(string.format("LUAVGL_EXAMPLE_DIR=\"%s/examples\"", string.gsub(os.projectdir(), "\\", "/")), "LV_CONF_INCLUDE_SIMPLE")
    add_files("simulator/**.c|tt.c")
    set_rundir("$(scriptdir)")