#include <iostream>
#include <vector>
#include <chrono>
#include <cstdint>

#include "cpu.hpp"

int main() {
    // Выделение памяти под матрицы
    std::vector<int64_t> A(N * N, 1);
    std::vector<int64_t> B(N * N, 2);
    std::vector<int64_t> C(N * N, 0);

    auto start = std::chrono::high_resolution_clock::now();
    
    scalar_naive(A.data(), B.data(), C.data());
    
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "RESULT: " << ms << std::endl;

    std::cout << std::endl;
    print_result(A.data());
    std::cout << std::endl;
    print_result(B.data());
    std::cout << std::endl;
    print_result(C.data());
    std::cout << std::endl;
    return 0;
}
