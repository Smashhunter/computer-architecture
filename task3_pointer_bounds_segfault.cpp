#include <iostream>

int main(){
    int* a = nullptr;
    *a =  10; // Доступ к ячейке памяти 0x0 запрещён.
}
