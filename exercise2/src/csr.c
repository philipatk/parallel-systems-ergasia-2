#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include"csr.h"


void csrInitSerial(int** initialArray, CsrBundle* csr, int arraySide) {

    csr->RowIndex = (int*)calloc(arraySide + 1, sizeof(int));
    
    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }
            
            csr->RowIndex[i + 1]++;
        }
    }
    
    for (int i = 0; i < arraySide; i++)
    {
        csr->RowIndex[i + 1] += csr->RowIndex[i];
    }
    
    int nonZeroValues = csr->RowIndex[arraySide];
    csr->V = (int*)malloc(sizeof(int)* nonZeroValues);
    csr->ColIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    
    
    int k = 0;
    
    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }   

            csr->V[k] = initialArray[i][j];
            csr->ColIndex[k] = j;
    
            k++;
        }
    }
}

void csrInitParallel(int** initialArray, CsrBundle* csr, int arraySide) {

    csr->RowIndex = (int*)calloc(arraySide + 1, sizeof(int));
    
    #pragma omp parallel for
    for (int i = 0; i < arraySide; i++)
    {
        int count = 0;
        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }
            
            count++;
        }
        
        csr->RowIndex[i + 1] = count;
    }

    for (int i = 0; i < arraySide; i++)
    {
        csr->RowIndex[i + 1] += csr->RowIndex[i];
    }
    
    int nonZeroValues = csr->RowIndex[arraySide];
    csr->V = (int*)malloc(sizeof(int)* nonZeroValues);
    csr->ColIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    

    int k = 0;

    #pragma omp parallel for private(k)
    for (int i = 0; i < arraySide; i++)
    {
        k = csr->RowIndex[i];

        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }

            csr->V[k] = initialArray[i][j];
            csr->ColIndex[k] = j;
            k++;
        }
    }
}

void csrMulSerial(CsrBundle* csr, int arraySide, int* vector, int* vectorSwitch) {
    
    // int sum = 0;
    // for (int i = 0; i < csr->RowIndex[arraySide]; i++)
    // {
    //     sum+= csr->V[i] * vector[];
    // }
    
}


void initialArrayMulSerial(int** initArray, int arraySide, int* vector, int* vectorSwitch) {

    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {
            vectorSwitch[j] += initArray[i][j] * vector[j];
        }
    }
}

void initialArrayMulParallel(int** initArray, int arraySide, int* vector, int* vectorSwitch) {

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {
            vectorSwitch[j] += initArray[i][j] * vector[j];
        }
    }
}