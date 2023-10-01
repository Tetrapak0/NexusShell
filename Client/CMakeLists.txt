cmake_minimum_required(VERSION 3.26)
project(ShortPad)

set(CMAKE_C_STANDARD    17)
set(CMAKE_CXX_STANDARD  17)
set(CMAKE_C_STANDARD_REQUIRED   True)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES x86_64 AND DEFINED CROSS_COMPILE)
    set(CMAKE_C_COMPILER aarch64-linux-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-g++)
endif()

file(GLOB ShPSource ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB ImSource ${CMAKE_CURRENT_SOURCE_DIR}//../imgui/*.cpp)

add_executable(ShortPad ${ShPSource} 
                        ${ImSource} 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/backends/imgui_impl_sdl3.cpp 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/backends/imgui_impl_sdlrenderer3.cpp 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/freetype/imgui_freetype.cpp)

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES aarch64 OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES armv8l)
    target_link_libraries(ShortPad PUBLIC z
                                          png
                                          brotlicommon
										  brotlienc
										  brotlidec
                                          ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/lib/SDL3/libSDL3-aarch64.a 
                                          ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/lib/FreeType/libfreetype-aarch64.a)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES x86_64 AND DEFINED CROSS_COMPILE)
    target_link_libraries(ShortPad PUBLIC z
                                          png
                                          brotlicommon
										  brotlienc
										  brotlidec
                                          ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/lib/SDL3/libSDL3-aarch64.a 
                                          ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/lib/FreeType/libfreetype-aarch64.a)
else()
    target_link_libraries(ShortPad PUBLIC z
                                          png
                                          brotlicommon
										  brotlienc
										  brotlidec
                                          ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/lib/SDL3/libSDL3.a 
                                          ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/lib/FreeType/libfreetype.a)
endif()
