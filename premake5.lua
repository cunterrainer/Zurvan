workspace "Zurvan"
    configurations {
        "Debug",
        "Release",
        "Distribution",
        "MinSizeDistribution"
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
    symbols "on"
    optimize "Speed"
    defines "NDEBUG"
    flags "LinkTimeOptimization"

filter { "configurations:Distribution" }
    runtime "Release"
    symbols "off"
    optimize "Full"
    defines "NDEBUG"
    flags "LinkTimeOptimization"

filter { "configurations:MinSizeDistribution" }
    runtime "Release"
    symbols "off"
    optimize "Size"
    defines "NDEBUG"
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