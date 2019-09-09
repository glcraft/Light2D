cmake_minimum_required(VERSION 3.10)

project(LibLight2D)

file(GLOB LIBSRC
    "*.cpp"
    "*.h"
)
add_library(LibLight2D ${LIBSRC})
target_include_directories(LibLight2D 
    PUBLIC ${GLM_INCLUDE_DIR}
)
set_target_properties(LibLight2D PROPERTIES
    CXX_STANDARD 17
)