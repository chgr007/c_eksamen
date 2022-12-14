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
    puts("HVOR FAEN 1");
    pthread_t threadReader;
    //pthread_t threadAnalyzer;
    puts("HVOR FAEN 2");




    /* Check length of arguments and return values before going any further */
    if (iArgC < 2) {
        printf("Please specify a file to read, ex. %s filename\n", pszArgV[0]);
        return 1;
    }
//    if (structPdfByteBuffer == NULL) {
//        printf("Failed to allocate %lu bytes of memory for pdfByteBuffer\n", sizeof(PDF_BYTE_BUFFER));
//        return 1;
//    }

    //sem_init(&structPdfByteBuffer->semWaitForBuffer, 0, 0);
    //sem_init(&structPdfByteBuffer->semWaitForProcessing, 0, 0);


    /* C89 doesn't have the void pointer, but I have to use it here according to the documentation */
    pthread_create(&threadReader, NULL, (void *) PdfReader, pszArgV[1]);
    //pthread_create(&threadAnalyzer, NULL, (void *) PdfAnalyzer, NULL);
    pthread_join(threadReader, NULL);
    //pthread_join(threadAnalyzer, NULL);
    //free(structPdfByteBuffer);
    return 0;
}

void *PdfReader(char *pszFileName) {
    long lFileBytes;
    char *szFileContent;
    int iBytesRead = 0;
    puts("HVOR FAEN 0");
    /* Ser ut som jeg trenger en pointer pointer! */
    PDF_BYTE_BUFFER *structPdfByteBuffer = malloc(sizeof(PDF_BYTE_BUFFER));
    memset(structPdfByteBuffer, 0, sizeof(PDF_BYTE_BUFFER));
    structPdfByteBuffer->byBuffer = (char *) malloc(4096);
    bzero(structPdfByteBuffer->byBuffer, 4096);
    szFileContent = (char *) malloc(421489212);
    puts("HVOR FAEN 032");

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
    puts("HVOR FAEN 1");

    fseek(fdFile, 0L, SEEK_SET);
    puts("HVOR FAEN 2");

    // Read the file content into the allocated memory. TODO: Check for errors on fread!
    // I could probably just do this in one operation. So that's a nother TODO. Now my plan is to read the file, then send the bytes in chunks.

    fread(szFileContent, 1, 4095, fdFile);
    strcpy(structPdfByteBuffer->byBuffer, "Hei");
    printf("File size: %ld bytes\n", lFileBytes);
    printf("%X", szFileContent[4096]);
//        while (iBytesRead < lFileBytes) {
//            printf("Read %d bytes from file\n", iBytesRead);
//
//            //strcpy(structPdfByteBuffer->byBuffer, szFileContent);
//            iBytesRead += 4096;
//        }



    // Close the file and free the memory
    fclose(fdFile);

    return (void *) OK;
}

void *PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer) {
    sem_wait(&pdfByteBuffer->semWaitForBuffer);
    printf("Inside pdfanalyzer");
    sem_post(&pdfByteBuffer->semWaitForProcessing);
    printf("File size: %d\n", pdfByteBuffer->iFileSize);
    printf("Bytes in buffer: %s\n", pdfByteBuffer->byBuffer);
    return (void *) OK;
}