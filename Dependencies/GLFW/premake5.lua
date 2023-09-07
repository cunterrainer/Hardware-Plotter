project "glfw"
    kind "StaticLib"
    language "C"

    files {
        "src/internal.h",
        "src/platform.h",
        "src/mappings.h",
        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/platform.c",
        "src/vulkan.c",
        "src/window.c",
        "src/egl_context.c",
        "src/osmesa_context.c",
        "src/null_platform.h",
        "src/null_joystick.h",
        "src/null_init.c",
        "src/null_monitor.c",
        "src/null_window.c", 
        "src/null_joystick.c"
    }

    includedirs {
        "include"
    }

    filter "system:windows"
        defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }
        files {
            "src/win32_time.h",
            "src/win32_thread.h",
            "src/win32_module.c",
            "src/win32_time.c",
            "src/win32_thread.c",

            "src/win32_platform.h",
            "src/win32_joystick.h",
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_monitor.c",
            "src/win32_window.c",
            "src/wgl_context.c"
        }


    function GetOS()
	    -- ask LuaJIT first
	    if jit then
		    return jit.os
	    end

	    -- Unix, Linux variants
	    local fh,err = assert(io.popen("uname -o 2>/dev/null","r"))
	    if fh then
		    osname = fh:read()
	    end

	    return osname or "Windows"
    end

    function wayland_generate (protocol_file, output_file)
        local cmd1 = "wayland-scanner client-header " .. protocol_file .. " " .. output_file .. ".h"
        local cmd2 = "wayland-scanner private-code "  .. protocol_file .. " ".. output_file .. "-code.h"
        prebuildcommands(cmd1)
        prebuildcommands(cmd2)
        files { output_file .. ".h", output_file .. "-code.h" }
    end

    function pkg_get_variable (moduleName, varName)
        local handle = io.popen("pkg-config " .. moduleName .. " --variable=" .. varName)
        local result = handle:read("*a")
        handle:close()
        return result:gsub('[\n\r]', '')
    end

    filter { "system:linux", "options:wayland" }
        defines "_GLFW_WAYLAND"
        files {
            "src/posix_time.h",
            "src/posix_thread.h",
            "src/posix_module.c",
            "src/posix_time.c",
            "src/posix_thread.c",
            "src/linux_joystick.c",
            "src/linux_joystick.h",
            "src/posix_poll.h",
            "src/posix_poll.c",

            "src/wl_platform.h",
            "src/xkb_unicode.h",
            "src/xkb_unicode.c",
            "src/wl_init.c",
            "src/wl_monitor.c",
            "src/wl_window.c"
        }

        if GetOS():lower():match("linux") and _OPTIONS["wayland"] then
            local wayland_protocols_base = pkg_get_variable("wayland-protocols", "pkgdatadir"):sub(2)
            local wayland_client_pkgdatadir = pkg_get_variable("wayland-client", "pkgdatadir")

            wayland_generate(wayland_client_pkgdatadir .. "/wayland.xml", "src/wayland-client-protocol")
            wayland_generate(wayland_protocols_base .. "/stable/xdg-shell/xdg-shell.xml", "src/wayland-xdg-shell-client-protocol")
            wayland_generate(wayland_protocols_base .. "/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml", "src/wayland-xdg-decoration-client-protocol")
            wayland_generate(wayland_protocols_base .. "/stable/viewporter/viewporter.xml", "src/wayland-viewporter-client-protocol")
            wayland_generate(wayland_protocols_base .. "/unstable/relative-pointer/relative-pointer-unstable-v1.xml", "src/wayland-relative-pointer-unstable-v1-client-protocol")
            wayland_generate(wayland_protocols_base .. "/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml", "src/wayland-pointer-constraints-unstable-v1-client-protocol")
            wayland_generate(wayland_protocols_base .. "/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml", "src/wayland-idle-inhibit-unstable-v1-client-protocol")
        end


    filter { "system:linux", "not options:wayland" }
        defines "_GLFW_X11"
        files {
            "src/posix_time.h",
            "src/posix_thread.h",
            "src/posix_module.c",
            "src/posix_time.c",
            "src/posix_thread.c",

            "src/x11_platform.h",
            "src/xkb_unicode.h",
            "src/x11_init.c",
            "src/x11_monitor.c",
            "src/x11_window.c",
            "src/xkb_unicode.c",
            "src/glx_context.c",
            "src/linux_joystick.c",
            "src/linux_joystick.h",
            "src/posix_poll.h",
            "src/posix_poll.c"
        }

   filter "system:macosx"
        defines "_GLFW_COCOA"
        files {
            "src/cocoa_time.h",
            "src/cocoa_time.c",
            "src/posix_thread.h",
            "src/posix_module.c",
            "src/posix_thread.c",

            "src/cocoa_platform.h",
            "src/cocoa_joystick.h",
            "src/cocoa_init.m",
            "src/cocoa_joystick.m",
            "src/cocoa_monitor.m",
            "src/cocoa_window.m",
            "src/nsgl_context.m"
        }
   filter {}
