#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

int f1(int *A){
    return A[0] + A[1] + A[2] + A[3] + A[4];
}

int f1_asm(int *A){
    int result;
    __asm__(
        // ".intel_syntax noprefix\n\t"
        "mov %0, 0\n\t"        
        "add %0, [%1]\n\t"
        "add %0, [%1+1*4]\n\t" // Смещение на 4 байта (sizeof(int))
        "add %0, [%1+2*4]\n\t"
        "add %0, [%1+3*4]\n\t"
        "add %0, [%1+4*4]\n\t"
        // ".att_syntax\n\t"
        : "=r" (result)
        : "S" (A)
        : "cc"
    );
    return result;
}

unsigned f2(int n){
    unsigned result = 0;
    for(int i = 1; i <= n; i++){
        result += i*i; 
    }
    return result;
}

unsigned f2_asm(int n){
    unsigned result = 0;
    __asm__(
        // ".intel_syntax noprefix\n\t"
        "mov ecx, %1\n\t"
        "xor ebx, ebx\n\t"
        "1:\n\t"      
        "   mov eax, ecx\n\t"
        "   xor edx, edx\n\t"
        "   mul ecx\n\t" // Получили квадрат
        "   add ebx, eax\n\t"
        "   loop 1b\n\t"
        "2:\n\t"
        "mov %0, ebx\n\t"
        // ".att_syntax\n\t"
        : "=r" (result)
        : "r" (n)
        : "cc", "eax", "ecx", "ebx", "edx"
    );
    return result;
}

int main(){
    srand(time(0));
    
    int a[5] = { rand() % 101, rand() % 101, rand() % 101, rand() % 101, rand() % 101};
    for(auto i: a){
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    
    std::cout << "Сумма от f1: " << f1(a) << std::endl;
    std::cout << "Сумма от f1 с ассемблером: " << f1_asm(a) << std::endl;

    std::cout << "Введите число n — для вычисления суммы квадратов от 1 до n:\n";
    unsigned n, result;
    std::cin >> n;

    auto t1 = std::chrono::high_resolution_clock::now();
    result = f2(n);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Сумма квадратов от f2: " << result << std::endl;
    std::cout << "Функция f2 отработала (в нс): " <<
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
        << std::endl << std::endl;
    
    t1 = std::chrono::high_resolution_clock::now();
    result = f2_asm(n);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Сумма квадратов от f2 с ассемблером: " << result << std::endl;
    std::cout << "Функция f2 с ассемблером отработала (в нс): " <<
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
        << std::endl << std::endl;


    // __asm__(
    //     // ".intel_syntax noprefix\n\t"
    //     "mov eax, 1\n\t"        // Переносим 1 в eax (начальный результат)
    //     "mov ecx, %1\n\t"        // Счетчик цикла i = 1
    //     "CMP_LOOP:\n\t"
    //     "    cmp ecx, 1\n\t"   // Сравниваем i с n
    //     "    jb END_LOOP\n\t"   // Если i > n, выходим из цикла
    //     "    xor edx, edx\n\t"  // Очищаем edx перед mul, чтобы избежать ошибок
    //     "    mul ecx\n\t"       // eax = eax * ecx
    //     "    loop CMP_LOOP\n\t"
    //     "END_LOOP:\n\t"
    //     "mov %0, eax\n\t"
    //     // ".att_syntax\n\t"
    //     : "=r" (result)
    //     : "r" (n)
    //     : "eax", "ecx", "edx", "cc"
    // );

    // std::cout << result << "\n";
    // // std::system("pause");
    return 0;   
}
