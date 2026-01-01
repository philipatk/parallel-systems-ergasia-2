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
    
}