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

inline __m128i _mm_mul_epi64_custom(__m128i a, __m128i b) {
    // a = [ a0_low, a0_high, a1_low, a1_high ]
    // b = [ b0_low, b0_high, b1_low, b1_high ]

    __m128i b_swapped = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i a_swapped = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1));
    // b_swapped = [ b0_high, b0_low, b1_high, b1_low ]
    // a_swapped = [ a0_high, a0_low, a1_high, a1_low ]

    // 2. Умножение младших частей (берет индексы 0 и 2):
    // a[0]*b[0] -> a0_low * b0_low (64 бита)
    // a[2]*b[2] -> a1_low * b1_low (64 бита)
    __m128i prod_low = _mm_mul_epu32(a, b); 
    // prod_low = [ (a0_low * b0_low)_64, (a1_low * b1_low)_64 ]

    // 3. Перекрестное умножение 1:
    // a[0]*b_swapped[0] -> a0_low * b0_high
    // a[2]*b_swapped[2] -> a1_low * b1_high
    __m128i prod_high1 = _mm_mul_epu32(a, b_swapped);
    // prod_high1 = [ (a0_low * b0_high)_64, (a1_low * b1_high)_64 ]

    // Перекрестное умножение 2:
    // a_swapped[0]*b[0] -> a0_high * b0_low
    // a_swapped[2]*b[2] -> a1_high * b1_low
    __m128i prod_high2 = _mm_mul_epu32(a_swapped, b);
    // prod_high2 = [ (a0_high * b0_low)_64, (a1_high * b1_low)_64 ]

    // 4. Складываем перекрестные части (целиком 64-битные сложения):
    __m128i prod_high = _mm_add_epi64(prod_high1, prod_high2);
    // prod_high = [ (a0_low*b0_high + a0_high*b0_low)_64, (a1_low*b1_high + a1_high*b1_low)_64 ]

    // 5. Сдвигаем влево на 32 бита:
    // Младшие 32 бита каждого 64-битного числа обнуляются, а значение уходит в старшие 32 бита!
    prod_high = _mm_slli_epi64(prod_high, 32);

    // 6. Складываем с prod_low:
    // Так как у prod_high младшие 32 бита равен 0, при сложении с prod_low
    // они просто объединяются без переносов!
    return _mm_add_epi64(prod_low, prod_high);
}

// 3. SSE вариант с i-k-j обходом (по 2 элемента int64_t за итерацию)
void sse_ikj(const int64_t* A, const int64_t* B, int64_t* C) {
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            __m128i r_vec = _mm_set1_epi64x(A[i * N + k]); // [ r, r ]
            
            for (int j = 0; j < N; j += 2) {
                __m128i b_vec = _mm_load_si128(reinterpret_cast<const __m128i*>(&B[k * N + j])); // [ B[k][j], B[k][j+1] ]
                __m128i c_vec = _mm_load_si128(reinterpret_cast<const __m128i*>(&C[i * N + j])); // [ C[i][j], C[i][j+1] ]
                
                __m128i prod = _mm_mul_epi64_custom(r_vec, b_vec); // [ r*B[k][j], r*B[k][j+1] ]
                c_vec = _mm_add_epi64(c_vec, prod); // C += prod
                
                _mm_store_si128(reinterpret_cast<__m128i*>(&C[i * N + j]), c_vec);
            }
        }
    }
}
