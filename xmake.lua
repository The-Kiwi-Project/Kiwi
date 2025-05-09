add_rules("mode.debug", "mode.release")
set_languages("c99", "c++20")
add_requires("xlnt", "catch2")

rule("qt.opengl")
    on_config(function (target) 
        import("detect.sdks.find_qt")
        local qt = assert(find_qt(), "Qt SDK not found!")
        local major_version = tonumber(string.match(qt.sdkver, "^(%d+)"))
        if major_version >= 6 then
            target:add("frameworks", "QtOpenGL", "QtOpenGLWidgets")
        else
            target:add("frameworks", "QtOpenGL")
        end
    end)

-- Kiwi Task!!!

target("kiwi")
    set_kind("binary")
    set_targetdir("./output")
    set_default(true)
    add_packages("xlnt")
    add_includedirs("source", "source/global")
    add_files("source/**.cc", "source/widget/**.h", "resource/resource.qrc")
    add_rules("qt.widgetapp", "qt.opengl")

-- Tool Application 

-- Simple tool for get cob index map 
target("cobmap")
    set_kind("binary")
    set_targetdir("./output")
    set_default(false)
    add_includedirs("source", "source/global")
    add_files("tools/cobmap.cc")
    add_files("source/hardware/**.cc")
    add_files("source/global/**.cc")

-- Load config, run P&R and view result in 2D view
target("view2d")
    set_kind("binary")
    set_targetdir("./output")
    set_default(false)
    add_packages("xlnt")
    add_includedirs("source", "source/global")
    add_files(
        "source/algo/**.cc",
        "source/circuit/**.cc",
        "source/global/**.cc",
        "source/hardware/**.cc",
        "source/parse/**.cc",
        "source/serde/**.cc",
        "source/widget/view2d/**.cc",
        "source/widget/view2d/**.h",
        "source/widget/frame/**.h",
        "source/widget/frame/**.cc",
        "tools/view2d.cc"
    )
    add_rules("qt.widgetapp", "qt.opengl")

-- Load config, run P&R and view result in 3D view
target("view3d")
    set_kind("binary")
    set_targetdir("./output")
    set_default(false)
    add_packages("xlnt")
    add_includedirs("source", "source/global")
    add_files(
        "source/algo/**.cc",
        "source/circuit/**.cc",
        "source/global/**.cc",
        "source/hardware/**.cc",
        "source/parse/**.cc",
        "source/serde/**.cc",
        "source/widget/view3d/**.cc",
        "source/widget/view3d/**.h",
        "source/widget/frame/**.h",
        "source/widget/frame/**.cc",
        "resource/resource.qrc",
        "tools/view3d.cc"
    )
    add_rules("qt.widgetapp", "qt.opengl")

-- Test Tasks

target("module_test")
    set_kind("binary")
    set_targetdir("./output")
    set_default(false)
    add_packages("xlnt")
    add_includedirs("source", "source/global", "test/module_test")
    add_files("test/module_test/**.cc")
    add_files(
        "source/algo/**.cc",
        "source/circuit/**.cc",
        "source/global/**.cc",
        "source/hardware/**.cc",
        "source/parse/**.cc",
        "source/serde/**.cc"
    )

target("regression_test")
    set_kind("binary")
    set_targetdir("./output")
    set_default(false)
    add_packages("xlnt", "catch2")
    add_includedirs("source", "source/global", "test/regression_test")
    add_files("test/regression_test/**.cc")
    add_files(
        "source/algo/**.cc",
        "source/circuit/**.cc",
        "source/global/**.cc",
        "source/hardware/**.cc",
        "source/parse/**.cc",
        "source/serde/**.cc"
    )

-- xmake project -k compile_commands

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--