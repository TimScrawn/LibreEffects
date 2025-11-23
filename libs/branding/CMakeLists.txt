cmake_minimum_required(VERSION 3.20)

add_library(branding STATIC
    theme.cpp
    theme.h
)

target_include_directories(branding PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

