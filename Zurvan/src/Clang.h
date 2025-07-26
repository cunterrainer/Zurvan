#pragma once

/*
    For some reason clang preferably links against main() if available even though the build
    settings would link against WinMain().
    However in order to link against WinMain() I created this macro.
    GCC & MSVC properly link against WinMain().
    Premake will define all these macros for you if you select clang as compiler.
*/
#if defined TOOLCHAIN_CLANG && defined CONFIG_DISTRIBUTION && defined SYSTEM_WINDOWS
    #define main() WinMain([[maybe_unused]]void* hInst, [[maybe_unused]]void* hInstPrev, [[maybe_unused]]char* cmdline, [[maybe_unused]]int cmdshow)
#endif