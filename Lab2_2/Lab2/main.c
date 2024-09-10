#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


#define __STDC_FORMAT_MACROS
#include <inttypes.h>


double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

void matrix_vector_product(double* a, double* b, double* c, size_t m, size_t n)
{
    for (int i = 0; i < m; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}


void run_serial(size_t n, size_t m)
{
    double* a, * b, * c;
    a = (double*)malloc(sizeof(*a) * m * n);
    b = (double*)malloc(sizeof(*b) * n);
    c = (double*)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }

    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }

    for (size_t j = 0; j < n; j++)
        b[j] = j;

    double t = cpuSecond();
    matrix_vector_product(a, b, c, m, n);
    t = cpuSecond() - t;

    printf("Elapsed time (serial): %.6f sec.\n", t);
    free(a);
    free(b);
    free(c);
}

//local thread function
void matrix_vector_product_omp(double* a, double* b, double* c, size_t m, size_t n)
{
    #pragma omp parallel num_threads(20)
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
        for (int i = lb; i <= ub; i++)
        {
            c[i] = 0.0;
            for (int j = 0; j < n; j++)
                c[i] += a[i * n + j] * b[j];
        }
    }
}

void run_parallel(size_t n, size_t m)
{
    double* a, * b, * c;

    a = (double*)malloc(sizeof(*a) * m * n);
    b = (double*)malloc(sizeof(*b) * n);
    c = (double*)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }
    //not parallel initialization
    //for (size_t i = 0; i < m; i++)
    //{
    //    for (size_t j = 0; j < n; j++)
    //        a[i * n + j] = i + j;
    //}

    //for (size_t j = 0; j < n; j++)
    //    b[j] = j;

    //parallel initialization
    #pragma omp parallel num_threads(20)
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
        for (int i = lb; i <= ub; i++) {
            for (int j = 0; j < n; j++)
                a[i * n + j] = i + j;
            c[i] = 0.0;
        }
    }
    for (int j = 0; j < n; j++)
        b[j] = j;


    int num_threads[] = { 1, 2, 4, 7, 8, 16, 20, 40 };
    for (int i = 0; i < sizeof(num_threads) / sizeof(num_threads[0]); i++)
    {
        omp_set_num_threads(num_threads[i]);
        double t = cpuSecond();
        matrix_vector_product_omp(a, b, c, m, n);
        t = cpuSecond() - t;
        printf("Elapsed time with %d threads: %.6f sec.\n", num_threads[i], t);
    }
    free(a);
    free(b);
    free(c);
}

int main(int argc, char* argv[])
{
    size_t M = 40000;
    size_t N = 40000;
    if (argc > 1)
        M = atoi(argv[1]);
    if (argc > 2)
        N = atoi(argv[2]);
    run_serial(M, N);
    run_parallel(M, N);

    //size_t m = 1000;
    //size_t n = 1000;

    //printf("Matrix-vector product (c[m] = a[m, n] * b[n]; m = %d, n = %d)\n", m, n);
    //printf("Memory used: %" PRIu64 " MiB\n", ((m * n + m + n) * sizeof(double)) >> 20);

    ///*run_serial();
    //run_parallel();*/

    return 0;
}