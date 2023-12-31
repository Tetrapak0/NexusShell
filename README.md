# NexusShell
<p align="center">
  <img src="https://github.com/Tetrapak0/NexusShell/blob/main/icon.png?raw=true" alt="Icon"/>
</p>

## Index:
- [Help Wanted](#help-wanted)
- [Acquisiton](#acquisition)
- [Build](#build)
    - [Server](#server)
    - [Client](#client)
        - [Clone](#1-clone-repository)
        - [Install Cmake](#2-install-cmake)
        - [Configure CMake](#3-configure-cmake)
            - [Cross-Compiling](#32-cross-compiling-for-arm)
            - [For current platform](#33-for-current-platform)
## Help Wanted:
#### Any contributions implementing following features will most likely be approved
### Top Priority:
- Code stability, reliability, efficiency, rigidity and robustness
- include SDL for aarch64
### High Priority:
- Image Buttons -- Get file and website thumbnails, cache them, send them to client. This will be even more important once the remote program volume control feature is implemented.
### Low Priority:
- Code cleanup
- Undo for item clearing
- Numeric sort for JSON elements (profiles, pages, buttons, etc.)
## Acquisition:
- [Releases](https://github.com/Tetrapak0/NexusShell/releases) (Executable is staticaly linked and portable)
- [Build](#build)
## Build:
### **If you wish to compile for aarch64, you must build your own SDL3 library for it with an aarch64 OpenGL3 library. Neither are provided, as I have no access to OpenGL3 for aarch64.**
### **You must also obtain the aarch64 version of the freetype library yourself.**
### Server:
As of now, the server only supports Windows and can only be build using Visual Studio. CMake and Linux support are coming.
### Client:
The client only supports Linux.
#### 1. Clone repository
##### 1.1 Install git if not installed:
- Arch & based: `sudo pacman -Syu git`
- Debian & based: `sudo apt update && sudo apt install git`
- Fedora: `sudo dnf upgrade --refresh && sudo dnf install git`
##### 1.2 Clone
`git clone https://github.com/Tetrapak0/NexusShell`
#### 2. Install FreeType
- Arch & based: `sudo pacman -Syu freetype2`
- Debian & based: `sudo apt update && sudo apt install libfreetype-dev`
- Fedora: `sudo dnf upgrade --refresh && sudo dnf install freetype-dev`
#### 3. Install CMake
- Arch & based: `sudo pacman -Syu cmake`
- Debian & based: `sudo apt update && sudo apt install cmake`
- Fedora: `sudo dnf upgrade --refresh && sudo dnf install cmake`
#### 4. Configure CMake
##### 4.1 Build directory
```console
cd NexusShell/Client
mkdir build
```
#
##### 4.2 Cross-compiling for arm?
###### 4.2.1 Install gcc for aarch64
- Arch & based: `sudo pacman -Syu aarch64-linux-gnu-gcc`
- Debian & based: `sudo apt update && sudo apt install gcc-aarch64-linux-gnu`
- Fedora: `gcc-aarch64-linux-gnu`
###### 4.2.2 Configure CMake for cross-compilation
```console
cmake .. -DCMAKE_BUILD_TYPE=Release -DCROSS_COMPILE
```
#
#### 4.3 For current platform:
```console
cmake .. -DCMAKE_BUILD_TYPE=Release
```
#### 5. Build
```console
cmake --build . --config Release
```
#### 6. Run
```console
./NexusShell
```
