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

// Функция для создания тестового файла, чтобы программа работала из коробки
void generate_file(const std::string& filename, int N) {
    std::ofstream out(filename);
    if (!out.is_open()) return;
    for (int i = 0; i < N; ++i) {
        // Генерируем какие-то псевдо-случайные точки
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

    // Проверяем, есть ли файл, если нет - создаем на 1 000 000 точек
    std::ifstream test(filename);
    if (!test.is_open()) {
        std::cout << "File not found. Generating " << filename << " with 1000000 points...\n";
        generate_file(filename, 1000000);
    }
    test.close();

    // Читаем файл в вектор
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

    // =========================================================
    // ВЕРСИЯ 1: Распараллеливание цикла (Data Parallelism)
    // =========================================================
    double sum_x1 = 0, sum_y1 = 0, sum_z1 = 0;
    double t1 = omp_get_wtime();

#pragma omp parallel for reduction(+:sum_x1, sum_y1, sum_z1) schedule(static)
    for (int i = 0; i < N; ++i) {
        sum_x1 += points[i].x;
        sum_y1 += points[i].y;
        sum_z1 += points[i].z;
    }

    double t2 = omp_get_wtime();
    double time_loop = t2 - t1;

    std::cout << "--- Version 1: Loop Parallelism ---\n";
    std::cout << "Time: " << time_loop << " sec\n";
    std::cout << "Center: (" << sum_x1 / N << ", " << sum_y1 / N << ", " << sum_z1 / N << ")\n\n";

    // =========================================================
    // ВЕРСИЯ 2: Функциональная декомпозиция (Task Parallelism)
    // =========================================================
    double sum_x2 = 0, sum_y2 = 0, sum_z2 = 0;
    double t3 = omp_get_wtime();

    // Используем секции: каждый блок выполняется своим потоком (или группой потоков)
#pragma omp parallel sections reduction(+:sum_x2, sum_y2, sum_z2)
    {
#pragma omp section
        {
            for (int i = 0; i < N; ++i) sum_x2 += points[i].x;
        }
#pragma omp section
        {
            for (int i = 0; i < N; ++i) sum_y2 += points[i].y;
        }
#pragma omp section
        {
            for (int i = 0; i < N; ++i) sum_z2 += points[i].z;
        }
    }

    double t4 = omp_get_wtime();
    double time_func = t4 - t3;

    std::cout << "--- Version 2: Functional Decomposition ---\n";
    std::cout << "Time: " << time_func << " sec\n";
    std::cout << "Center: (" << sum_x2 / N << ", " << sum_y2 / N << ", " << sum_z2 / N << ")\n\n";

    // Проверка
    double diff = std::fabs(sum_x1 - sum_x2) + std::fabs(sum_y1 - sum_y2) + std::fabs(sum_z1 - sum_z2);
    std::cout << "Check: " << (diff < 1e-4 ? "OK" : "FAIL") << "\n";

    return 0;
}