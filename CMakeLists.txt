cmake_minimum_required(VERSION 3.16)

project(qtsolitaire VERSION 1.0 LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wunused)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

# Recursively find all .h and .hpp files in src/ and subdirectories
file(GLOB_RECURSE PROJECT_SOURCES
    "src/*.h"
    "src/*.cpp"
    "src/*.qrc"
    "src/*.ui"
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network)


