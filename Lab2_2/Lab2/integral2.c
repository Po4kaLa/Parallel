#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>


#define __STDC_FORMAT_MACROS
#include <inttypes.h>

const double PI = 3.14159265358979323846;
const double a = -4.0;
const double b = 4.0;
const int nsteps = 40000000;

double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

double func(double x)
{
    return exp(-x * x);
}

double integrate(double (*func)(double), double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 0; i < n; i++)
        sum += func(a + h * (i + 0.5));

    sum *= h;

    return sum;
}

double integrate_omp(double (*func)(double), double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;
    #pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int tid = omp_get_thread_num();

        double sumloc = 0.0;
        for (int i = tid; i < n; i += nthreads)
            sumloc += func(a + h * (i + 0.5));
        #pragma omp atomic
        sum += sumloc;
    }
    sum *= h;
    return sum;
}

double run_serial()
{
    double t = cpuSecond();
    double res = integrate(func, a, b, nsteps);
    t = cpuSecond() - t;
    printf("Result (serial): %.12f; error %.12f\n", res, fabs(res - sqrt(PI)));
    return t;
}
double run_parallel(int num_threads)
{
    double t = cpuSecond();
    omp_set_num_threads(num_threads);
    double res = integrate_omp(func, a, b, nsteps);
    t = cpuSecond() - t;
    printf("Result (parallel, %d threads): %.12f; error %.12f\n", num_threads, res, fabs(res - sqrt(PI)));
    return t;
}

int main(int argc, char** argv)
{
    printf("Integration f(x) on [%.12f, %.12f], nsteps = %d\n", a, b, nsteps);
    double tserial = run_serial();

    int num_threads[] = { 1, 2, 4, 7, 8, 16, 20, 40 };
    double tparallel[8];

    for (int i = 0; i < 8; i++) {
        tparallel[i] = run_parallel(num_threads[i]);
    }

    printf("\nExecution time (serial): %.6f\n", tserial);
    for (int i = 0; i < 8; i++) {
        printf("Execution time (parallel, %d threads): %.6f\n", num_threads[i], tparallel[i]);
        printf("Speedup (vs serial): %.2f\n", tserial / tparallel[i]);
    }

    return 0;
}