#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SEED 42

// this is the serial way
void multiply_serial(int *p1, int *p2, int *res, int n) {
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            res[i + j] += p1[i] * p2[j];
        }
    }
}

int min(int a, int b) { return (a < b) ? a : b; }

int max(int a, int b) { return (a > b) ? a : b; }

void multiply_parallel(int *p1, int *p2, int *res, int n) {
// perelel loop dynamic makes it balance the load
#pragma omp parallel for schedule(dynamic)
    for (int k = 0; k <= 2 * n; k++) {
        
        int sum = 0;

        int start = max(0, k - n);

        int end = min(k, n);

        for (int i = start; i <= end; i++) {
            sum += p1[i] * p2[k - i];
        }

        // thread safe right final sum
        res[k] = sum;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("args= %s <degree_n> <num_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    omp_set_num_threads(num_threads); // sets the number of threads

    int *poly1 = malloc((n + 1) * sizeof(int));
    int *poly2 = malloc((n + 1) * sizeof(int));
    int *res_serial = calloc((2 * n + 1), sizeof(int));
    int *res_parallel = calloc((2 * n + 1), sizeof(int));

    double start_init = omp_get_wtime();
    srand(SEED);
    for (int i = 0; i <= n; i++) {
        poly1[i] = (rand() % 100) + 1;
        poly2[i] = (rand() % 100) + 1;
    }
    double end_init = omp_get_wtime();

    double start_serial = omp_get_wtime();
    multiply_serial(poly1, poly2, res_serial, n);
    double end_serial = omp_get_wtime();

    double start_parallel = omp_get_wtime();
    multiply_parallel(poly1, poly2, res_parallel, n);
    double end_parallel = omp_get_wtime();

    printf("initialization time: %f s\n", end_init - start_init);
    printf("serial time:   %f s\n", end_serial - start_serial);
    printf("parallel time: %f s\n", end_parallel - start_parallel);

    int errors = 0;
    for (int i = 0; i <= 2 * n; i++) {
        if (res_serial[i] != res_parallel[i]) {
            errors++;
            printf("Error: index %d, serial=%d, parallel=%d\n", i, res_serial[i], res_parallel[i]);
        }
    }

    if (errors == 0) {
        printf("succes\n");
    } else {
        printf("failed: %d errors\n", errors);
    }

    free(poly1);
    free(poly2);
    free(res_serial);

    return 0;
}