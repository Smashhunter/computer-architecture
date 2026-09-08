#include <cuda_runtime.h>
#include <iostream>

// #define N 4096
#define BLOCK_SIZE 16

__global__ void matrixMulCUDA(const int64_t* A, const int64_t* B, int64_t* C) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N) {
        int64_t value = 0;
        for (int k = 0; k < N; ++k) {
            value += A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = value;
    }
}

extern "C" void run_cuda_kernel(const int64_t* h_A, const int64_t* h_B, int64_t* h_C) {
    size_t bytes = N * N * sizeof(int64_t);
    int64_t *d_A, *d_B, *d_C;

    cudaMalloc(&d_A, bytes);
    cudaMalloc(&d_B, bytes);
    cudaMalloc(&d_C, bytes);

    cudaMemcpy(d_A, h_A, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, bytes, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 blocksPerGrid(N / BLOCK_SIZE, N / BLOCK_SIZE);

    matrixMulCUDA<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C);
    cudaDeviceSynchronize();

    cudaMemcpy(h_C, d_C, bytes, cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}
