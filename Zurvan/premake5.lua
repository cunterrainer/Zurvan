project "Zurvan"
    language "C++"
    cppdialect "C++17"
    floatingpoint "Strict" -- Needed to run the simulation better

    -- gcc* clang* msc*
    filter "toolset:msc*"
        warnings "High" -- High
        externalwarnings "off" -- Default
        buildoptions { "/sdl" }
        disablewarnings "4244" -- float to int without cast

    filter { "toolset:gcc* or toolset:clang*" }
        enablewarnings {
            "cast-align",
            "cast-qual",
            "disabled-optimization",
            "format=2",
            "init-self",
            "missing-declarations",
            "missing-include-dirs",
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
            "stack-protector",
            "vla",
            "shift-overflow"
        }
        disablewarnings { "unused-parameter", "conversion", "missing-field-initializers", "unknown-warning-option" }

    filter { "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:gcc*" }
        buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
        linkoptions { "-Wl,--gc-sections" } -- remove unused sections (code)

    filter { "system:linux or system:macosx", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:clang*" }
        buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
        linkoptions { "-Wl,--gc-sections" } -- remove unused sections (code)

    filter { "system:windows", "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:clang*" }
        buildoptions { "-ffunction-sections", "-fdata-sections" } -- places each function and data item in its own section
        linkoptions { "-fuse-ld=lld", "-Wl,/OPT:REF,/OPT:ICF" } -- remove unused sections (code)

    filter { "configurations:Release or configurations:Distribution or configurations:MinSizeDistribution", "toolset:msc*" }
        linkoptions { "/OPT:REF", "/OPT:ICF" } -- remove unused sections (code)

    filter "toolset:gcc*"
        warnings "Extra"
        externalwarnings "Off"
        linkgroups "on" -- activate position independent linking
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

    filter "toolset:clang*"
        warnings "Extra"
        externalwarnings "Everything"
        enablewarnings {
            "array-bounds",
            "long-long",
            "implicit-fallthrough", 
        }
    filter {}

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


    filter { "configurations:Debug" }
        kind "ConsoleApp"

    filter { "configurations:Release" }
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
        flags "FatalWarnings"

    filter { "configurations:Distribution" }
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
        flags "FatalWarnings"

    filter { "configurations:MinSizeDistribution" }
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
        flags "FatalWarnings"
    filter {}
