#include <iostream>

int main(){
    constexpr int REPS = 100000000;
    int* test;
    for(int i = 0; i < REPS; i++){
        test = new int(5);
        (*test)++;
        test = nullptr;
    }
    getchar();
    return 0;
}
