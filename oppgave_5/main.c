#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "include/pdf_reader.h"

/* Entry point for a program which analyzes the byte-composition of a PDF-document.
 * The main function does some crude error checking, and then initializes the threads and semaphores.
 * The two threads in use are the PdfReader and the PdfAnalyzer. The PdfReader reads the PDF-file, while the PdfAnalyzer checks
 * and keeps track of the byte-composition (in HEX).
 * */
int main(int iArgC, char *pszArgV[]) {
    pthread_t threadReader, threadAnalyzer;

    PDF_BYTE_BUFFER *structPdfByteBuffer = malloc(sizeof(PDF_BYTE_BUFFER));
    memset(structPdfByteBuffer, 0, sizeof(PDF_BYTE_BUFFER));
    memset(structPdfByteBuffer->byBuffer, 0, sizeof(BYTE) * 4096);
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
    for (int i = 0; i < 256; i++) {
        // If there's a count, print it.
        if (structPdfByteBuffer->iAnalyzedBytes[i] > 0) {
            printf("Byte: 0x%02X, Count: %d\n", i, structPdfByteBuffer->iAnalyzedBytes[i]);
        }
    }
    free(structPdfByteBuffer);
    return 0;
}
