-- Reusable App project definition
-- Can be included by parent projects via: include("path/to/app-lib/app-project.lua")
-- This will also include the Log project dependency and vendor libraries

local app_lib_dir = path.getdirectory(_SCRIPT)
local app_lib_src = app_lib_dir .. "/app-lib"
local log_lib_dir = app_lib_dir .. "/dep/log-lib"
local vendor_dir = app_lib_dir .. "/vendor"
local dep_dir = app_lib_dir .. "/dep"

local vulkanSDK = os.getenv("VULKAN_SDK") or os.getenv("VK_SDK_PATH")

include(log_lib_dir .. "/log-project.lua")

project("STB")
kind("StaticLib")
language("C")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	vendor_dir .. "/stb/**.c",
	vendor_dir .. "/stb/**.h",
})

includedirs({
	vendor_dir .. "/stb",
})

project("GLM")
kind("Utility")
language("C++")

files({
	vendor_dir .. "/glm/**.hpp",
	vendor_dir .. "/glm/**.h",
})

includedirs({
	vendor_dir .. "/glm",
})

project("ImGui")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	vendor_dir .. "/imgui/*.cpp",
	vendor_dir .. "/imgui/*.h",
	vendor_dir .. "/imgui/backends/imgui_impl_glfw.cpp",
	vendor_dir .. "/imgui/backends/imgui_impl_glfw.h",
	vendor_dir .. "/imgui/backends/imgui_impl_opengl3.cpp",
	vendor_dir .. "/imgui/backends/imgui_impl_opengl3.h",
	vendor_dir .. "/imgui/backends/imgui_impl_opengl3_loader.h",
})

includedirs({
	vendor_dir,
	vendor_dir .. "/imgui",
})

filter("system:windows")
includedirs({ dep_dir .. "/GLFW/include" })
filter({})

if vulkanSDK then
	files({
		vendor_dir .. "/imgui/backends/imgui_impl_vulkan.cpp",
		vendor_dir .. "/imgui/backends/imgui_impl_vulkan.h",
	})
	includedirs({ vulkanSDK .. "/Include" })
end

project("App")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

defines({ "GLEW_STATIC" })

files({
	app_lib_src .. "/src/**.cpp",
	app_lib_src .. "/src/**.h",
	app_lib_src .. "/include/**.h",
})

includedirs({
	dep_dir .. "/GLEW/include",
	dep_dir .. "/OpenALSoft/include",
	log_lib_dir .. "/log-lib/include",
	app_lib_src .. "/include",
	app_lib_src .. "/src",
	vendor_dir .. "/glm",
	vendor_dir .. "/stb",
	vendor_dir .. "/imgui",
})

filter("system:windows")
includedirs({ dep_dir .. "/GLFW/include" })
filter({})

dependson({ "GLM" })

filter("system:windows")
links({
	dep_dir .. "/GLFW/lib/glfw3.lib",
	"opengl32.lib",
	dep_dir .. "/GLEW/lib/glew32s.lib",
	"Log",
	"STB",
	"ImGui",
})

filter({ "system:windows", "configurations:Debug" })
links({ dep_dir .. "/OpenALSoft/lib/Debug/OpenAL32" })

filter({ "system:windows", "configurations:Release or configurations:Dist" })
links({ dep_dir .. "/OpenALSoft/lib/Release/OpenAL32" })

filter("system:linux")
links({
	"glfw",
	"GL",
	"GLEW",
	"openal",
	"Log",
	"STB",
	"ImGui",
})

filter("system:macosx")
links({
	"glfw",
	"OpenGL.framework",
	"GLEW",
	"openal",
	"Log",
	"STB",
	"ImGui",
})

filter({})

if vulkanSDK then
	defines({ "AE_VULKAN" })
	includedirs({ vulkanSDK .. "/Include" })

	filter("system:windows")
	links({ vulkanSDK .. "/Lib/vulkan-1.lib" })

	filter("system:linux or system:macosx")
	links({ "vulkan" })

	filter({})
end

pchheader(path.getabsolute(app_lib_src .. "/src/general/pch.h"))
pchsource(app_lib_src .. "/src/general/pch.cpp")
