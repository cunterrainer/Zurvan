project "raylib"
    language "C"
    kind "StaticLib"
    warnings "off"
    externalwarnings "off"

    files {
        "src/**.h",
        "src/rcore.c",
        "src/rmodels.c",
        "src/rshapes.c",
        "src/rtext.c",
        "src/rtextures.c",
        "src/utils.c",
        "src/raudio.c"
    }

    includedirs {
        "src/external/glfw/include"
    }

    filter "system:not emscripten"
        defines "PLATFORM_DESKTOP_GLFW" -- PLATFORM_DESKTOP_RGFW
        files "src/rglfw.c"

    filter "system:emscripten"
        defines { "PLATFORM_WEB", "GRAPHICS_API_OPENGL_ES2" }

    filter { "system:macosx", "files:src/rglfw.c" }
        compileas "Objective-C"

    filter "toolset:gcc* or toolset:clang*"
        disablewarnings "implicit-function-declaration"

    filter "system:windows"
        defines "_CRT_SECURE_NO_WARNINGS"
    filter "system:macosx"
        defines "GL_SILENCE_DEPRECATION"