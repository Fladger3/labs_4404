#include <omp.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int threads = 8;

    omp_set_num_threads(threads);

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();
        printf("Potok %d iz %d: Hello World\n", tid, total);
    }

    return 0;
}
