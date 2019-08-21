cmake_minimum_required(VERSION 3.10)

project(Light2D)

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