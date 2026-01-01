#include<stdio.h>
#include<stdlib.h>
// #include<omp.h>
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

    for (int i = 0; i < arraySide; i++)
    {
        srand((int)getTime());

        for (int j = 0; j < arraySide; j++)
        {
            if (((double)rand()/RAND_MAX) * 100 <= (double)percentageOfZeros)
            {
                initialArray[i][j] = 0;
                continue;
            }
            
            initialArray[i][j] = rand();
        }

        vector[i] = rand();
    }
    
    double csrInitTimeSerial = getTime();
    csrInitSerial(initialArray, arraySide, numOfThreads);
    csrInitTimeSerial = getTime() - csrInitTimeSerial;
    






    return 0;
}