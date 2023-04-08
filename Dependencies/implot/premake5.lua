project "ImPlot"
    kind "StaticLib"
    language "C++"

    files {
        "src/**.cpp",
        "include/ImPlot/**.h"
    }

    includedirs {
        "include/ImPlot",
        "../ImGui/include",
        "../ImGui/include/ImGui"
    }