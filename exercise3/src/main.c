#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include "mergeSort.h"
#include "timer.h"

int main(int argc, char* argv[]) {

    if (argc != 4)
    {
        printf("\nInvalid Number Of Arguments\n");
        fflush(stdout);
        return 1;
    }
    
    long int arraySize = atol(argv[1]);
    int mode = atoi(argv[2]);
    int threadCount = atoi(argv[3]);

    // Αν δεν εχει δηλωθει 0 ή 1 για τον αλγοριθμο (Σεριακος ή Παραλληλος)
    if (mode != 0 && mode != 1)
    {
        printf("\nMust Input:\n\t0 for Serial\n\t1 for Parallel\n");
    }
    
    // Κανουμε malloc
    int *A = (int*)malloc(sizeof(int)* arraySize);

    // Φτιαχουμε ντετερμινιστικα τον πινακα
    srand(271225);
    for (int i = 0; i < arraySize; i++)
    {
        A[i] = rand();
    }
    
    // Αναλγογως τη λειτουργια μετραμε τον χρονο
    double time;

    if (mode == 0)
    {
        printf("\nRunning Serial Program\n");
        time = getTime();
        serialMergeSort(A, 0 , arraySize - 1);
        time = getTime() - time;
    }
    else if (mode == 1)
    {   
        printf("\nRunning Parallel Program\n");
        time = getTime();

        // θετουμε τον αριθμο των νηματων για τη δουλεια
        omp_set_num_threads(threadCount);
        
        // Απο εδω και κατω αρχιζει ο παραλληλος αλγοριθμος
        #pragma omp parallel
        {
            // δουλευει μονο ενα νημα τα αλλα περιμενουν
            #pragma omp single
            {
                parallelMergeSort(A, 0, arraySize - 1);
            }
        }

        time = getTime() - time;
    }
    
    // Ελεγχουμε αν ειναι ταξινομημενος ο πινακας
    for (int i = 0; i < arraySize - 1; i++)
    {
        if (!(A[i] >= A[i+1]))
        {
            printf("\nArray Is Not Sorted Correctly\n");
            return 1;
        }
        
    }

    printf("\nArray Is Sorted!\n");
    printf("\nTime: %.6f\n", time);

    free(A);
    
    return 0;
}