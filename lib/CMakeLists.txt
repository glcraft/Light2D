cmake_minimum_required(VERSION 3.10)

project(LibLight2D)

file(GLOB LIBSRC
    "*.cpp"
    "*.h"
)
add_library(LibLight2D ${LIBSRC})