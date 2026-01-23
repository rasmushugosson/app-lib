-- Reusable App project definition
-- Can be included by parent projects via: include("path/to/app-lib/app-project.lua")
-- This will also include the Log project dependency and vendor libraries

local app_lib_dir = path.getdirectory(_SCRIPT)
local app_lib_src = app_lib_dir .. "/app-lib"
local log_lib_dir = app_lib_dir .. "/dep/log-lib"
local event_lib_dir = app_lib_dir .. "/dep/event-lib"
local vendor_dir = app_lib_dir .. "/vendor"
local dep_dir = app_lib_dir .. "/dep"

local vulkanSDK = os.getenv("VULKAN_SDK") or os.getenv("VK_SDK_PATH")

-- Detect Vulkan availability
-- On Windows: requires VULKAN_SDK or VK_SDK_PATH environment variable
-- On Linux/macOS: can also use system-installed Vulkan
local function detect_vulkan()
	if vulkanSDK then
		return true, vulkanSDK
	end
	-- Check for system Vulkan on Linux/macOS
	if os.target() == "linux" or os.target() == "macosx" then
		if os.isfile("/usr/include/vulkan/vulkan.h") then
			return true, nil -- Available but no SDK path
		end
	end
	return false, nil
end

local vulkanAvailable, vulkanSDKPath = detect_vulkan()
AE_VULKAN_AVAILABLE = vulkanAvailable -- Export as global for parent projects

if vulkanAvailable then
	if vulkanSDKPath then
		print(">> Vulkan SDK detected: " .. vulkanSDKPath)
	else
		print(">> System Vulkan detected")
	end
end

include(log_lib_dir .. "/log-project.lua")
include(event_lib_dir .. "/event-project.lua")

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

project("Nlohmann")
kind("Utility")
language("C++")

files({
	vendor_dir .. "/nlohmann/**.hpp",
})

includedirs({
	vendor_dir .. "/nlohmann",
})

project("GLAD")
kind("StaticLib")
language("C")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	vendor_dir .. "/glad/src/glad.c",
	vendor_dir .. "/glad/include/**.h",
})

includedirs({
	vendor_dir .. "/glad/include",
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

if vulkanAvailable then
	files({
		vendor_dir .. "/imgui/backends/imgui_impl_vulkan.cpp",
		vendor_dir .. "/imgui/backends/imgui_impl_vulkan.h",
	})
	if vulkanSDKPath then
		includedirs({ vulkanSDKPath .. "/Include" })
	end
end

project("App")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	app_lib_src .. "/src/**.cpp",
	app_lib_src .. "/src/**.h",
	app_lib_src .. "/include/**.h",
})

includedirs({
	vendor_dir .. "/glad/include",
	dep_dir .. "/OpenALSoft/include",
	log_lib_dir .. "/log-lib/include",
	event_lib_dir .. "/event-lib/include",
	app_lib_src .. "/include",
	app_lib_src .. "/src",
	vendor_dir .. "/glm",
	vendor_dir .. "/stb",
	vendor_dir .. "/imgui",
	vendor_dir .. "/nlohmann",
})

filter("system:windows")
includedirs({ dep_dir .. "/GLFW/include" })
filter({})

dependson({ "GLM", "Nlohmann" })

filter("system:windows")
links({
	dep_dir .. "/GLFW/lib/glfw3.lib",
	"opengl32.lib",
	"Log",
	"Event",
	"STB",
	"ImGui",
	"GLAD",
})

filter({ "system:windows", "configurations:Debug" })
links({ dep_dir .. "/OpenALSoft/lib/Debug/OpenAL32" })

filter({ "system:windows", "configurations:Release or configurations:Dist" })
links({ dep_dir .. "/OpenALSoft/lib/Release/OpenAL32" })

filter("system:linux")
links({
	"glfw",
	"GL",
	"openal",
	"Log",
	"Event",
	"STB",
	"ImGui",
	"GLAD",
})

filter("system:macosx")
links({
	"glfw",
	"OpenGL.framework",
	"openal",
	"Log",
	"Event",
	"STB",
	"ImGui",
	"GLAD",
})

filter({})

if vulkanAvailable then
	defines({ "AE_VULKAN" })
	if vulkanSDKPath then
		includedirs({ vulkanSDKPath .. "/Include" })
		filter("system:windows")
		links({ vulkanSDKPath .. "/Lib/vulkan-1.lib" })
		filter({})
	end

	filter("system:linux or system:macosx")
	links({ "vulkan" })

	filter({})
end

pchheader(path.getabsolute(app_lib_src .. "/src/general/pch.h"))
pchsource(app_lib_src .. "/src/general/pch.cpp")
