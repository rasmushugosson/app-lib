# CLAUDE.md

This file contains guidance for Claude Code when working with this repository.

## Project Overview

App-lib is a C++23 graphics application library that wraps OpenGL, GLFW, GLEW, Vulkan, Dear ImGui, OpenAL Soft, STB, and GLM. It depends on log-lib (included as a git submodule).

## Build System

Uses Premake5 for build configuration.

### Quick Build (Linux)
```bash
premake5 gmake-clang   # or gmake-gcc
cd build/linux-clang
make config=debug -j$(nproc)
# Run from project root:
./bin/Sandbox/Debug/Sandbox
```

### Key Premake Files
- `premake5.lua` - Main workspace, Sandbox project, custom actions
- `app-project.lua` - Reusable App project definition (for submodule usage)

### Custom Premake Actions
- `premake5 gmake-gcc` - Generate GCC makefiles
- `premake5 gmake-clang` - Generate Clang makefiles
- `premake5 format` - Run clang-format
- `premake5 lint` / `premake5 lint-fix` - Run clang-tidy

### Build Configurations
- `debug` - Debug symbols, AE_DEBUG defined, logging active
- `release` - Optimized, AE_RELEASE defined, release logging only
- `dist` - Distribution build, AE_DIST defined, no logging

## Project Structure

```
app-lib/
├── app-lib/           # Main library source
│   ├── include/       # Public headers (Window.h, Files.h, DearImGui.h, OpenGLMaths.h, etc.)
│   └── src/           # Implementation
├── sandbox/           # Example application
│   └── src/
├── dep/               # Dependencies
│   ├── log-lib/       # Git submodule
│   ├── GLFW/          # Precompiled (Windows), system lib (Linux)
│   ├── GLEW/          # Precompiled (Windows), system lib (Linux)
│   └── OpenALSoft/    # DLL + import lib
└── vendor/            # Source dependencies
    ├── stb/           # Image/audio loading
    ├── imgui/         # Dear ImGui
    └── glm/           # Math library
```

## Platform-Specific Notes

### GLFW Headers
- **Windows**: Uses bundled headers from `dep/GLFW/include`
- **Linux**: Uses system GLFW headers (requires GLFW 3.4+ for full API support)

Install Linux dependencies:
```bash
# Arch
sudo pacman -S glfw glew openal

# Debian/Ubuntu
sudo apt install libglfw3-dev libglew-dev libopenal-dev
```

### Vulkan Support

Vulkan is optional. The build system automatically detects Vulkan availability:
- **Windows**: Requires [Vulkan SDK](https://vulkan.lunarg.com/). The installer sets `VULKAN_SDK` environment variable.
- **Linux**: Uses system-installed Vulkan. Install headers and validation layers:
  ```bash
  # Arch
  sudo pacman -S vulkan-headers vulkan-validation-layers

  # Debian/Ubuntu
  sudo apt install vulkan-headers vulkan-validationlayers
  ```

Note: `vulkan-validation-layers` is only required for debug builds. Release/dist builds work without it.

### Link Order
When linking, dependencies must come after dependents:
```lua
links({ "App", "ImGui", "STB", "Log", "glfw", "GL", "GLEW", "openal" })
```

## Key Patterns

### Path Resolution
`FormatDevPath()` in `Files.h` returns paths as-is. The Sandbox must be run from the project root directory so relative paths like `res/fonts/...` resolve correctly.

### Logging API (from log-lib)
```cpp
AE_LOG(AE_TRACE, "message with {} format", value);
AE_LOG_TRACE("shorthand for trace level");
AE_LOG_ERROR("error message");
```

### Window Creation
```cpp
ae::WindowDesc desc;
desc.title = "My App";
desc.width = 1280;
desc.height = 720;
desc.graphicsAPI = ae::GraphicsAPI::OPENGL;

ae::Window window(desc);
window.Create();
// ... render loop ...
window.Destroy();
```

## Submodule Usage

When using app-lib as a submodule in another project:
```lua
include("path/to/app-lib/app-project.lua")

project("YourProject")
    includedirs({
        "path/to/app-lib/dep/log-lib/log-lib/include",
        "path/to/app-lib/app-lib/include",
        "path/to/app-lib/vendor/glm",
        "path/to/app-lib/vendor/imgui"
    })
    links({ "App", "ImGui", "STB", "Log" })
```

## Common Issues

1. **Font loading fails**: Run executable from project root, not from bin/ directory
2. **Undefined GLFW symbols on Linux**: Ensure system GLFW is 3.4+ (`pkg-config --modversion glfw3`)
3. **Linker errors with ImGui**: Check link order - App must come before ImGui
4. **Header not found (DearImGui.h, OpenGLMaths.h)**: These are the correct names, not ImGui.h/Maths.h
5. **Vulkan window doesn't appear (Linux)**: Install `vulkan-headers` and `vulkan-validation-layers` (debug builds require validation layers)
6. **"Failed to find required validation layers"**: Install validation layers package, or build with `config=release`
