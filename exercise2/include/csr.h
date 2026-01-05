#pragma once

// Αυτη ειναι η κλασσικη δομη ενος csr πινακα
typedef struct{

    int* V;
    int* ColIndex;
    int* RowIndex;

} CsrBundle;

// Παρακατω οι συναρτησεις που ζητουνται απο την εκφωνηση
void csrInitSerial(int** initialArray, CsrBundle* csr, int arraySide);

void csrInitParallel(int** initialArray, CsrBundle* csr, int arraySide);

void csrMulSerial(CsrBundle* csr, int arraySide, int* vector, int* vectorSwitch);

void csrMulParallel(CsrBundle* csr, int arraySide, int* vector, int* vectorSwitch);

void initialArrayMulSerial(int** initArray, int arraySide, int* vector, int* vectorSwitch);

void initialArrayMulParallel(int** initArray, int arraySide, int* vector, int* vectorSwitch);