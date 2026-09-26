#include <iostream>
#include <chrono>
#include <cstdlib>

void iterate_all(int* arr, size_t arr_size){
    // Using code from 3-rd example: igoro.com/archive/gallery-of-processor-cache-effects/
    int steps = 64 * 1024 * 1024; // Arbitrary number of steps
    size_t lengthMod = arr_size - 1;
    for (int i = 0; i < steps; i++)
    {
        arr[(i * 16) & lengthMod]++; // (x & lengthMod) is equal to (x % arr.Length)
    }
}

int main(){
    constexpr size_t start_size = 1024 / sizeof(int); // 1 KB array size of int
    constexpr size_t last_size = 128*1024*1024 / sizeof(int); // 128 MB array size of int
    for(int i = 1; start_size*i <= last_size; i*=2){
        size_t current_size = start_size*i;
        int* test_arr = (int*) calloc(current_size, sizeof(int)); // Fill array with zeros
        auto start = std::chrono::high_resolution_clock::now();
        iterate_all(test_arr, current_size);
        auto end = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
        std::cout << "Microseconds working with size " << current_size*sizeof(int) / 1024 << " KB: " << ns.count() << std::endl;
        delete test_arr;
    }
}
