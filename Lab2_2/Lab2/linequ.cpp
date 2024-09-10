#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

const int N = 10000; // Размер матрицы и вектора

void simpleIterationMethod(std::vector<double>& x, const std::vector<std::vector<double>>& A, const std::vector<double>& b) {
    const double epsilon = 1e-6;
    const int maxIterations = 1000;

    std::vector<double> x_new(N, 0.0);

    for (int iter = 0; iter < maxIterations; ++iter) {
        #pragma omp parallel for
        for (int i = 0; i < N; ++i) {
            double sum = 0.0;
            for (int j = 0; j < N; ++j) {
                if (j != i) {
                    sum += A[i][j] * x[j];
                }
            }
            x_new[i] = (b[i] - sum) / A[i][i];
        }

        double error = 0.0;
        for (int i = 0; i < N; ++i) {
            error += std::abs(x_new[i] - x[i]);
            x[i] = x_new[i];
        }

        if (error < epsilon) {
            break;
        }
    }
}

int main() {
    std::vector<std::vector<double>> A(N, std::vector<double>(N, 1.0));
    for (int i = 0; i < N; ++i) {
        A[i][i] = 2.0;
    }

    std::vector<double> b(N, N + 1);
    std::vector<double> x(N, 0.0);

    double start_time, end_time;
    int num_threads;

    for (num_threads = 1; num_threads <= omp_get_max_threads(); ++num_threads) {
        omp_set_num_threads(num_threads);
        start_time = omp_get_wtime();
        simpleIterationMethod(x, A, b);
        end_time = omp_get_wtime();
        std::cout << "Time taken with " << num_threads << " threads: " << end_time - start_time << " seconds" << std::endl;
    }

    return 0;
}