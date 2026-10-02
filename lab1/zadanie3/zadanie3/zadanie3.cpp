#include <iostream>
#include "ways.h"

int main() {
    const int NUM_THREADS = 8;   // фиксированное число потоков

    std::cout << "=== Way 1: array + ordered ===\n";
    way1(NUM_THREADS);

    std::cout << "\n=== Way 2: counter + critical ===\n";
    way2(NUM_THREADS);

    std::cout << "\n=== Way 3: ordered + reverse for ===\n";
    way3(NUM_THREADS);

    std::cout << "\n=== Way 4: array + single + barrier ===\n";
    way4(NUM_THREADS);

    std::cout << "\n=== Way 5: array + master + barrier ===\n";
    way5(NUM_THREADS);

    std::cout << "\n=== Way 6: atomic capture ===\n";
    way6(NUM_THREADS);

    return 0;
}