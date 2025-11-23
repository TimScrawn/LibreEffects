cmake_minimum_required(VERSION 3.20)
project(LibreEffects VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC requires /Zc:__cplusplus and /permissive- for proper C++17 support with Qt
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

# Build options
option(BUILD_LIBRECANVAS "Build LibreCanvas application" ON)

# Add branding library
add_subdirectory(libs/branding)

# Add core library
add_subdirectory(libs/core)

# Add applications
if(BUILD_LIBRECANVAS)
    add_subdirectory(apps/librecanvas)
endif()

# Installation
install(DIRECTORY libs/branding DESTINATION include/libreeffects
    FILES_MATCHING PATTERN "*.h"
)

