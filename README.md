# ShortPad
### Why?
There may be other software alternatives to the Elgato Stream Deck and similar products, but none of them provide the same functionality as ShortPad will before exiting beta.
## Acquisition:
- [Releases](https://github.com/Tetrapak0/ShortPad/releases) (Executable is staticaly linked and portable)
- [Build](##build)
## Build:
### Server:
As of now, the server only supports Windows and can only be build using Visual Studio. CMake and Linux support is coming.
### Client:
The client only supports Linux.
#### 1. Clone repository
##### 1.1 Install git if not installed:
- Arch & based: `sudo pacman -Syu git`
- Debian & based: `sudo apt update && sudo apt install git`
- Fedora: `sudo dnf upgrade --refresh && sudo dnf install git`
##### 1.2 Clone
`git clone https://github.com/Tetrapak0/ShortPad`
#### 2. Install CMake
- Arch & based: `sudo pacman -Syu cmake`
- Debian & based: `sudo apt update && sudo apt install cmake`
- Fedora: `sudo dnf upgrade --refresh && sudo dnf install cmake`
#### 3. Configure CMake
##### 3.1 Build directory
```shell
cd ShortPad/Client
mkdir build
```
#
##### 3.2 Cross-compiling for arm?
###### 3.2.1 Install gcc for aarch64
- Arch & based: `sudo pacman -Syu aarch64-linux-gnu-gcc`
- Debian & based: `sudo apt update && sudo apt install gcc-aarch64-linux-gnu`
- Fedora: `gcc-aarch64-linux-gnu`
###### 3.2.2 Configure CMake for cross-compilation
```shell
cmake .. -DCMAKE_BUILD_TYPE=Release -DCROSS_COMPILE
```
#
#### 3.3 For current platform:
```shell
cmake .. -DCMAKE_BUILD_TYPE=Release
```
#### 4. Build
```shell
cmake --build . --config Release
```
#### 5. Run
```shell
./ShortPad
```
