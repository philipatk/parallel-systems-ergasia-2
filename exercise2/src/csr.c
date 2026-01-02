#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define ZEROSPERCENTAGE 40


void csrInitSerial(int** initialArray, int arraySide, int numOfThreads) {

    int arraySize = arraySide * arraySide;
    int nonZeroValues = (arraySize * (100 - ZEROSPERCENTAGE))/100;

    int* V = (int*)malloc(sizeof(int)* nonZeroValues);
    int* RowIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    int* ColIndex = (int*)malloc(sizeof(int) * (arraySide + 1));

    ColIndex[0] = 0;
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
            RowIndex[k] = j;
    
            k++;
        }
        ColIndex[i + 1] = k;
    }
}

void csrInitParallel(int** initialArray, int arraySide, int numOfThreads) {

    int arraySize = arraySide * arraySide;
    int nonZeroValues = (arraySize * (100 - ZEROSPERCENTAGE))/100;

    int* V = (int*)malloc(sizeof(int)* nonZeroValues);
    int* RowIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    int* ColIndex = (int*)malloc(sizeof(int) * (arraySide + 1));

    ColIndex[0] = 0;
    int k = 0;

    int sum = 0;
    omp_set_num_threads(numOfThreads);

   #pragma omp parallel collapsed(2) shared(sum)
    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {
            if (initialArray[i][j] == 0)
            {
                continue;
            }

            for (int k = 0; k < arraySide + 1 && V[k] == initialArray[i][j]; k++)
            {
                V[k] = initialArray[i][j];
                RowIndex[k] = j;
                sum += 1;
            }
        }
        ColIndex[i + 1] = sum;
    }

    int k = 0;
    #pragma omp parallel collapsed(2)
    for (int i = 0; i < arraySide; i++)
    {
        for (int j = 0; j < arraySide; j++)
        {   
            if (initialArray[i][j] == 0)
            {
                continue;
            }
            
            V[k] = initialArray[i][j];
            k++;
        }
        
    }
    
}