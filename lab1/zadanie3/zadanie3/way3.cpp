#include <iostream>
#include <omp.h>
#include "ways.h"

void way3(int n) {
#pragma omp parallel num_threads(n)
    {
        // Цикл идёт от n-1 до 0. ordered гарантирует порядок итераций.
#pragma omp for ordered
        for (int i = n - 1; i >= 0; --i) {
#pragma omp ordered
            std::cout << "Thread " << i
                << " of " << n << "\n";
        }
    }
}