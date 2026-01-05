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
    
    // Παιρνουμε ολα τα ορισματα απο το τερμιναλ
    int arraySide = atoi(argv[1]);
    int percentageOfZeros = atoi(argv[2]);
    int numOfIterations = atoi(argv[3]);
    int numOfThreads = atoi(argv[4]);

    // Αν δηλωθει εσφαλμενο μεγεθος πλευρας πινακα
    if (arraySide < 1)
    {
        printf("\nPlease Enter a Valid Number for Array Side\n");
        fflush(stdout);
        return 1;
    }
    
    // Φτιαχνουμε τον Αρχικο Πινακα
    long long int** initialArray = (long long int**)malloc(sizeof(long long int*) * arraySide);
    for (int i = 0; i < arraySide; i++)
    {
        initialArray[i] = (int*)malloc(sizeof(int) * arraySide);
    }

    // Φτιαχνουμε το διανυσμα + το vecSwitch που βοηθαει στο να κανουμε τραμπα τα δεδομενα για επαναληψη
    int* vector = (int*)malloc(sizeof(int)* arraySide);
    int* vectorSwitch = (int*)malloc(sizeof(int)* arraySide);

    // βαζουμε το seed
    srand((int)getTime());
    
    // Γεμιζουμε το πινακα με τυχαιες τιμες (συνηθως μηδεν) 
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

    // Φτιαχνουμε τη δομη για τον csr πινακα (κοιτα csr.h)
    CsrBundle* csrSerial = (CsrBundle*)malloc(sizeof(CsrBundle));
    CsrBundle* csrParallel = (CsrBundle*)malloc(sizeof(CsrBundle));

    // Θετουμε τον αριθμο νηματων
    omp_set_num_threads(numOfThreads);
    
    // Γεμιζουμε τον csr ΣΕΡΙΑΚΑ
    double csrInitTimeSerial = getTime();
    csrInitSerial(initialArray, csrSerial, arraySide);
    csrInitTimeSerial = getTime() - csrInitTimeSerial;
    
    // Γεμιζουμε τον csr ΠΑΡΑΛΛΗΛΑ
    double csrInitTimeParallel = getTime();
    csrInitParallel(initialArray, csrParallel, arraySide);
    csrInitTimeParallel = getTime() - csrInitTimeParallel;
    
    // Πολλαπλασιαζουμε τον csr με το διανυσμα ΣΕΙΡΙΑΚΑ
    double csrMulTimeSerial = getTime();

    for (int i = 0; i < numOfIterations; i++)
    {
        csrMulSerial(csrSerial, arraySide, vector, vectorSwitch);            
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;
    }
    csrMulTimeSerial = getTime() - csrMulTimeSerial;
    
    // Πολλαπλασιαζουμε τον csr με το διανυσμα ΠΑΡΑΛΛΗΛΑ

    double csrMulTimeParallel = getTime();
    
    for (int i = 0; i < numOfIterations; i++)
    {
        csrMulParallel(csrSerial, arraySide, vector, vectorSwitch);
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;        
    }
    csrMulTimeParallel = getTime() - csrMulTimeParallel;
    
    
    // Πολλαπλασιαζουμε τον αρχικο πινακα ΣΕΙΡΙΑΚΑ
    double initialArrayMulTimeSerial = getTime();

    for (int i = 0; i < numOfIterations; i++)
    {
        initialArrayMulSerial(initialArray, arraySide, vector, vectorSwitch);
        
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;
    }
    initialArrayMulTimeSerial = getTime() - initialArrayMulTimeSerial;
    
    
    // Πολλαπλασιαζουμε τον αρχικο πινακα ΠΑΡΑΛΛΗΛΑ
    double initialArrayMulTimeParallel = getTime();
    
    for (int i = 0; i < numOfIterations; i++)
    {
        initialArrayMulParallel(initialArray, arraySide, vector, vectorSwitch);
        
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;
    }
    initialArrayMulTimeParallel = getTime() - initialArrayMulTimeParallel;
    
    
    
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
            
    
    // ΚΑΝΟΥΜΕ ΦΡΙΜΠΕΣ
    for (int i = 0; i < arraySide; i++)
    {
        free(initialArray[i]);
    }
    
    free(initialArray);
    free(vectorSwitch);
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