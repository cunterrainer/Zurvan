project "Zurvan"
    language "C++"
    cppdialect "C++17"
    floatingpoint "Default" -- Needed imrpove simulation accuracy

    files {
        "**.cpp",
        "**.h"
    }

    includedirs {
        RaylibDir .. "/src",
        "../Dependencies/raygui"
    }

    externalincludedirs {
        RaylibDir .. "/src",
        "../Dependencies/raygui",
        "../Dependencies/raygui/src"
    }

    links {
        "raylib",
        "raygui"
    }

    filter "system:windows"
        links {
            "Winmm",
            "opengl32",
            "gdi32",
            "shell32",
            "User32"
        }

    filter "system:linux"
        links {
            "GL",
            "X11",
            "rt",
            "dl",
            "m"
        }

    filter "system:macosx"
        linkoptions "-framework AppKit -framework iokit -framework OpenGl"
        disablewarnings { "sign-conversion" }

    filter "system:emscripten"
        linkoptions { "-sUSE_GLFW=3", "-sASYNCIFY", "-sMIN_WEBGL_VERSION=2", "-sMAX_WEBGL_VERSION=2", "-sALLOW_MEMORY_GROWTH=1", "-sUSE_PTHREADS=1" }

    filter "configurations:Debug"
        warnings "off"
        externalwarnings "off"

    -- gcc* clang* msc*
    filter { "toolset:msc*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        warnings "High" -- High
        externalwarnings "off" -- Default
        -- buildoptions { "/sdl" } -- only usefull for debug and maybe not even that, we don't want uninitialized pointers to be nullptrs
        -- disablewarnings "4244" -- float to int without cast

    filter { "toolset:gcc* or toolset:clang*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        enablewarnings {
            "cast-align",
            "cast-qual",
            "disabled-optimization",
            "format=2",
            "init-self",
            "missing-declarations",
            "missing-include-dirs",
            "missing-field-initializers",
            "unused-parameter",
            "redundant-decls",
            "shadow",
            "sign-conversion",
            "strict-overflow=5",
            "switch-default",
            "undef",
            "uninitialized",
            "unreachable-code",
            "unused",
            "alloca",
            "conversion",
            "deprecated",
            "format-security",
            "null-dereference",
            "deprecated-copy",
            "stack-protector",
            "vla",
            "shift-overflow"
        }
        disablewarnings { "unknown-warning-option" }

    filter { "toolset:gcc*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        warnings "Extra"
        externalwarnings "off"
        -- linkgroups "on" -- activate position independent linking
        enablewarnings {
            "array-bounds=2",
            "duplicated-branches",
            "duplicated-cond",
            "logical-op",
            "arith-conversion",
            "stringop-overflow=4",
            "implicit-fallthrough=3",
            "trampolines"
        }

    filter { "toolset:clang*", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        warnings "Extra"
        externalwarnings "Everything"
        enablewarnings {
            "array-bounds",
            "long-long",
            "implicit-fallthrough", 
        }

    filter { "system:emscripten", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution" }
        linkoptions "--memory-init-file 0"
        warnings "Extra"
        externalwarnings "Everything"

    filter { "configurations:Debug" }
        kind "ConsoleApp"

    filter { "configurations:Release" }
        kind "ConsoleApp"
        fatalwarnings { "All" }

    filter { "configurations:Distribution" }
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
        fatalwarnings { "All" }

    filter { "configurations:MinSizeDistribution" }
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
        fatalwarnings { "All" }
    filter {}