#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "include/main.h"

/* Entry point for a program which analyzes the byte-composition of a PDF-document.
 * The main function does some crude error checking, and then initializes the threads and semaphores.
 * The two threads in use are the PdfReader and the PdfAnalyzer. The PdfReader reads the PDF-file, while the PdfAnalyzer checks
 * and keeps track of the byte-composition (in HEX).
 * */
int main(int iArgC, char *pszArgV[]) {
    pthread_t threadReader, threadAnalyzer;

    PDF_BYTE_BUFFER *structPdfByteBuffer = malloc(sizeof(PDF_BYTE_BUFFER));
    memset(structPdfByteBuffer, 0, sizeof(PDF_BYTE_BUFFER));
    bzero(structPdfByteBuffer->byBuffer, 4096);
    structPdfByteBuffer->iDoneReading = 0;


    /* Check length of arguments and return values before going any further */
    if (iArgC < 2) {
        printf("no file to read specified, ex. %s filename\n", pszArgV[0]);
        printf("Defaulting to: \"PG3401-Hjemmeeksamen-14dager-H22.pdf\" \n");
        strcpy(structPdfByteBuffer->szFileName, "PG3401-Hjemmeeksamen-14dager-H22.pdf");
    } else {
        strcpy(structPdfByteBuffer->szFileName, pszArgV[1]);
    }
    if (structPdfByteBuffer == NULL) {
        printf("Failed to allocate %lu bytes of memory for pdfByteBuffer\n", sizeof(PDF_BYTE_BUFFER));
        return 1;
    }

    sem_init(&structPdfByteBuffer->semWaitForBuffer, 0, 0);
    sem_init(&structPdfByteBuffer->semWaitForProcessing, 0, 0);

    /* C89 doesn't have the void pointer, but I have to use it here according to the documentation */
    pthread_create(&threadReader, NULL, (void *) PdfReader, structPdfByteBuffer);
    pthread_create(&threadAnalyzer, NULL, (void *) PdfAnalyzer, structPdfByteBuffer);

    pthread_join(threadAnalyzer, NULL);
    pthread_join(threadReader, NULL);
    sem_destroy(&structPdfByteBuffer->semWaitForBuffer);
    sem_destroy(&structPdfByteBuffer->semWaitForProcessing);
    printf("Joning threads\n");
    for (int i = 0; i < 255; i++) {
        if (structPdfByteBuffer->iAnalyzedBytes[i] > 0) {
            printf("Byte: 0x%02X, Count: %d\n", i, structPdfByteBuffer->iAnalyzedBytes[i]);
        }
    }
    free(structPdfByteBuffer);
    return 0;
}

void *PdfReader(PDF_BYTE_BUFFER *structPdfByteBuffer) {
    long lFileBytes;
    char *szFileContent;
    int iBytesRead = 0;

    szFileContent = (char *) malloc(5000);

    // open a file called text_to_read.txt, return 1 if there's an error opening the file
    FILE *fdFile = fopen(structPdfByteBuffer->szFileName, "r");
    if (fdFile == NULL) {
        printf("Error opening file\n");
        return (void *) ERROR;
    }

    // Rewind the cursor to the end of the file, count it's size and rewind it again. Then allocate
    // memory for the file content. TODO: Check for errors on fseek and ftell!
    fseek(fdFile, 0L, SEEK_END);
    lFileBytes = ftell(fdFile);
    fseek(fdFile, 0L, SEEK_SET);

    // Read the file content into the allocated memory. TODO: Check for errors on fread!
    // I could probably just do this in one operation. So that's a nother TODO. Now my plan is to read the file, then send the bytes in chunks.

    printf("File size: %ld bytes\n", lFileBytes);

    while (iBytesRead < lFileBytes) {
        printf("Read %d bytes from file\n", iBytesRead);
        /* The file position pointer is incremented by n bytes fread reads
         * TODO: Kan bare fread-e rett i byBuffer her vel?
         * */
        int iChunk = fread(szFileContent, 1, 4096, fdFile);

        printf("Read: %d bytes\n", iChunk);
        memcpy(structPdfByteBuffer->byBuffer, szFileContent, iChunk);
        structPdfByteBuffer->iNumBytes = iChunk;

        iBytesRead += iChunk;
        if (ferror(fdFile)) {
            printf("Error reading file\n");
            return (void *) ERROR;
        }

        sem_post(&structPdfByteBuffer->semWaitForBuffer);
        if (feof(fdFile)) {
            structPdfByteBuffer->iDoneReading = 1;
        } else {
            sem_wait(&structPdfByteBuffer->semWaitForProcessing);
        }
        printf("Inside pdfReader\n");
    }

    // Close the file and free the memory from the buffer
    fclose(fdFile);
    free(szFileContent);

    return (void *) OK;
}

void *PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer) {
    while (pdfByteBuffer->iDoneReading == 0) {
        sem_wait(&pdfByteBuffer->semWaitForBuffer);
        printf("Inside pdfanalyzer\n");
        sem_post(&pdfByteBuffer->semWaitForProcessing);
        for (int i = 0; i < pdfByteBuffer->iNumBytes; i++) {
            int iIntRepresentation = pdfByteBuffer->byBuffer[i];
            pdfByteBuffer->iAnalyzedBytes[iIntRepresentation]++;
        }
        printf("\nbyte chunk size: %d\n", pdfByteBuffer->iNumBytes);
    }
    return NULL;
}