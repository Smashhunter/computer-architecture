cmake_minimum_required(VERSION 3.18)
project(MatrixMultiplication LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Проверяем, что используется компилятор GCC или Clang
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(WARNING "Скрипт оптимизирован под GCC/Clang. Текущий компилятор: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Флаги для задач на ошибки/отладку (без оптимизации, без защиты стека)
set(FLAGS_DEBUG_NO_PROTECT "-O0" "-g" "-fno-stack-protector")

# Флаги для задач на производительность (максимальная оптимизация + инструкции процессора)
set(FLAGS_PERFORMANCE "-O3" "-march=native")


# ==============================================================================
# 1. Максимально возможная память (п. 1)
# ==============================================================================
add_executable(1_max_memory task1_max_memory.cpp)
target_compile_options(1_max_memory PRIVATE "-O0" "-g")


# ==============================================================================
# 2. Выход за границы массива без указателей (п. 2)
# ==============================================================================
# 2.1 Искажение соседних переменных
add_executable(2_array_bounds_corruption task2_array_bounds_corruption.cpp)
target_compile_options(2_array_bounds_corruption PRIVATE ${FLAGS_DEBUG_NO_PROTECT})

# 2.2 Ошибка доступа к памяти (Access Violation / Segfault)
add_executable(2_array_bounds_segfault task2_array_bounds_segfault.cpp)
# target_compile_options(2_array_bounds_segfault PRIVATE ${FLAGS_DEBUG_NO_PROTECT})


# ==============================================================================
# 3. Выход за границы через УКАЗАТЕЛИ (п. 3)
# ==============================================================================
# 3.1 Искажение данных через указатели
add_executable(3_pointer_bounds_corruption task3_pointer_bounds_corruption.cpp)
target_compile_options(3_pointer_bounds_corruption PRIVATE ${FLAGS_DEBUG_NO_PROTECT})

# 3.2 Segfault через арифметику указателей
add_executable(3_pointer_bounds_segfault task3_pointer_bounds_segfault.cpp)
target_compile_options(3_pointer_bounds_segfault PRIVATE ${FLAGS_DEBUG_NO_PROTECT})


# ==============================================================================
# 4. Утечка памяти и её устранение (п. 4)
# ==============================================================================
# 4.1 Демонстрация утечки
add_executable(4_memory_leak task4_memory_leak.cpp)
target_compile_options(4_memory_leak PRIVATE "-O0" "-g")

# 4.2 Исправленный вариант без утечки
add_executable(4_memory_leak_fixed task4_memory_leak_fixed.cpp)
target_compile_options(4_memory_leak_fixed PRIVATE "-O0" "-g")


# ==============================================================================
# 5. Эксперименты с кэш-памятью (п. 5)
# ==============================================================================
# 5.1 Замеры времени работы с кэшем (Эксперимент №3)
add_executable(5_cache_benchmarks task5_cache_benchmarks.cpp)
target_compile_options(5_cache_benchmarks PRIVATE ${FLAGS_PERFORMANCE})

# 5.2 Многопоточный пример (cache_example6.cpp)
add_executable(5_cache_example6 cache_example6.cpp)
target_compile_options(5_cache_example6 PRIVATE ${FLAGS_PERFORMANCE})
# Подключаем pthread для многопоточности в GCC
find_package(Threads REQUIRED)
target_link_libraries(5_cache_example6 PRIVATE Threads::Threads)

