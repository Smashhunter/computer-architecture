cmake_minimum_required(VERSION 3.18)
project(MatrixMultiplication LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)

# Настройки архитектуры для GTX 1660 Ti (Turing sm_75)
set(CMAKE_CUDA_ARCHITECTURES 75)
set(MATRIX_SIZE 4096 CACHE STRING "Size of square matrix in every bench")


# Флаги компиляции CPU для SSE4.1
if(MSVC)
    add_compile_options(/arch:AVX) # Включает SSE/AVX инструкции в MSVC
else()
    add_compile_options(-msse4.2 -O3)
endif()

# Создаем исполнимый файл CUDA
add_executable(main main.cpp matrix_cuda.cu cpu.hpp)

target_compile_definitions(main PRIVATE
    N=MATRIX_SIZE
)

# Создаем исполнимый файл CPU
# add_executable(cpu_bench main_cpu.cpp)
