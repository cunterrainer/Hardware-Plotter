project "ImPlot"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    files {
        "src/**.cpp",
        "include/ImPlot/**.h"
    }

    includedirs {
        "include/ImPlot",
        "../ImGui/include",
        "../ImGui/include/ImGui"
    }
