cmake_minimum_required(VERSION 3.26)
project(NexusShell)

set(CMAKE_C_STANDARD    17)
set(CMAKE_CXX_STANDARD  17)
set(CMAKE_C_STANDARD_REQUIRED   True)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES x86_64 AND DEFINED CROSS_COMPILE)
	set(CMAKE_C_COMPILER aarch64-linux-gcc)
	set(CMAKE_CXX_COMPILER aarch64-linux-g++)
endif()

file(GLOB NxShSource ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB ImGuiSource ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/*.cpp)
file(GLOB ImBackendSource ${CMAKE_CURRENT_SOURCE_DIR}/../imgui/backends/*.cpp)

add_executable(NexusShell ${NxShSource} ${ImGuiSource} ${ImBackendSource})

if((CMAKE_HOST_SYSTEM_PROCESSOR MATCHES aarch64 OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES armv8l) OR 
	(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES x86_64 AND DEFINED CROSS_COMPILE))
	target_link_libraries(NexusShell PRIVATE z
										   png
										   brotlicommon
										   brotlienc
										   brotlidec
										   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/FreeType/libfreetype-aarch64.a
										   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/SDL3/libSDL3-aarch64.a )
else()
	target_link_libraries(NexusShell PRIVATE z
										   png
										   brotlicommon
										   brotlienc
										   brotlidec
										   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/FreeType/libfreetype.a					  
										   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/SDL3/libSDL3.a )
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(NexusShell PRIVATE _DEBUG)
endif()
