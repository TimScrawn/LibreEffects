cmake_minimum_required(VERSION 3.20)
project(LibreCanvas)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Application source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/canvaswidget.cpp
    src/canvaswidget.h
    src/layer.cpp
    src/layer.h
    src/document.cpp
    src/document.h
    src/layerpanel.cpp
    src/layerpanel.h
    src/tool.cpp
    src/tool.h
    src/toolpanel.cpp
    src/toolpanel.h
    src/history.cpp
    src/history.h
    src/lassotool.cpp
    src/lassotool.h
    src/clonestamptool.cpp
    src/clonestamptool.h
    src/transformtool.cpp
    src/transformtool.h
)

# Application resources
set(RESOURCES
    resources/librecanvas.qrc
)

# Create executable
add_executable(LibreCanvas ${SOURCES} ${RESOURCES})

# Link libraries
target_link_libraries(LibreCanvas PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    branding
)

# Include directories
target_include_directories(LibreCanvas PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs
)

# Install rules
install(TARGETS LibreCanvas
    RUNTIME DESTINATION bin
)

