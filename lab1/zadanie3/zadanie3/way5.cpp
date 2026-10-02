#include <iostream>
#include <omp.h>
#include "ways.h"

void way5(int n) {
    int ids[64] = { 0 };

#pragma omp parallel num_threads(n)
    {
        int tid = omp_get_thread_num();
        ids[tid] = tid;

#pragma omp barrier   // синхронизация: все ids[] заполнены

        // master: блок выполняет только поток 0.
        // В отличие от single, в конце master НЕТ неявного барьера.
#pragma omp master
        {
            for (int i = n - 1; i >= 0; --i)
                std::cout << "Thread " << ids[i]
                << " of " << n << "\n";
        }
    }
}