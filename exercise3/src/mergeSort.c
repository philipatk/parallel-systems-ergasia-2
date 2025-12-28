#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define LIMIT 100

// Συναρτηση ταξινομησης
void merge(int* A, int left, int mid, int right) {

    int bSize = mid - left + 1;
    int cSize = right - mid;

    int *B = (int*)malloc(bSize * sizeof(int));
    int *C = (int*)malloc(cSize * sizeof(int));

    // δημιουργουμε τους δυο temp πινακες
    for (int i = 0; i < bSize; i++)
    {
        B[i] = A[left + i];
    }

    for (int i = 0; i < cSize; i++)
    {
        C[i] = A[mid + 1 + i];
    }

    int i = 0;
    int j = 0;
    int k = left;

    // βαζουμε με προτεραιοτητα το μεγαλυτερο στοιχειο αναμεσα στους δυο temp πινακες
    while (i < bSize && j < cSize) {
        if (B[i] >= C[j]) {
            A[k] = B[i];
            i++;
        }
        else {
            A[k] = C[j];
            j++;
        }
        k++;
    }
    
    // Βαζουμε οσα στοιχεια λειπουν
    while (i < bSize) {
        A[k] = B[i];
        i++;
        k++;
    }

    while (j < cSize) {
        A[k] = C[j];
        j++;
        k++;
    }

    free(B);
    free(C);
}

// Συναρτηση διαιρεσης με αναδρομη
void serialMergeSort(int* A, int left, int right) {

    if (left == right)
        return;

    int mid = (left + right)/2;

    serialMergeSort(A, left, mid);
    serialMergeSort(A, mid + 1, right);

    merge(A, left, mid, right);
}

// Συναρτηση παρραλληλης ταξινομησης
void parallelMergeSort(int* A, int left, int right) {
    
    if (left == right)
        return;

    int mid = (left + right)/2;
    int size = right - left + 1;

    // αυτη τη στιγμη τρεχει μονο ενα νημα. Τα αλλα περιμενουν

    // βαζουμε ενα "task" στο todo list για να το κανει ενα νημα απο αυτα που περιμενουν
    // το shared variable ειναι ετσι ωστε οταν τελειωσει η εντολη και βγει out of bounds να η χαθουν τα δεδομενα. αλλιως το A θα αλλαζε
    // "τοπικα" μοναχα για το ενα νημα που εκανε τις πραξεις.
    #pragma omp task shared(A) if(size > LIMIT)
    parallelMergeSort(A, left, mid);

    // διαβασε πιο πανω
    #pragma omp task shared(A) if(size > LIMIT)
    parallelMergeSort(A, mid + 1, right);

    // εδω πρεπει να εχουν τελειωσει τα προηγουμενα δυο βηματα για να γινει το σορταρισμα οποτε βαζουμε ενα barrier
    #pragma omp taskwait
    merge(A, left, mid, right);
}