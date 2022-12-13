#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "include/pdf_reader.h"

/* Entry point for a program which analyzes the byte-composition of a PDF-document.
 * The main function does some crude error checking, and then initializes the threads and semaphores.
 * The two threads in use are the PdfReader and the PdfAnalyzer. The PdfReader reads the PDF-file, while the PdfAnalyzer checks
 * and keeps track of the byte-composition (in HEX).
 * */
int main(int iArgC, char *pszArgV[]) {
    pthread_t threadReader, threadAnalyzer;
    PDF_BYTE_BUFFER *pdfByteBuffer = (PDF_BYTE_BUFFER *) malloc(sizeof(PDF_BYTE_BUFFER));

    /* Check length of arguments and return values before going any further */
    if (iArgC < 2) {
        printf("Please specify a file to read, ex. %s filename\n", pszArgV[0]);
        return 1;
    }
    if (pdfByteBuffer == NULL) {
        printf("Failed to allocate %lu bytes of memory for pdfByteBuffer\n", sizeof(PDF_BYTE_BUFFER));
        return 1;
    }

    if (pthread_create(&threadReader, NULL, (BYTE *) PdfReader, pszArgV[1]) != 0) {
        printf("Error when creating thread\n");
        return 1;
    }
    if (pthread_create(&threadAnalyzer, NULL, (BYTE *) PdfAnalyzer, NULL) != 0) {
        printf("Error when creating thread\n");
        return 1;
    }

    sem_init(&pdfByteBuffer->semWaitForBuffer, 0, 0);
    sem_init(&pdfByteBuffer->semWaitForProcessing, 0, 0);

    PDF_BYTE_BUFFER *structPdfByteBuffer = (PDF_BYTE_BUFFER *) malloc(sizeof(PDF_BYTE_BUFFER));

    /* C89 doesn't have the void pointer, but I have to use it here according to the documentation */
    pthread_create(&threadReader, NULL, (void *) PdfReader, pszArgV[1]);;
    pthread_create(&threadAnalyzer, NULL, (void *) PdfAnalyzer, NULL);

    return 0;
}

void *PdfReader(char *pszFileName, PDF_BYTE_BUFFER *structPdfByteBuffer) {
    long lFileBytes;
    char *szFileContent;
    int iBytesRead = 0;

    // open a file called text_to_read.txt, return 1 if there's an error opening the file
    FILE *fdFile = fopen(pszFileName, "r");
    if (fdFile == NULL) {
        printf("Error opening file");
        return (void *) ERROR;
    }

    // Rewind the cursor to the end of the file, count it's size and rewind it again. Then allocate
    // memory for the file content. TODO: Check for errors on fseek and ftell!
    fseek(fdFile, 0L, SEEK_END);
    lFileBytes = ftell(fdFile);
    fseek(fdFile, 0L, SEEK_SET);
    szFileContent = (char *) malloc(lFileBytes * sizeof(char));
    // Read the file content into the allocated memory. TODO: Check for errors on fread!
    // I could probably just do this in one operation. So that's a nother TODO. Now my plan is to read the file, then send the bytes in chunks.
    fread(szFileContent, sizeof(char), lFileBytes, fdFile);


    int i, iBufferIndex = 0;
    if (!ferror(fdFile) && feof(fdFile)) {
        for (i = 0; i < lFileBytes; i++) {
            /* i % 4095 makes the index not go out of bounds (4096 would make it go off by one!) */
            iBufferIndex = i % 4095;
            structPdfByteBuffer->byBuffer[iBufferIndex] = (BYTE) szFileContent[i];
            iBytesRead++;
            if (i != 0 && iBufferIndex == 0) {
                structPdfByteBuffer->iFileSize = iBytesRead;
                sem_post(&structPdfByteBuffer->semWaitForBuffer);
                sem_wait(&structPdfByteBuffer->semWaitForProcessing);
            }
        }
    } else {
        fclose(fdFile);
        return ERROR;
    }


    // Close the file and free the memory
    fclose(fdFile);

    return (void *) OK;
}

void *PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer) {
    sem_wait(&pdfByteBuffer->semWaitForBuffer);
    sem_post(&pdfByteBuffer->semWaitForProcessing);
    printf("File size: %d\n", pdfByteBuffer->iFileSize);
    printf("Bytes in buffer: %s\n", pdfByteBuffer->byBuffer);
    return (void *) OK;
}