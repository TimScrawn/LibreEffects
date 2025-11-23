cmake_minimum_required(VERSION 3.20)
project(LibreEffectsCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Branding library will be added from parent CMakeLists

# Core library (placeholder for future core components)
add_library(core STATIC
    # Core components will be added here
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core PUBLIC branding)

