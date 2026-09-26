#include <iostream>

void fill_with(int* arr, size_t size, int n){
    while(size--){
        arr[size] = n;
    }
}
void print_arr(int* arr, size_t size){
    for(size_t i = 0; i < size; i++){
        std::cout << arr[i] << ' ';
    }
    std::cout << std::endl;
}

int main(){
    size_t A = 5, B = 2;
    int* a = new int[A];
    int* b = new int[B];
    fill_with(a, A, 1);
    fill_with(b, B, 2);

    *(b-5)=11;

    std::cout << "a: ";
    print_arr(a, A);
    std::cout << "b: ";
    print_arr(b, B);

    return 0;
}
