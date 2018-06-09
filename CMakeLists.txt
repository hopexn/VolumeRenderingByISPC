cmake_minimum_required(VERSION 3.10)
project(VolumeRenderingISPC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")

#添加Qt5依赖
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)
add_definitions(${QT_DEFINITIONS})

add_executable(VolumeRenderingISPC main.cpp DisplayWidget.h ispc/Renderer.o)

target_link_libraries(VolumeRenderingISPC Qt5::Widgets boost_system boost_timer)