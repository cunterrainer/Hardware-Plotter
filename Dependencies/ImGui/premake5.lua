project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    files {
        "src/**.cpp",
        "include/ImGui/**.h"
    }

    includedirs {
        "include/ImGui",
        "../GLFW/include"
    }
