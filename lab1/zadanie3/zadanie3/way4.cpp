#include <iostream>
#include <omp.h>
#include "ways.h"

void way4(int n) {
    int ids[64] = { 0 };

#pragma omp parallel num_threads(n)
    {
        int tid = omp_get_thread_num();
        ids[tid] = tid;

        // barrier: ждЄм, пока все потоки заполн€т свои €чейки ids[]
#pragma omp barrier

// single: блок выполн€ет ровно один поток. ¬ конце single
// есть не€вный барьер Ч остальные потоки ждут его завершени€.
#pragma omp single
        {
            for (int i = n - 1; i >= 0; --i)
                std::cout << "Thread " << ids[i]
                << " of " << n << "\n";
        }
    }
}