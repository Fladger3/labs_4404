#include <iostream>
#include <omp.h>          // заголовок OpenMP: даёт omp_set_num_threads, omp_get_thread_num и т.д.

int main() {
    const int NUM_THREADS = 8;   // фиксированное число потоков (менять здесь)

    omp_set_num_threads(NUM_THREADS); // сообщаем среде выполнения, сколько потоков создать

#pragma omp parallel         // начало параллельной области: создаётся NUM_THREADS потоков
    {
        int tid = omp_get_thread_num();   // уникальный номер текущего потока (0..NUM_THREADS-1)
        int nthreads = omp_get_num_threads(); // фактическое число потоков в этой параллельной области

        // critical защищает std::cout: иначе строки разных потоков могут перемешаться
#pragma omp critical
        std::cout << "Thread " << tid << " of " << nthreads << std::endl;
    }   // конец параллельной области: все дочерние потоки завершаются, остаётся только основной

    return 0;
}