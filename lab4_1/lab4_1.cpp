#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <omp.h>

struct Point3D {
    double x, y, z;
};

// Генератор файла (если файла нет)
void generate_file(const std::string& filename, int N) {
    std::ofstream out(filename);
    if (!out.is_open()) return;
    for (int i = 0; i < N; ++i) {
        out << (i % 100) * 0.1 << " " << (i % 50) * 0.2 << " " << (i % 25) * 0.3 << "\n";
    }
    out.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <num_threads> <filename>\n";
        return 1;
    }

    int NUM_THREADS = std::atoi(argv[1]);
    std::string filename = argv[2];

    omp_set_num_threads(NUM_THREADS);

    std::ifstream test(filename);
    if (!test.is_open()) {
        std::cout << "File not found. Generating " << filename << " with 1000000 points...\n";
        generate_file(filename, 1000000);
    }
    test.close();

    // Чтение файла
    std::ifstream in(filename);
    std::vector<Point3D> points;
    Point3D p;
    while (in >> p.x >> p.y >> p.z) {
        points.push_back(p);
    }
    in.close();

    int N = points.size();
    if (N == 0) {
        std::cerr << "Error: empty file.\n";
        return 1;
    }

    std::cout << "Loaded " << N << " points from " << filename << "\n";
    std::cout << "Threads: " << NUM_THREADS << "\n\n";

    // Переменная для глобальной суммы (разделяемая всеми потоками)
    double global_sum = 0.0;

    double t_start = omp_get_wtime();

    // Функциональная декомпозиция
#pragma omp parallel sections
    {
#pragma omp section
        {
            double local_sum_x = 0;
            for (int i = 0; i < N; ++i) local_sum_x += points[i].x;
            // Критическая секция: только один поток может обновлять global_sum
#pragma omp critical
            {
                global_sum += local_sum_x;
            }
        }

#pragma omp section
        {
            double local_sum_y = 0;
            for (int i = 0; i < N; ++i) local_sum_y += points[i].y;
#pragma omp critical
            {
                global_sum += local_sum_y;
            }
        }

#pragma omp section
        {
            double local_sum_z = 0;
            for (int i = 0; i < N; ++i) local_sum_z += points[i].z;
#pragma omp critical
            {
                global_sum += local_sum_z;
            }
        }
    }

    double t_end = omp_get_wtime();
    double time_exec = t_end - t_start;

    // Вычисление финального значения по формуле (Σx + Σy + Σz) / 3N
    double result = global_sum / (3.0 * N);

    std::cout << "--- Functional Decomposition + Critical Section ---\n";
    std::cout << "Time: " << time_exec << " sec\n";
    std::cout << "Global sum: " << global_sum << "\n";
    std::cout << "Result (Σx+Σy+Σz)/3N = " << result << "\n\n";

    // Проверка корректности
    // Посчитаем последовательно для сравнения
    double check_sum = 0;
    for (int i = 0; i < N; ++i) check_sum += points[i].x + points[i].y + points[i].z;
    double check_result = check_sum / (3.0 * N);

    double diff = std::fabs(result - check_result);
    std::cout << "Check sequential value: " << check_result << "\n";
    std::cout << "Check: " << (diff < 1e-6 ? "OK" : "FAIL") << "\n";

    return 0;
}