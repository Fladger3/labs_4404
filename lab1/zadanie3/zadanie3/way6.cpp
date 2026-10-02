#include <iostream>
#include <omp.h>
#include "ways.h"

void way6(int n) {
    int counter = n;   // разделяемая переменная

#pragma omp parallel num_threads(n)
    {
        int my_id;

        // critical: атомарно уменьшаем счётчик и сразу запоминаем результат.
        // Это надёжно работает в OpenMP 2.0 (VS 2019), в отличие от atomic capture.
#pragma omp critical
        {
            --counter;
            my_id = counter;
        }

        // Печать — тоже в critical, чтобы строки не перемешались.
        // (Можно объединить с блоком выше, но так нагляднее.)
#pragma omp critical
        std::cout << "Thread " << my_id
            << " of " << n << "\n";
    }
}