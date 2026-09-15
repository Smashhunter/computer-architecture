#include <iostream>
#include <vector>
#include <chrono>
#include <cstdint>

constexpr int N = 4096;

// Объявляем внешнюю функцию, скомпилированную в CUDA
extern "C" void run_cuda_kernel(const int64_t* h_A, const int64_t* h_B, int64_t* h_C);

int main() {
    // Выделение памяти под матрицы
    std::vector<int64_t> A(N * N, 1);
    std::vector<int64_t> B(N * N, 2);
    std::vector<int64_t> C(N * N, 0);

    auto start = std::chrono::high_resolution_clock::now();
    
    // Запуск умножения на CUDA
    run_cuda_kernel(A.data(), B.data(), C.data());
    
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "RESULT: " << ms << std::endl;
    return 0;
}
