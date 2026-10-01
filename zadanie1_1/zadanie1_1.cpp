#include <omp.h>       // OpenMP для многопоточности
#include <iostream>     // ввод/вывод C++
#include <cstdlib>      // для printf

int main(int argc, char* argv[]) {
    // Задаём количество потоков
    int threads = 8;

    // Устанавливаем число потоков для параллельной области OpenMP
    omp_set_num_threads(threads);

    // Параллельная область: код внутри выполняется каждым потоком
#pragma omp parallel
    {
        // Получаем номер текущего потока (0 .. threads-1)
        int tid = omp_get_thread_num();
        // Получаем общее количество потоков в команде
        int total = omp_get_num_threads();
        // Каждый поток выводит свой номер и общее число потоков
        printf("Potok %d iz %d: Hello World\n", tid, total);
    }

    return 0;
}
