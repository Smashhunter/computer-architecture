#include <iostream>
#include <vector>
#include <chrono>
#include <cstdint>

#include "cpu.hpp"


// Объявляем внешнюю функцию, скомпилированную в CUDA

int main() {
    // Выделение памяти под матрицы
    std::vector<int64_t> A(N * N, 1);
    std::vector<int64_t> B(N * N, 2);
    std::vector<int64_t> C(N * N, 0);

    auto start = std::chrono::high_resolution_clock::now();
    
    scalar_ikj(A.data(), B.data(), C.data());
    
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "RESULT: " << ms << std::endl;
    return 0;
}
