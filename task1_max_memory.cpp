#include <iostream>
#include <cstdlib>

size_t get_max_allocatable_block() {
    size_t low = 0;
    size_t high = 1ULL << 40; // Стартуем с 1 ТБ для 64-бит систем
    size_t max_success = 0;

    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        void* ptr = std::malloc(mid);

        if (ptr) {
            max_success = mid;
            std::free(ptr);
            low = mid + 1; // Пробуем больше
        } else {
            high = mid - 1; // Пробуем меньше
        }
    }
    return max_success;
}

int main(){
    std::cout << "Max block: " << get_max_allocatable_block() / (1024 * 1024) << " MB\n";
    return 0;
}
