#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

void matrix_vector_product(const std::vector<double>& a, const std::vector<double>& b, std::vector<double>& c, size_t m, size_t n, size_t start_row, size_t end_row) {
    for (size_t i = start_row; i < end_row; ++i) {
        c[i] = 0.0;
        for (size_t j = 0; j < n; ++j) {
            c[i] += a[i * n + j] * b[j];
        }
    }
}

void run_serial(size_t n, size_t m) {
    std::vector<double> a(m * n);
    std::vector<double> b(n);
    std::vector<double> c(m);

    // Инициализация матрицы и вектора
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            a[i * n + j] = i + j;
        }
    }

    for (size_t j = 0; j < n; j++) {
        b[j] = j;
    }

    auto start = std::chrono::high_resolution_clock::now();
    matrix_vector_product(a, b, c, m, n, 0, m);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time (serial): " << elapsed.count() << " sec." << std::endl;
}

void run_parallel(size_t n, size_t m, size_t num_threads) {
    std::vector<double> a(m * n);
    std::vector<double> b(n);
    std::vector<double> c(m);

    // Инициализация матрицы
    auto init_thread = [&a, m, n](size_t start_row, size_t end_row) {
        for (size_t i = start_row; i < end_row; ++i) {
            for (size_t j = 0; j < n; ++j) {
                a[i * n + j] = i + j;
            }
        }
    };

    std::vector<std::thread> threads;

    size_t rows_per_thread = m / num_threads;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_row = i * rows_per_thread;
        size_t end_row = (i == num_threads - 1) ? m : start_row + rows_per_thread;
        threads.emplace_back(init_thread, start_row, end_row);
    }

    for (auto& t : threads) {
        t.join();
    }

    // Инициализация вектора
    for (size_t j = 0; j < n; j++) {
        b[j] = j;
    }

    auto start = std::chrono::high_resolution_clock::now();

    threads.clear();
    auto product_thread = [&a, &b, &c, m, n](size_t start_row, size_t end_row) {
        matrix_vector_product(a, b, c, m, n, start_row, end_row);
    };

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_row = i * rows_per_thread;
        size_t end_row = (i == num_threads - 1) ? m : start_row + rows_per_thread;
        threads.emplace_back(product_thread, start_row, end_row);
    }
    
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time (parallel with " << num_threads << " threads): " << elapsed.count() << " sec." << std::endl;
}

int main() {
    size_t m = 20000; 
    size_t n = 20000; 

    run_serial(n, m);

    int num_threads[] = { 1, 2, 4, 7, 8, 16, 20, 40 };
    
    for (int threads : num_threads) {
        run_parallel(n, m, threads);
    }

    return 0;
}
