#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include"csr.h"
#include"timer.h"

int main(int argc, char* argv[]) {
    
    if (argc != 5)
    {
        printf("\nInvalid Number Of Arguments\n");
        fflush(stdout);
        return 1;
    }
    
    int arraySide = atoi(argv[1]);
    int percentageOfZeros = atoi(argv[2]);
    int numOfIterations = atoi(argv[3]);
    int numOfThreads = atoi(argv[4]);


    if (arraySide < 1)
    {
        printf("\nPlease Enter a Valid Number for Array Side\n");
        fflush(stdout);
        return 1;
    }
    
    int** initialArray = (int**)malloc(sizeof(int*) * arraySide);
    for (int i = 0; i < arraySide; i++)
    {
        initialArray[i] = (int*)malloc(sizeof(int) * arraySide);
    }

    int* vector = (int*)malloc(sizeof(int)* arraySide);

    srand((int)getTime());
    
    for (int i = 0; i < arraySide; i++)
    {      
        for (int j = 0; j < arraySide; j++)
        {
            if (((double)rand()/RAND_MAX) * 100 <= (double)percentageOfZeros)
            {
                initialArray[i][j] = 0;
                continue;
            }
            
            initialArray[i][j] = (rand() % (2 * RAND_MAX + 1)) - RAND_MAX;
        }

        vector[i] = (rand() % (2 * RAND_MAX + 1)) - RAND_MAX;
    }


    int arraySize = arraySide * arraySide;
    int nonZeroValues = (arraySize * (100 - percentageOfZeros))/100;

    CsrBundle* csrSerial = (CsrBundle*)malloc(sizeof(CsrBundle));
    CsrBundle* csrParallel = (CsrBundle*)malloc(sizeof(CsrBundle));

    omp_set_num_threads(numOfThreads);
    
    double csrInitTimeSerial = getTime();
    csrInitSerial(initialArray, csrSerial, arraySide);
    csrInitTimeSerial = getTime() - csrInitTimeSerial;
    
    double csrInitTimeParallel = getTime();
    csrInitParallel(initialArray, csrParallel, arraySide);
    csrInitTimeParallel = getTime() - csrInitTimeParallel;
    
    double csrMulTimeSerial = getTime();
    csrMulSerial(csrSerial, vector, arraySide, numOfIterations);
    csrMulTimeSerial = getTime() - csrMulTimeSerial;
    
    // double csrMulTimeParallel = getTime();
    // csrMulParallel(csrSerial, arraySide, numOfIterations);
    // csrMulTimeParallel = getTime() - csrMulTimeParallel;
    
    
    // double initialArrayMulTimeSerial = getTime();
    // initialArrayMulSerial(initialArray, arraySide, vector);
    // initialArrayMulTimeSerial = getTime() - initialArrayMulTimeSerial;
    
    // double initialArrayMulTimeParallel = getTime();
    // initialArrayMulParallel(initialArray, arraySide, vector);
    // initialArrayMulTimeParallel = getTime() - initialArrayMulTimeParallel;
    
    
    
    // for (int i = 0; i < arraySide; i++)
    // {
        //     for (int j = 0; j < arraySide; j++)
    //     {
    //         if (csrParallel->V != csrSerial->V || csrParallel->RowIndex != csrSerial->RowIndex || csrParallel->ColIndex != csrSerial->ColIndex)
    //         {
        //             printf("\ncsr Arrays Do Not Match\n");
        //             fflush(stdout);
        //             return 1;
        //         }
        
        //     }
        
        // }
        // printf("\ncsr Arrays Match!\n");    
            
    
    for (int i = 0; i < arraySide; i++)
    {
        free(initialArray[i]);
    }
    
    free(initialArray);
    free(vector);
    
    free(csrSerial->V);
    free(csrSerial->ColIndex);
    free(csrSerial->RowIndex);
    free(csrSerial);
    
    free(csrParallel->V);
    free(csrParallel->ColIndex);
    free(csrParallel->RowIndex);
    free(csrParallel);
    
    return 0;
}