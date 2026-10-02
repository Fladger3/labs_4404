#include <iostream>
#include <iomanip>   // std::setprecision, std::fixed
#include <vector>
#include <cmath>     // std::abs
#include <omp.h>

int main() {
    const int    N = 16000; // размер массива
    const int    NUM_THREADS = 8;     // число потоков
    const double EPS = 1e-6;  // допуск при сравнении вещественных чисел

    omp_set_num_threads(NUM_THREADS); // задаём число потоков для всех параллельных областей

    std::vector<double> a(N), b(N);   // a — исходный массив, b — результат

    // Инициализация: a[i] = i. Параллельный for без schedule — по умолчанию static.
#pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        a[i] = static_cast<double>(i);
    }

    // Крайние элементы копируем как есть (для них формула не применяется)
    b[0] = a[0];
    b[N - 1] = a[N - 1];

    // Массив имён типов распределения — для вывода в консоль
    const char* schedules[] = { "static", "dynamic", "guided" };
    const int   num_sched = 3;

    for (int s = 0; s < num_sched; ++s) {
        double t_start = omp_get_wtime(); // точка старта замера (секунды, double)

        // Три разных schedule — три отдельные параллельные области.
        // schedule(static)  — итерации делятся на равные непрерывные блоки.
        // schedule(dynamic) — потоки берут по одной итерации по мере освобождения.
        // schedule(guided)  — размер блока уменьшается динамически.
        if (s == 0) {
#pragma omp parallel for schedule(static)
            for (int i = 1; i < N - 1; ++i) {
                b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
            }
        }
        else if (s == 1) {
#pragma omp parallel for schedule(dynamic)
            for (int i = 1; i < N - 1; ++i) {
                b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
            }
        }
        else {
#pragma omp parallel for schedule(guided)
            for (int i = 1; i < N - 1; ++i) {
                b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
            }
        }

        double t_end = omp_get_wtime(); // точка финиша замера

        // Эталонные значения для проверки корректности
        double expected1 = (a[0] + a[1] + a[2]) / 3.0;
        double expectedN = (a[N - 3] + a[N - 2] + a[N - 1]) / 3.0;

        // std::abs — модуль разности; сравниваем с EPS
        bool ok = (std::abs(b[1] - expected1) < EPS) &&
            (std::abs(b[N - 2] - expectedN) < EPS);

        std::cout << std::fixed << std::setprecision(6); // формат вывода: 6 знаков после точки
        std::cout << "Schedule = " << schedules[s]
            << ", threads = " << NUM_THREADS
            << ", time = " << (t_end - t_start) << " sec"
            << ", check = " << (ok ? "OK" : "FAIL") << "\n";
        std::cout << "  b[1]    = " << b[1] << " (expected " << expected1 << ")\n";
        std::cout << "  b[N-2]  = " << b[N - 2] << " (expected " << expectedN << ")\n";
    }

    return 0;
}