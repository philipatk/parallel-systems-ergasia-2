#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include"csr.h"

// Συναρτηση που αρχικοποιει ΣΕΙΡΙΑΚΑ τον csr πινακα
void csrInitSerial(int** initialArray, CsrBundle* csr, int arraySide) {

    // Θετουμε ολες τις τιμες των κελιων σε 0 (Επειδη μετα κανουμε προσθεση πρεπει οι τιμες να ειναι 0)
    csr->RowIndex = (int*)calloc(arraySide + 1, sizeof(int));
    
    // Φτιαχνουμε το RowIndex που μετραει ποσα μη μηδενικα στοιχεια υπαρχουν συνολικα ΕΩΣ τη γραμμη i
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

    // Απαραιτητη πραξη για να λειτουργει σωστα το RowIndex
    for (int i = 0; i < arraySide; i++)
    {
        csr->RowIndex[i + 1] += csr->RowIndex[i];
    }
    
    int nonZeroValues = csr->RowIndex[arraySide];

    // Φτιαχουμε την υπολοιπη δομη του csr
    csr->V = (int*)malloc(sizeof(int)* nonZeroValues);
    csr->ColIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    
    
    // Γεμιζουμε τους πινακες στοιχειο - στοιχειο παραλειπτοντας τα μηδενικα
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

// Συναρτηση που αρχικοποιει τον csr ΠΑΡΑΛΛΗΛΑ
void csrInitParallel(int** initialArray, CsrBundle* csr, int arraySide) {

    // Θετουμε ολες τις τιμες των κελιων σε 0 (Επειδη μετα κανουμε προσθεση πρεπει οι τιμες να ειναι 0)
    csr->RowIndex = (int*)calloc(arraySide + 1, sizeof(int));
    
    // Αρκει να παραλληλοποιησουμε την εξωτερικη λουπα ετσι ωστε καθε νημα να παρει ενα "κομματι" των i
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

    // Μονο σειριακα
    for (int i = 0; i < arraySide; i++)
    {
        csr->RowIndex[i + 1] += csr->RowIndex[i];
    }
    

    int nonZeroValues = csr->RowIndex[arraySide];

    // Φτιαχνουμε την υπολοιπη δομη του csr
    csr->V = (int*)malloc(sizeof(int)* nonZeroValues);
    csr->ColIndex = (int*)malloc(sizeof(int) * nonZeroValues);
    

    // Eδω ξανα παραλληλοποιουμε ΜΟΝΟ την εξωτερικη λουπα. Με το k να ειναι ιδιωτικο για το καθε νημα.
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

// Συναρτηση που Πολλαπλασιαζει ΣΕΙΡΙΑΚΑ το csr
void csrMulSerial(CsrBundle* csr, int arraySide, int* vector, int* vectorSwitch) {
    
    // Μεσω της δομης του csr εκμεταλευομαστε τους πινακες και υπολογιζουμε τις θεσεις 
    int start;
    int end;

    // Ουσιαστικα παιρνουμε ενα μεμονομενο κομματι (start - end) με τη βοηθεια του Rowindex. 
    // Ετσι ξερουμε ποσα στοιχεια ειναι για τη συγκεκριμενη γραμμη.
    // Επειτα πολλαππλασιαζουμε τη καθε ΣΤΗΛΗ με το στοιχειο του διανυσματος που αντιστοιχει.
    // Το στοιχειο του διανυσματος εχει θεση: Col_Index[j] (j - οστο στοιχειο του V).
    // Φυσικα το Col_Index δειχνει το νουμερο της στηλης του καθε στοιχειου.
    for (int i = 0; i < arraySide; i++)
    {
        start = csr->RowIndex[i];
        end = csr->RowIndex[i + 1];

        int sum = 0;

        for (int j = start; j < end; j++)
        {
            sum += csr->V[j] * vector[csr->ColIndex[j]];
        }
        
        vectorSwitch[i] = sum;
    }   
}

// Συναρτηση που Πολλαπλασιαζει τον αρχικο πινακα ΣΕΙΡΙΑΚΑ
void initialArrayMulSerial(int** initArray, int arraySide, int* vector, int* vectorSwitch) {

    // Μηδενιζουμε το sum καθε γραμμη του πινακα διοτι χρησιμοποιειται κατ επαναληψη και πρεπει να ειναι αδειο
    for (int i = 0; i < arraySide; i++)
    {
        int sum = 0;

        for (int j = 0; j < arraySide; j++)
        {
            sum += initArray[i][j] * vector[j];
        }

        vectorSwitch[i] = sum;
    }
}

void initialArrayMulParallel(int** initArray, int arraySide, int* vector, int* vectorSwitch) {

    // Εδω παραλληλοποιουμε την εξωτερικη λουπα
    // Το sum ειναι ιδιωτικο για καθε νημα αφου το καθενα δημιουργει το δικο του
    #pragma omp parallel for
    for (int i = 0; i < arraySide; i++)
    {
        int sum = 0;

        for (int j = 0; j < arraySide; j++)
        {
            sum += initArray[i][j] * vector[j];
        }
        vectorSwitch[i] = sum;
 
    }
}