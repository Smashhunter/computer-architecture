#include <iostream>

int main(){
    constexpr int REPS = 100000;
    int* test;
    for(int i = 0; i < REPS; i++){
        test = new int(5);
        (*test)++;
        delete test;
    }
    getchar();
    return 0;
}
