
local vulkanSDK = os.getenv("VULKAN_SDK") or os.getenv("VK_SDK_PATH")

workspace "App"
    architecture "x64"
    configurations { "Debug", "Release" }

    defines { "GLEW_STATIC" }

    filter "system:windows"
        defines { "AE_WINDOWS" }

    filter "system:macosx"
        defines { "AE_MACOS" }

    filter "system:linux"
        defines { "AE_LINUX" }

    filter "configurations:Debug"
        defines { "AE_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "AE_RELEASE" }
        optimize "On"

    filter "action:vs*"
        startproject "Sandbox"

project "Log"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "../log-lib/src/**.cpp", "../log-lib/src/**.h", "../log-lib/include/**.h" }

    includedirs { "../log-lib/include", "../log-lib/src" }

    pchheader "general/pch.h"
    pchsource "../log-lib/src/general/pch.cpp" 

project "GLM"
    kind "Utility"
    language "C++"
    location "../vendor/glm"

    files { "../vendor/glm/**.hpp", "../vendor/glm/**.h" }

    includedirs { "../vendor/glm" }

project "App"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "../app-lib/src/**.cpp", "../app-lib/src/**.h", "../app-lib/include/**.h", "../app-lib/vendor/**.cpp", "../app-lib/vendor/**.h" }

    includedirs { "../dep/GLFW/include", "../dep/GLEW/include", "../dep/OpenALSoft/include", "../log-lib/include", "../app-lib/include", "../app-lib/src", "../app-lib/vendor", "../vendor/glm" }

    links { "../dep/GLFW/lib/glfw3.lib", "opengl32.lib", "../dep/GLEW/lib/glew32s.lib", "Log" }

    dependson { "GLM" }

    if vulkanSDK then
        defines { "AE_VULKAN" }
        includedirs { vulkanSDK .. "/Include" }
        links { vulkanSDK .. "/Lib/vulkan-1.lib" }
    end

    filter "configurations:Debug"
        links { "../dep/OpenALSoft/lib/Debug/OpenAL32" }

    filter "configurations:Release"
        links { "../dep/OpenALSoft/lib/Release/OpenAL32" }

    pchheader "general/pch.h"
    pchsource "../app-lib/src/general/pch.cpp" 

project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
  
    files { "../sandbox/src/**.cpp", "../sandbox/src/**.h" }
    includedirs { "../dep/GLFW/include", "../dep/GLEW/include", "../dep/OpenALSoft/include", "../log-lib/include", "../app-lib/include", "../sandbox/src", "../vendor/glm" }

    pchheader "general/pch.h"
    pchsource "../sandbox/src/general/pch.cpp" 

    links { "../dep/GLFW/lib/glfw3.lib", "opengl32.lib", "../dep/GLEW/lib/glew32s.lib", "Log", "App" }

    dependson { "GLM" } 

    if vulkanSDK then
        defines { "AE_VULKAN" }
        includedirs { vulkanSDK .. "/Include" }
        links { vulkanSDK .. "/Lib/vulkan-1.lib" }
    end

    filter "configurations:Debug"
        links { "../dep/OpenALSoft/lib/Debug/OpenAL32" }

        postbuildcommands {
            '{COPY} ../dep/OpenALSoft/lib/Debug/OpenAL32.dll "%{cfg.targetdir}"'
        }

    filter "configurations:Release"
        links { "../dep/OpenALSoft/lib/Release/OpenAL32" }

        postbuildcommands {
            '{COPY} ../dep/OpenALSoft/lib/Release/OpenAL32.dll "%{cfg.targetdir}"'
        }
