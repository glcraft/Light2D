cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(Light2D)

add_subdirectory(glm)
set (GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/glm")
add_subdirectory(libglw)
add_subdirectory(nlohmann_json)


add_subdirectory(lib)
add_subdirectory(app)
add_dependencies(Light2DApp LibLight2D)

target_include_directories(Light2DApp 
    PUBLIC "lib"
)