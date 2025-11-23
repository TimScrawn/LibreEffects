cmake_minimum_required(VERSION 3.20)
project(LibreEffectsCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Branding library will be added from parent CMakeLists

# Core library (placeholder for future core components)
# For now, we'll create an interface library since there are no sources yet
add_library(core INTERFACE)

target_include_directories(core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core INTERFACE branding)

