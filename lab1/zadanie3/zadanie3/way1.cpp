#include <iostream>
#include <omp.h>
#include "ways.h"

void way1(int n) {
    // ids[tid] = tid — заполняем номера потоков, потом печатаем с конца.
    int ids[64] = { 0 };

#pragma omp parallel num_threads(n)
    {
        int tid = omp_get_thread_num();
        ids[tid] = tid;

        // ordered-цикл идёт по возрастанию i, но внутри ordered-секции
        // порядок итераций сохраняется → печатаем ids с конца.
#pragma omp for ordered
        for (int i = 0; i < n; ++i) {
#pragma omp ordered
            std::cout << "Thread " << ids[n - 1 - i]
                << " of " << n << "\n";
        }
    }
}