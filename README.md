# App Lib

## General

This library combines nine well-known APIs and libraries with additional wrapper and utility functionality. The aim of this library is cover the basic boilerplate code required to use them together for graphics applications. The APIs and libraries are:
[OpenGL](https://www.opengl.org/), [GLFW](https://github.com/glfw/glfw), [GLEW](https://github.com/nigels-com/glew),
[Vulkan](https://www.vulkan.org/), [STB_Image](https://github.com/nothings/stb/blob/master/stb_image.h),
[STB_Image_Write](https://github.com/nothings/stb/blob/master/stb_image_write.h),
[STB_Vorbis](https://github.com/nothings/stb/blob/master/stb_vorbis.c),
[Dear ImGui](https://github.com/ocornut/imgui), and [OpenAL Soft](https://github.com/kcat/openal-soft).

For details on how these components are used and which ones are required, see the
[Third-party APIs/libraries](#third-party-apislibraries) section. The library is built using
[Premake5](https://premake.github.io/) for `C++20`.

In addition to the third-party components mentioned, this library is also built on my own utility logging library, see the original repository [here](https://github.com/rasmushugosson/log-lib) for more information.

## Getting Started

1. **Clone the repository** and open a terminal in the project root.
2. Navigate into the `dev` folder and **Run the provided `Premake5` script**.
   - If you’re on **Windows** with **Visual Studio 2022**, use the included `.bat` file to generate a `.sln` solution.
   - Otherwise, run `premake5` with the appropriate arguments to generate project files for your platform and IDE of choice.
3. **Open the generated solution/project** in your IDE and build the `Sandbox` project. The resulting binaries will appear in the `bin` folder.
4. **Run the `Sandbox` project** to verify that the library and dependencies are set up correctly.

### Project Integration

If you want to integrate this library into your own project, the easiest way is to replace the `sandbox` folder with your own project files. The `premake5.lua` script in the `dev` folder can then be modified to include your source files and libraries. Ensure all preprocessor definitions, include directories, and library dependencies are set up correctly as specified in the `premake5.lua` script.

### Additional Dependencies

- **Premake5:** This library uses [Premake5](https://premake.github.io/) as its build configuration tool.  
  Ensure that `premake5` is installed on your system or copied into the `dev` folder.  
  You can download it [here](https://premake.github.io/download/).

- **Graphics Drivers / SDKs:**  
  - **OpenGL:** The library assumes OpenGL drivers are installed and available by default.  
  - **Vulkan:** If you want to enable Vulkan support, install the [Vulkan SDK](https://www.vulkan.org/tools#download-these-essential-development-tools) 
    and add it to your system `PATH`. If the Vulkan SDK is not found, only the OpenGL portion will be usable.

## Usage

There are two main parts to this library: the `Window` class in the `Window.h` header file and the other utility classes and functions in the other header files.

### Window Class

The `Window` class is defined in the `Window.h` header file and is located in the `app-lib/include` folder. This class is a wrapper around [GLFW](https://github.com/glfw/glfw), [GLEW](https://github.com/nigels-com/glew) and [Dear ImGui](https://github.com/ocornut/imgui). It provides a simple way of creating a window, handling input events, adding interface components and setting up a rendering context.

To specify the properties of the created window, the `WindowDesc` struct is passed to the `Window` constructor. Although, these properties can also be set individually after creation using class methods, except the rendering API to be used since that requires complete recreation. The `WindowDesc` struct has the following members:

```cpp
enum class GraphicsAPI
{
  OPENGL = 0,
  VULKAN
};

struct WindowDesc
{
  std::string title; // The title of the window
  uint32_t width; // The width of the window in pixels
  uint32_t height; // The height of the window in pixels
  bool resizable; // Whether the window is resizable
  bool minimizable; // Whether the window can be minimized
  bool minimized; // Whether the window is minimized
  bool maximizable; // Whether the window can be maximized
  bool maximized; // Whether the window is maximized
  bool fullscreen; // Whether the window is fullscreen (overrides width and height if true)
  uint8_t monitor; // The monitor on which the window should be displayed
  bool vsync; // Whether vsync is enabled
  uint32_t fps; // The target frames per second if vsync is disabled
  GraphicsAPI graphicsAPI; // The graphics API to create a context for
};
```

### Additional Header Files

The other header files in the `app-lib/include` folder contain utility classes and functions that can be used to create graphics applications. `Files.h` is mostly a wrapper around [STB](https://github.com/nothings/stb) containing classes and methods for reading and writing image and audio files. The `OpenGL.h`, `Vulkan.h` and `OpenAL.h` header files are all similar in structure, with each containing a macro that checks and throws custom exceptions for the respective API calls, in addition to forward including the respective API headers. Lastly, the `ImGui.h` header is the original [Dear ImGui](https://github.com/ocornut/imgui) header file with some modifications to work with project structure.

### Code Example

Below is an example program demonstrating how this library can be used to create a simple graphics application.

```cpp
// Sandbox.cpp
// Some parts of the code are omitted for brevity. See the full example in the sandbox folder

// Author: Rasmus Hugosson
// Date: 2025-03-17

#include "Window.h"
#include "ImGui.h"

static void OnInterfaceUpdate()
{
  // This function is called every frame as the ImGui interface is updated
}

int main()
{
  // We wrap the code in a try-catch block to catch any exceptions that might be thrown
  try
  {
    // Description of the window
    ae::WindowDesc windowDesc;
    windowDesc.title = "Sandbox";
    windowDesc.width = 1280;
    windowDesc.height = 720;
    windowDesc.resizable = true;
    windowDesc.minimizable = true;
    windowDesc.minimized = false;
    windowDesc.maximizable = true;
    windowDesc.maximized = false;
    windowDesc.monitor = 0;
    windowDesc.fullscreen = false;
    windowDesc.vsync = true;
    windowDesc.fps = 165; // Not actually used since vsync is enabled
    windowDesc.graphicsAPI = ae::GraphicsAPI::OPENGL;

    // We can now create a window with the descriptor
    ae::Window window(windowDesc);
    window.Create();

    // Set the static pointer to the window
    s_pWindow = &window;

    // Icons can also be loaded by specifying image paths of different sizes
    ae::IconSet icons({
      "res/icons/default16.png",
      "res/icons/default24.png",
      "res/icons/default32.png",
      "res/icons/default48.png",
      "res/icons/default64.png"
      });

    // The icon set can then be set for the window
    window.SetIconSet(icons);

    // A cursor can be created from an image path and optionally a hot spot
    ae::Cursor cursor("res/cursors/arrow_white.png", 11, 6);
    window.SetCursor(cursor);

    // The function from before must also be specified to be called every frame
    window.SetOnInterfaceUpdateCB(OnInterfaceUpdate);

    // Update loop
    while (!window.ShouldClose())
    {
      // Input events can be checked at any time
      if (window.GetKeyboard().IsKeyPressed(ae::Key::ESCAPE))
      {
        window.Close();
      }

      // We can then update the window each frame
      window.SetActive();
      window.Clear();
      window.Update();
    }

    s_pWindow = nullptr;

    // Clean everything up by destroying the window before termination
    window.ResetCursor();
    window.ResetIconSet();
    window.Destroy();
  }

  // If any exceptions are thrown, we catch them here	
  catch (const std::exception& e)
  {
    // We can log the exception to the console
    AE_LOG_CONSOLE(AE_ERROR, e.what());
  }
}
```

The example program found in the `sandbox` folder demonstrates how to use the library to create a simple graphics application. Below is a screenshot of the program running.

![Sandbox](docs/example.png) 

## Third-party APIs/libraries

All third-party libraries included as source code have been modified to work with the project structure. For example including the precompiled header and altering include paths. Although, no changes have been made to the functionality of the libraries.

### OpenGL

*(Uses the system’s OpenGL drivers. Not bundled in this repository.)*

### GLFW

GLFW is used to create application windows and handle input events.  
It is included as a **precompiled static library** in the `lib` folder.  
If you wish to build it yourself or replace the binaries, see the [official repo](https://github.com/glfw/glfw).

### GLEW

GLEW is used for loading OpenGL functions.  
It is included as a **precompiled static library** in the `lib` folder.  
If you wish to build it yourself or replace the binaries, see the [official repo](https://github.com/nigels-com/glew).

### Vulkan

*(Not strictly required, but supported if installed. Assumes Vulkan SDK is installed and added to PATH variables. Not bundled in this repository.)* 

### STB_Image

STB_Image is used for loading various image formats.  
It is included as source code in `app-lib/vendor`.  
See the [official repo](https://github.com/nothings/stb/blob/master/stb_image.h) for details.

### STB_Image_Write

STB_Image_Write is used for saving images to disk.  
It is included as source code in `app-lib/vendor`.  
See the [official repo](https://github.com/nothings/stb/blob/master/stb_image_write.h).

### STB_Vorbis

STB_Vorbis is used for loading Ogg Vorbis audio files.  
It is included as source code in `app-lib/vendor`.  
See the [official repo](https://github.com/nothings/stb/blob/master/stb_vorbis.c).

### Dear ImGui

Dear ImGui is used to create an immediate-mode graphical user interface.  
It is included as source code in `app-lib/vendor` and `app-lib/include`.  
See the [official repo](https://github.com/ocornut/imgui) for more information.

### OpenAL Soft

OpenAL Soft is used to play audio and is licensed under the **GNU Library General Public License (LGPL) v2.1**.  
In this repository, it is distributed as a **separate DLL** (plus a small import `.lib` on Windows) in the `lib` folder.  
Under the LGPL, end users are permitted to **replace** the OpenAL Soft DLL with a custom build if they wish.

For more information, see:

- The **LGPL v2.1** text (`COPYING` from OpenAL Soft) is located in the `licenses/` folder.
- The **PFFFT** license (used within OpenAL Soft) is also included in `licenses/`.
- The official source code is available at [https://github.com/kcat/openal-soft](https://github.com/kcat/openal-soft).

Please ensure you **comply with the LGPL** requirements if you distribute this library, particularly allowing users to swap in their own OpenAL Soft build.

### Third-party licenses

If you plan to use this library in your own projects, you must comply with the original licenses of:

- [GLFW](https://github.com/glfw) (**Zlib**),
- [GLEW](https://github.com/nigels-com) (**Modified BSD** / **MIT**),
- [STB libraries](https://github.com/nothings) (**Public Domain / MIT**),
- [Dear ImGui](https://github.com/ocornut) (**MIT**),
- [OpenAL Soft](https://github.com/kcat) (**LGPL v2.1**).

All license texts can be found in the `licenses` folder, and proper credit must be given to the respective authors.

## Included Assets

The `res` folder contains assets used in the example program. The `icons` folder contains images of different sizes used as window icons. The `cursors` folder contains images used as custom cursors. The `fonts` folder contains a font used by Dear ImGui.

All assets are created by me, except for the font which is from the [Google Fonts](https://fonts.google.com/) website. The font is licensed under the `Open Font License` which is included in the same folder as the font itself (`res/fonts`).

## License

This library (excluding mentioned third-party components) is licensed under the **Apache License 2.0**.  
See the [LICENSE](LICENSE) file in this repository for details.
