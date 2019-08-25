cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(Light2D)

find_package(SDL2 REQUIRED)

add_subdirectory(libglw)

file(GLOB SRC
    "*.cpp"
    "*.h"
)

add_executable(Light2D ${SRC})
target_link_libraries(Light2D libglw ${SDL2_LIBRARY} ${SDL2_SDLMAIN_LIBRARY})
target_include_directories(Light2D 
    PUBLIC ${SDL2_INCLUDE_DIR}
)
set_target_properties(Light2D PROPERTIES
    CXX_STANDARD 17
)