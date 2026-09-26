#include <iostream>

int main(){
    int a[10];
    a[100000] = 22; // Segfault error
}
