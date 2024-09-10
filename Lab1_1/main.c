#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifdef USE_DOUBLE
typedef double number_t;
#define SIN(x) sin(x)
#else
typedef float number_t;
#define SIN(x) sinf(x)
#endif

#define ARRAY_SIZE 10000000

int main() {
    number_t *array = (number_t *)malloc(ARRAY_SIZE * sizeof(number_t));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        array[i] = SIN((2 * M_PI * i) / ARRAY_SIZE);
    }

    number_t sum = 0;
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        sum += array[i];
    }

    printf("Sum: %f\n", (double)sum);

    free(array);
    return 0;
}
