cmake_minimum_required(VERSION 3.10)

project(Light2DApp)

find_package(SDL2 REQUIRED)

file(GLOB APPSRC
    "*.cpp"
    "*.h"
)

add_executable(Light2DApp ${APPSRC})
target_link_libraries(Light2DApp libglw ${SDL2_LIBRARY} ${SDL2_SDLMAIN_LIBRARY})
target_include_directories(Light2DApp 
    PUBLIC ${SDL2_INCLUDE_DIR}
)
message("libglw include : ${LIBGLW_INCLUDE_DIR}")
set_target_properties(Light2DApp PROPERTIES
    CXX_STANDARD 17
)