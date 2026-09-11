cmake_minimum_required(VERSION 3.18)

project(lab-2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    main
    main.cpp
)
target_compile_options(
    main
    PRIVATE
    -O0
    -save-temps=obj
    -masm=intel
)
