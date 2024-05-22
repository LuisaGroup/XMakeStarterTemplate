-- optional: Minimum xmake version requirement
set_xmakever("2.9.1")
-- optional: build rules (optional)
add_rules("mode.release", "mode.debug", "mode.releasedbg")

-- optional: set options in luisa-compute
-- also writen as:
-- xmake f --enable_tests=false
set_config("enable_tests", false)

-- include luisa-compute
includes("ext/LuisaCompute")

-- define a target
target("demo")
    set_kind("binary")
    -- optional: Use LuisaCompute's optimized settings, 
    -- _config_project({
    --     project_kind = "binary"
    -- })
    add_deps("lc-dsl", "stb-image")
    -- brutely add all cpp files
    add_files("src/**.cpp")
-- optional: mark end (good habit)
target_end()