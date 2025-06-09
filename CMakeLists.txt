cmake_minimum_required(VERSION 3.14)
project(SpotlightSearch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

qt_standard_project_setup()

qt_add_executable(SpotlightSearch
    src/main.cpp
    src/spotlightwindow.cpp
    src/spotlightwindow.h
)

target_link_libraries(SpotlightSearch PRIVATE Qt6::Widgets)
