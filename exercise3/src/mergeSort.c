#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include"mergeSort.h"
#define LIMIT 1000

// Συναρτηση ταξινομησης
void merge(Bundle* arrays, int left, int mid, int right) {

    int bSize = mid - left + 1;
    int cSize = right - mid;

    // δημιουργουμε τους δυο temp πινακες
    for (int i = left; i < left + bSize; i++)
    {
        arrays->B[i] = arrays->A[i];
    }

    for (int i = mid + 1; i < mid + 1 + cSize; i++)
    {
        arrays->C[i] = arrays->A[i];
    }

    int i = 0;
    int j = 0;
    int k = 0;

    // βαζουμε με προτεραιοτητα το μεγαλυτερο στοιχειο αναμεσα στους δυο temp πινακες
    while (i < bSize && j < cSize) {
        if (arrays->B[left + i] >= arrays->C[mid + 1 + j]) {
            arrays->A[left + k] = arrays->B[left + i];
            i++;
        }
        else {
            arrays->A[left + k] = arrays->C[mid + 1 + j];
            j++;
        }
        k++;
    }
    
    // Βαζουμε οσα στοιχεια λειπουν
    while (i < bSize) {
        arrays->A[left + k] = arrays->B[left + i];
        i++;
        k++;
    }

    while (j < cSize) {
        arrays->A[left + k] = arrays->C[mid + 1 + j];
        j++;
        k++;
    }
}

// Συναρτηση διαιρεσης με αναδρομη
void serialMergeSort(Bundle* arrays, int left, int right) {

    if (left == right)
        return;

    int mid = (left + right)/2;

    serialMergeSort(arrays, left, mid);
    serialMergeSort(arrays, mid + 1, right);

    merge(arrays, left, mid, right);
}

// Συναρτηση παρραλληλης ταξινομησης
void parallelMergeSort(Bundle* arrays, int left, int right) {
    
    if (left == right)
        return;

    int mid = (left + right)/2;
    int size = right - left + 1;

    // αυτη τη στιγμη τρεχει μονο ενα νημα. Τα αλλα περιμενουν

    // βαζουμε ενα "task" στο todo list για να το κανει ενα νημα απο αυτα που περιμενουν
    // το shared variable ειναι ετσι ωστε οταν τελειωσει η εντολη και βγει out of bounds να η χαθουν τα δεδομενα. αλλιως το A θα αλλαζε
    // "τοπικα" μοναχα για το ενα νημα που εκανε τις πραξεις.
    #pragma omp task shared(arrays) if(size > LIMIT)
    parallelMergeSort(arrays, left, mid);

    // διαβασε πιο πανω
    #pragma omp task shared(arrays) if(size > LIMIT)
    parallelMergeSort(arrays, mid + 1, right);

    // εδω πρεπει να εχουν τελειωσει τα προηγουμενα δυο βηματα για να γινει το σορταρισμα οποτε βαζουμε ενα barrier
    #pragma omp taskwait
    merge(arrays, left, mid, right);
}