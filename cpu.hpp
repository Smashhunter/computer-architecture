#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>

constexpr int N = 4096;

// 1. Наивный скалярный вариант (i-j-k)
void scalar_naive(const int64_t* A, const int64_t* B, int64_t* C) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int64_t sum = 0;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

// 2. Оптимизированный скалярный вариант (i-k-j)
void scalar_ikj(const int64_t* A, const int64_t* B, int64_t* C) {
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            int64_t r = A[i * N + k];
            for (int j = 0; j < N; ++j) {
                C[i * N + j] += r * B[k * N + j];
            }
        }
    }
}

// Вспомогательная функция: 64-битное целочисленное умножение в SSE (2x int64_t)
inline __m128i _mm_mul_epi64_custom(__m128i a, __m128i b) {
    __m128i b_swapped = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i prod_low   = _mm_mul_epu32(a, b);
    __m128i prod_high1 = _mm_mul_epu32(a, b_swapped);
    __m128i prod_high2 = _mm_mul_epu32(_mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1)), b);
    
    __m128i prod_high = _mm_add_epi64(prod_high1, prod_high2);
    prod_high = _mm_slli_epi64(prod_high, 32);
    
    return _mm_add_epi64(prod_low, prod_high);
}

// 3. SSE вариант с i-k-j обходом (по 2 элемента int64_t за итерацию)
void sse_ikj(const int64_t* A, const int64_t* B, int64_t* C) {
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            __m128i r_vec = _mm_set1_epi64x(A[i * N + k]);
            
            for (int j = 0; j < N; j += 2) {
                __m128i b_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&B[k * N + j]));
                __m128i c_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&C[i * N + j]));
                
                __m128i prod = _mm_mul_epi64_custom(r_vec, b_vec);
                c_vec = _mm_add_epi64(c_vec, prod);
                
                _mm_storeu_si128(reinterpret_cast<__m128i*>(&C[i * N + j]), c_vec);
            }
        }
    }
}
