#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SEED 42

// (ii) Σειριακός αλγόριθμος πολλαπλασιασμού 
void multiply_serial(int *p1, int *p2, int *res, int n) {
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            res[i + j] += p1[i] * p2[j];
        }
    }
}

// Συνάρτηση min για να βρούμε τα όρια
int min(int a, int b) { return (a < b) ? a : b; }
// Συνάρτηση max
int max(int a, int b) { return (a > b) ? a : b; }

void multiply_parallel(int *p1, int *p2, int *res, int n) {
    
    // Το τελικό μέγεθος είναι 2*n
    // Χρησιμοποιούμε schedule(dynamic) γιατί οι μεσαίες θέσεις έχουν
    // ΠΟΛΥ περισσότερη δουλειά από τις ακριανές.
    #pragma omp parallel for schedule(dynamic)
    for (int k = 0; k <= 2 * n; k++) {
        
        int sum = 0;
        
        // Μαθηματικά όρια για να μην βγούμε εκτός πίνακα:
        // Το i πρέπει να είναι τουλάχιστον 0, αλλά και τέτοιο ώστε το (k-i) <= n
        // Άρα i >= k - n.
        int start = max(0, k - n);
        
        // Το i δεν μπορεί να ξεπεράσει το n, αλλά και το k (αφού j >= 0)
        int end = min(k, n);

        for (int i = start; i <= end; i++) {
            sum += p1[i] * p2[k - i];
        }
        
        res[k] = sum;
    }
}

int main(int argc, char *argv[]) {
    // (iii) Λήψη ορισμάτων: βαθμός n και αριθμός νημάτων 
    if (argc < 3) {
        printf("Usage: %s <degree_n> <num_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    omp_set_num_threads(num_threads);

    // Δέσμευση μνήμης (βαθμός n σημαίνει n+1 στοιχεία)
    int *poly1 = malloc((n + 1) * sizeof(int));
    int *poly2 = malloc((n + 1) * sizeof(int));
    int *res_serial = calloc((2 * n + 1), sizeof(int));
    int *res_parallel = calloc((2 * n + 1), sizeof(int));

    // (i) Δημιουργία τυχαίων πλήρων πολυωνύμων με μη-μηδενικούς συντελεστές 
    double start_init = omp_get_wtime();
    srand(SEED);
    for (int i = 0; i <= n; i++) {
        poly1[i] = (rand() % 100) + 1; // +1 για να μην είναι ποτέ μηδέν
        poly2[i] = (rand() % 100) + 1;
    }
    double end_init = omp_get_wtime();

    double start_serial = omp_get_wtime();
    multiply_serial(poly1, poly2, res_serial, n);
    double end_serial = omp_get_wtime();


    // --- ΠΑΡΑΛΛΗΛΗ ΕΚΤΕΛΕΣΗ ---
    double start_parallel = omp_get_wtime();
    multiply_parallel(poly1, poly2, res_parallel, n);
    double end_parallel = omp_get_wtime();

    // Εκτύπωση αποτελεσμάτων χρόνου 
    printf("Initialization Time: %f s\n", end_init - start_init);
    printf("Serial Time:   %f s\n", end_serial - start_serial);
    printf("Parallel Time: %f s\n", end_parallel - start_parallel);

    // --- (iv) ΕΠΑΛΗΘΕΥΣΗ ΟΡΘΟΤΗΤΑΣ ---
    int errors = 0;
    for (int i = 0; i <= 2 * n; i++) {
        if (res_serial[i] != res_parallel[i]) {
            errors++;
            // Αν θες να δεις το λάθος, ξε-σχολίασε την επόμενη γραμμή:
            // printf("Error at index %d: Serial=%d, Parallel=%d\n", i, res_serial[i], res_parallel[i]);
        }
    }

    if (errors == 0) {
        printf("Verification: SUCCESS (Serial and Parallel results match)\n");
    } else {
        printf("Verification: FAILED with %d errors\n", errors);
    }

    // Απελευθέρωση μνήμης
    free(poly1);
    free(poly2);
    free(res_serial);

    return 0;
}