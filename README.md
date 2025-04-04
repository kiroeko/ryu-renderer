# Ryu-Renderer

An OpenGL renderer for Windows 64 bit platform.

The goal of this project is to use OpenGl to implement most of the important features of modern renderers, including PBR / batching / GPU instancing / OIT / bloom / deferred shading and SSAO. However, due to the complexity of implementation, ray tracing / global illumination / mesh shaders and more GPU driven things are not included in the plan.

## Tech Stack
- C++ 20
- [OpenGL 4.6](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf)
- [GLAD 2](https://gen.glad.sh/): Used to load OpenGL functions.
- [GLFW 3.4](https://www.glfw.org/): Used to manage OpenGL context.
- [STB](https://github.com/nothings/stb): Used to handle image.
- [GLM 1.0.1](https://github.com/g-truc/glm): Used to support mathematical geometric operations.
- [Boost 1.87.0](https://www.boost.org/): Used for basic extensions to C++.

## Environment Preparation

### 1. Basic Check
The basic environment should meet the following requirements:
- Graphics card supports OpenGL 4.6
- Windows 64-bit system

### 2. Install Software
- [Powershell](https://learn.microsoft.com/zh-cn/powershell/scripting/install/installing-powershell-on-windows)
- [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/vs/)
- [CMake 3.26+](https://cmake.org/download/)
- [Boost prebuilt windows binaries for msvc143](https://sourceforge.net/projects/boost/files/boost-binaries/)

Please note that all other third-party C++ libraries required by the project are already embedded in this repository, so there is no need to download or install them separately.

### 3. Config Software

#### 3.1 Config Boost
Add the environment variable **BOOST_ROOT** in Windows, with its value set to the installation directory of Boost on your local machine.

### 4. Finished!
Now you should have completed all the environment preparations.

## Run and build
The tool folder under the project directory provides various Powershell scripts that may be used in development:

### Run
You can execute the following powershell command to generate and open a Visual Studio solution for this project:
```powershell
<path-to-project>\tools\build\cmake_open_project_vs2022.ps1
```
You can then run the project in Visual Studio after it opens.

### Build
In addition to using the .sln generated above, you can also build the release version of this project with the following powershell command:
```powershell
<path-to-project>\tools\build\cmake_build_project_vs2022.ps1
```