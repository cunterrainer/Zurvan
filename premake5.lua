workspace "Zurvan"
    configurations {
        "Debug",
        "Release"
    }
    startproject "Zurvan"

outputdir = "/BIN/%{cfg.toolset}/%{cfg.shortname}/%{prj.name}/"
cwd = os.getcwd() -- get current working directory
targetdir(cwd .. outputdir .. "bin")
objdir(cwd .. outputdir .. "bin-int")

RaylibDir = cwd .. "/Dependencies/raylib"


filter "system:windows"
    platforms { "x64", "x86" }
filter "system:linux"
    platforms "x64"
filter "system:macosx"
    platforms "universal"

filter { "configurations:Debug" }
    runtime "Debug"
    symbols "on"
    optimize "off"
filter { "configurations:Release" }
    runtime "Release"
    symbols "off"
    defines "NDEBUG"
    optimize "Speed"
    flags "LinkTimeOptimization"
filter {}

-- only for visual studio
flags {
    "MultiProcessorCompile"
}
staticruntime "on"
removeunreferencedcodedata "on"

include "Zurvan"
include "Dependencies/raylib"
include "Dependencies/raygui"