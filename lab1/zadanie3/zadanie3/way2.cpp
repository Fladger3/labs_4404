#include <iostream>
#include <omp.h>
#include "ways.h"

void way2(int n) {
    // Разделяемая переменная: каждый поток её уменьшает на 1.
    int counter = n;

#pragma omp parallel num_threads(n)
    {
        // critical: только один поток одновременно декрементирует и печатает.
#pragma omp critical
        {
            --counter;
            std::cout << "Thread " << counter
                << " of " << n << "\n";
        }
    }
}