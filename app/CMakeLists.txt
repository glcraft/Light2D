cmake_minimum_required(VERSION 3.10)

project(Light2DApp)

find_package(SDL2 REQUIRED)

file(GLOB APPSRC
    "*.cpp"
    "*.h"
    "*.hpp"
)

add_executable(Light2DApp ${APPSRC})
target_link_libraries(Light2DApp libglw LibLight2D ${SDL2_LIBRARY} ${SDL2_SDLMAIN_LIBRARY} nlohmann_json::nlohmann_json)
target_include_directories(Light2DApp 
    PUBLIC ${SDL2_INCLUDE_DIR}
    PUBLIC "${CMAKE_SOURCE_DIR}/regex_literals"
)
message("libglw include : ${LIBGLW_INCLUDE_DIR}")
set_target_properties(Light2DApp PROPERTIES
    CXX_STANDARD 17
)
if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # GCC
    target_link_libraries(Light2DApp stdc++fs)
endif()
