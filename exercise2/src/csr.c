#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define ZEROSPERCENTAGE 40


void csrInitSerial(int** initialArray, int arraySide, int numOfThreads) {

    int arraySize = arraySide * arraySide;
    int nonZeroValues = (arraySize * (100 - ZEROSPERCENTAGE))/100;

    int* V = (int*)malloc(sizeof(int)* nonZeroValues);
    int* ColIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    int* RowIndex = (int*)malloc(sizeof(int) * (arraySide + 1));

    RowIndex[0] = 0;
    int k = 0;

    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }   

            V[k] = initialArray[i][j];
            ColIndex[k] = j;
    
            k++;
        }
        RowIndex[i + 1] = k;
    }
}

void csrInitParallel(int** initialArray, int arraySide, int numOfThreads) {

    int arraySize = arraySide * arraySide;
    int nonZeroValues = (arraySize * (100 - ZEROSPERCENTAGE))/100;

    int* V = (int*)malloc(sizeof(int)* nonZeroValues);
    int* ColIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    int* RowIndex = (int*)calloc(arraySide + 1, sizeof(int));


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
        
        RowIndex[i + 1] = count;
    }

    for (int i = 0; i < arraySide; i++)
    {
        RowIndex[i + 1] += RowIndex[i];
    }
    
    int k = 0;

    #pragma omp parallel for private(k)
    for (int i = 0; i < arraySide; i++)
    {
        k = RowIndex[i];

        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }

            V[k] = initialArray[i][j];
            ColIndex[k] = j;
            k++;
        }
    }
}