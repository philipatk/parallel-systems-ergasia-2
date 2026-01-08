#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
    int** initialArraySerial = (int**)malloc(sizeof(int*) * arraySide);
    int** initialArrayParallel = (int**)malloc(sizeof(int*) * arraySide);

    for (int i = 0; i < arraySide; i++)
    {
        initialArraySerial[i] = (int*)malloc(sizeof(int) * arraySide);
        initialArrayParallel[i] = (int*)malloc(sizeof(int) * arraySide);        
    }

    // Φτιαχνουμε το διανυσμα + το vecSwitch που βοηθαει στο να κανουμε τραμπα τα δεδομενα για επαναληψη
    int* vector = (int*)malloc(sizeof(int)* arraySide);
    int* vectorSwitch = (int*)malloc(sizeof(int)* arraySide);
    // Copys του vector για να εχουμε εναν μπουσουλα
    int* vectorCopy = (int*)malloc(sizeof(int)* arraySide);
    int* vectorResultSerial = (int*)malloc(sizeof(int)* arraySide);

    // βαζουμε το seed
    srand((int)getTime());
    
    // Γεμιζουμε το πινακα με τυχαιες τιμες (συνηθως μηδεν) 
    // Καναμε τις τιμες μικρες με ευρος (-1000, 1000) διοτι θα γινοτατε overflow
    for (int i = 0; i < arraySide; i++)
    {      
        for (int j = 0; j < arraySide; j++)
        {
            if (((double)rand()/RAND_MAX) * 100 <= (double)percentageOfZeros)
            {
                initialArraySerial[i][j] = 0;
                initialArrayParallel[i][j] = 0;
                continue;
            }
            
            initialArraySerial[i][j] = ((rand() % 2001) - 1000);
            initialArrayParallel[i][j] = initialArraySerial[i][j];
        }

        vector[i] = ((rand() % 2001) - 1000);
        vectorCopy[i] = vector[i];
    }

    // Φτιαχνουμε τη δομη για τον csr πινακα (κοιτα csr.h)
    CsrBundle* csrSerial = (CsrBundle*)malloc(sizeof(CsrBundle));
    CsrBundle* csrParallel = (CsrBundle*)malloc(sizeof(CsrBundle));

    // Θετουμε τον αριθμο νηματων
    omp_set_num_threads(numOfThreads);

    printf("\nInitializing csr Array Serial...\n");
    
    // Γεμιζουμε τον csr ΣΕΡΙΑΚΑ
    double csrInitTimeSerial = getTime();
    csrInitSerial(initialArraySerial, csrSerial, arraySide);
    csrInitTimeSerial = getTime() - csrInitTimeSerial;
    

    printf("\nInitializing csr Array Parallel...\n");

    // Γεμιζουμε τον csr ΠΑΡΑΛΛΗΛΑ
    double csrInitTimeParallel = getTime();
    csrInitParallel(initialArrayParallel, csrParallel, arraySide);
    csrInitTimeParallel = getTime() - csrInitTimeParallel;

    printf("\nTesting if Arrays Match...\n");

    // Εξετάζουμε αν οι πινακες csr ειναι ιδιοι
    for (int i = 0; i <= arraySide; i++) 
        {
            if (csrSerial->RowIndex[i] != csrParallel->RowIndex[i]) 
            {
                printf("\ncsr Arrays Do Not Match\n");
                fflush(stdout);
                return 1;
            }
        }

    int nonZeroValues = csrSerial->RowIndex[arraySide]; 

    for (int i = 0; i < nonZeroValues; i++) 
    {
    
        if (csrSerial->V[i] != csrParallel->V[i] || csrSerial->ColIndex[i] != csrParallel->ColIndex[i]) 
        {
            printf("\ncsr Arrays Do Not Match\n");
            fflush(stdout);
            return 1;
        }
    }

    printf("\ncsr Arrays Match!\n");

    printf("\nMultiplying csr Serial...\n");

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

    // Μεταφερουμε τα δεδομενα του σειριακου σε ενα αντιγραφο για συγκριση
    memcpy(vectorResultSerial, vector, sizeof(int) * arraySide);
    
    // Πολλαπλασιαζουμε τον csr με το διανυσμα ΠΑΡΑΛΛΗΛΑ
    memcpy(vector, vectorCopy, sizeof(int) * arraySide);

    printf("\nMultiplying csr Parallel...\n");

    double csrMulTimeParallel = getTime();

    printf("\nChecking If Results Match...\n");
    
    for (int i = 0; i < numOfIterations; i++)
    {
        csrMulParallel(csrParallel, arraySide, vector, vectorSwitch);
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;        
    }
    csrMulTimeParallel = getTime() - csrMulTimeParallel;

    for (int i = 0; i < arraySide; i++) {
        if (vector[i] == vectorResultSerial[i]) {
            continue;
        }

        printf("\nVector Results (Serial - Parallel) Do Not Match!\n");
        fflush(stdout);
        return 1;
    }
    
    printf("\nVector Results (Serial - Parallel) Match!\n");    
    

    printf("\nMultiplying initArray Serial...\n");

    // Πολλαπλασιαζουμε τον αρχικο πινακα ΣΕΙΡΙΑΚΑ

    memcpy(vector, vectorCopy, sizeof(int) * arraySide);

    double initialArrayMulTimeSerial = getTime();

    for (int i = 0; i < numOfIterations; i++)
    {
        initialArrayMulSerial(initialArraySerial, arraySide, vector, vectorSwitch);
        
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;
    }
    initialArrayMulTimeSerial = getTime() - initialArrayMulTimeSerial;
    
    // Μεταφερουμε τα δεδομενα του σειριακου σε ενα αντιγραφο για συγκριση    
    memcpy(vectorResultSerial, vector, sizeof(int) * arraySide);
    
    // Πολλαπλασιαζουμε τον αρχικο πινακα ΠΑΡΑΛΛΗΛΑ

    memcpy(vector, vectorCopy, sizeof(int) * arraySide);

    printf("\nMultiplying initArray Parallel...\n");

    double initialArrayMulTimeParallel = getTime();
    
    for (int i = 0; i < numOfIterations; i++)
    {
        initialArrayMulParallel(initialArrayParallel, arraySide, vector, vectorSwitch);
        
        int* vecTemp = vector;
        vector = vectorSwitch;
        vectorSwitch = vecTemp;
    }
    initialArrayMulTimeParallel = getTime() - initialArrayMulTimeParallel;
    
    for (int i = 0; i < arraySide; i++) {
        if (vector[i] == vectorResultSerial[i]) {
            continue;
        }

        printf("\nVector Results (Serial - Parallel) Do Not Match!\n");
        fflush(stdout);
        return 1;
    }
    
    printf("\nVector Results (Serial - Parallel) Match!\n");    
    
    printf("\ncsr Initialization Serial: %f\n", csrInitTimeSerial);
    printf("\ncsr Initialization Parallel: %f\n", csrInitTimeParallel);
    printf("\ncsr Multiplication Serial: %f\n", csrMulTimeSerial);
    printf("\ncsr Multiplication Parallel: %f\n", csrMulTimeParallel);
    printf("\nInitial Array Multiplication Serial: %f\n", initialArrayMulTimeSerial);
    printf("\nInitial Array Multiplication Parallel: %f\n", initialArrayMulTimeParallel);

    printf("\nFreeing Everything And Exiting...\n");
    printf("\nGoodbye!\n");

    // ΚΑΝΟΥΜΕ ΦΡΙΜΠΕΣ
    for (int i = 0; i < arraySide; i++)
    {
        free(initialArraySerial[i]);
        free(initialArrayParallel[i]);        
    }
    
    free(initialArraySerial);
    free(initialArrayParallel);
    free(vectorResultSerial);
    free(vectorSwitch);
    free(vectorCopy);
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