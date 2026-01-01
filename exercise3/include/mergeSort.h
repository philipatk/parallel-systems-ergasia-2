#pragma once

typedef struct{
    int* A;
    int* B;
    int* C;
} Bundle;

void serialMergeSort(Bundle* arrays,  int left, int right);

void parallelMergeSort(Bundle* arrays, int left, int right);