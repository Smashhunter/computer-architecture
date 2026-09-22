#include <cuda_runtime.h>
#include <iostream>

#define N 4096
#define BLOCK_SIZE 16

__global__ void matrixMulCUDA(const int64_t* A, const int64_t* B, int64_t* C) {
    // blockIdx - координаты (x, y) блока на сетке 256x256
    // blockDim - размер каждого блока (16x16)
    // threadIdx - координаты (x, y) и смещение потока внутри блока
    // 
    // Пример для элемента C[row=23][col=50]:
    // y (строка):  row = 23 => blockIdx.y = 23 / 16 = 1, threadIdx.y = 23 % 16 = 7
    // x (столбец): col = 50 => blockIdx.x = 50 / 16 = 3, threadIdx.x = 50 % 16 = 2
    
    int row = blockIdx.y * blockDim.y + threadIdx.y; // 1 * 16 + 7 = 23
    int col = blockIdx.x * blockDim.x + threadIdx.x; // 3 * 16 + 2 = 50

    if (row < N && col < N) {
        int64_t value = 0;
        for (int k = 0; k < N; ++k) {
            value += A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = value;
    }
}

extern "C" void run_cuda_kernel(const int64_t* h_A, const int64_t* h_B, int64_t* h_C) {
    int deviceCount = 0;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0) {
        std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: Видеокарта NVIDIA не найдена системой!" << std::endl;
        return;
    }

    size_t bytes = N * N * sizeof(int64_t);
    int64_t *d_A, *d_B, *d_C;

    // Аллоцируем память на Device (видеокарте)
    cudaError_t err;
    err = cudaMalloc((void**)&d_A, bytes);
    if (err != cudaSuccess) std::cerr << "cudaMalloc d_A error: " << cudaGetErrorString(err) << std::endl;
    
    err = cudaMalloc((void**)&d_B, bytes);
    if (err != cudaSuccess) std::cerr << "cudaMalloc d_B error: " << cudaGetErrorString(err) << std::endl;
    
    err = cudaMalloc((void**)&d_C, bytes);
    if (err != cudaSuccess) std::cerr << "cudaMalloc d_C error: " << cudaGetErrorString(err) << std::endl;

    // Единожды копируем с оперативной памяти в выделенную память видеокарты
    cudaMemcpy(d_A, h_A, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, bytes, cudaMemcpyHostToDevice);
    cudaMemset(d_C, 0, bytes);

    dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 blocksPerGrid(N / BLOCK_SIZE, N / BLOCK_SIZE);

    // Запускаем вычисления для каждого блока 
    matrixMulCUDA<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C);

    err = cudaGetLastError();
    if (err != cudaSuccess) {
        std::cerr << "Kernel launch error: " << cudaGetErrorString(err) << std::endl;
    }

    err = cudaDeviceSynchronize(); // Ждем пока кадый поток закончит счёт
    if (err != cudaSuccess) {
        std::cerr << "cudaDeviceSynchronize error: " << cudaGetErrorString(err) << std::endl;
    }

    err = cudaMemcpy(h_C, d_C, bytes, cudaMemcpyDeviceToHost); // Загружаем в результат в ОЗУ
    if (err != cudaSuccess) {
        std::cerr << "cudaMemcpy D2H error: " << cudaGetErrorString(err) << std::endl;
    }

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}
