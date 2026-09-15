#include <iostream>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <immintrin.h> // для _mm_free если нужен _mm_malloc

#include "cpu.hpp"

// Вспомогательная функция для выделения выровненной памяти
int64_t* allocate_aligned_int64(size_t count, size_t alignment = 16) {
    size_t bytes = count * sizeof(int64_t);
#if defined(_MSC_VER)
    return static_cast<int64_t*>(_aligned_malloc(bytes, alignment));
#else
    return static_cast<int64_t*>(std::aligned_alloc(alignment, bytes));
#endif
}

// Функция для освобождения памяти
void free_aligned(void* ptr) {
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

int main() {
    size_t total_elements = N * N;

    // Выделяем память с выравниванием по 16 байт (128 бит под SSE)
    int64_t* A = allocate_aligned_int64(total_elements, 16);
    int64_t* B = allocate_aligned_int64(total_elements, 16);
    int64_t* C = allocate_aligned_int64(total_elements, 16);

    // Инициализация значений
    std::fill_n(A, total_elements, 1);
    std::fill_n(B, total_elements, 2);
    std::fill_n(C, total_elements, 0);

    auto start = std::chrono::high_resolution_clock::now();
    
    sse_ikj(A, B, C);
    
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "RESULT: " << ms << std::endl;

    // Освобождаем выровненную память
    free_aligned(A);
    free_aligned(B);
    free_aligned(C);

    return 0;
}
