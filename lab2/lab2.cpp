#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <omp.h>

// Ручное умножение матриц A(MxK) * B(KxN) = C(MxN)
static void matmul_serial(const std::vector<double>& A,
    const std::vector<double>& B,
    std::vector<double>& C,
    int M, int K, int N) {
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            double aik = A[(size_t)i * K + k];
            const double* Bk = &B[(size_t)k * N];
            double* Ci = &C[(size_t)i * N];
            for (int j = 0; j < N; ++j) {
                Ci[j] += aik * Bk[j];
            }
        }
    }
}

static void matmul_parallel(const std::vector<double>& A,
    const std::vector<double>& B,
    std::vector<double>& C,
    int M, int K, int N) {
#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            double aik = A[(size_t)i * K + k];
            const double* Bk = &B[(size_t)k * N];
            double* Ci = &C[(size_t)i * N];
            for (int j = 0; j < N; ++j) {
                Ci[j] += aik * Bk[j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <num_threads> <M> <K> [N]\n";
        return 1;
    }

    int NUM_THREADS = std::atoi(argv[1]);
    int M = std::atoi(argv[2]);
    int K = std::atoi(argv[3]);
    int N = (argc >= 5) ? std::atoi(argv[4]) : K;

    omp_set_num_threads(NUM_THREADS);

    std::cout << "Matrix multiplication: A(" << M << "x" << K
        << ") * B(" << K << "x" << N << ") = C(" << M << "x" << N << ")\n";
    std::cout << "Threads: " << NUM_THREADS << "\n\n";

    std::vector<double> A((size_t)M * K), B((size_t)K * N),
        Cser((size_t)M * N, 0.0), Cpar((size_t)M * N, 0.0);

    for (size_t i = 0; i < A.size(); ++i) A[i] = ((i % 10) + 1) / 10.0;
    for (size_t i = 0; i < B.size(); ++i) B[i] = ((i % 10) + 1) / 10.0;

    // Последовательная версия
    double t0 = omp_get_wtime();
    matmul_serial(A, B, Cser, M, K, N);
    double t1 = omp_get_wtime();
    double t_serial = t1 - t0;

    // Параллельная версия (3 прогона, берем лучший для точности)
    double t_par = 1e30;
    for (int rep = 0; rep < 3; ++rep) {
        std::fill(Cpar.begin(), Cpar.end(), 0.0);
        double t2 = omp_get_wtime();
        matmul_parallel(A, B, Cpar, M, K, N);
        double t3 = omp_get_wtime();
        if (t3 - t2 < t_par) t_par = t3 - t2;
    }

    // Проверка корректности
    double max_diff = 0.0;
    for (size_t i = 0; i < Cser.size(); ++i) {
        double d = std::fabs(Cser[i] - Cpar[i]);
        if (d > max_diff) max_diff = d;
    }

    double speedup = t_serial / t_par;
    double efficiency = (speedup / NUM_THREADS) * 100.0;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Serial:     time = " << t_serial << " sec\n";
    std::cout << "Parallel:   time = " << t_par << " sec\n";
    std::cout << "Speedup:    " << speedup << "x\n";
    std::cout << "Efficiency: " << efficiency << " %\n";
    std::cout << "Max diff:   " << max_diff << " (EPS = 0.000001)\n";
    std::cout << "Check:      " << (max_diff < 1e-6 ? "OK" : "FAIL") << "\n";

    return 0;
}