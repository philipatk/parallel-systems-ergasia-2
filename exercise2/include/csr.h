#pragma once

typedef struct{

    int* V;
    int* ColIndex;
    int* RowIndex;

} CsrBundle;

void csrInitSerial(int** initialArray, CsrBundle* csr, int arraySide);

void csrInitParallel(int** initialArray, CsrBundle* csr, int arraySide);

void csrMulSerial(CsrBundle* csr, int* vector, int arraySide, int numOfIterations);