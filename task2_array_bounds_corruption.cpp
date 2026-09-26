#include <iostream>

int main(){
    // Воссоздадим ситуацию в методических материалах
    int c = 3, d = 4, e = 5, f = 6;
    int a[6] = {1, 1, 1, 1, 1, 1};
    int b[2] = {2, 2};
    
    b[3] = 11;
    std::cout << "a: ";
    for(auto n: a){
        std::cout << n << ' ';
    }
    std::cout << std::endl << "b: ";
    for(auto n: b){
        std::cout << n << ' ';
    }
    std::cout << std::endl << "c: " << c << std::endl;
    std::cout << "d: " << d << std::endl;
    std::cout << "e: " << e << std::endl;
    std::cout << "f: " << f << std::endl;
    std::cout << std::endl << std::endl;
}
