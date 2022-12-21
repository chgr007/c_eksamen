#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#include "include/pdf_reader.h"


void *PdfReader(PDF_BYTE_BUFFER *structPdfByteBuffer) {
    long lFileBytes;
    char *szFileContent;
    int iBytesRead = 0;

    szFileContent = (char *) malloc(5000);

    // open a file called text_to_read.txt, return 1 if there's an error opening the file
    FILE *fdFile = fopen(structPdfByteBuffer->szFileName, "rb");
    if (fdFile == NULL) {
        printf("Error opening file\n");
        return (void *) ERROR;
    }

    // Rewind the cursor to the end of the file, count it's size and rewind it again. Then allocate
    fseek(fdFile, 0L, SEEK_END);
    lFileBytes = ftell(fdFile);
    fseek(fdFile, 0L, SEEK_SET);


    printf("File size: %ld bytes\n", lFileBytes);

    while (iBytesRead < lFileBytes) {
        printf("Read %d bytes from file\n", iBytesRead);
        /* The file position pointer is incremented by n bytes fread reads */
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
        sem_post(&pdfByteBuffer->semWaitForProcessing);
        for (int i = 0; i < pdfByteBuffer->iNumBytes; i++) {
            BYTE byNumRepresentation = (BYTE) pdfByteBuffer->byBuffer[i];
            pdfByteBuffer->iAnalyzedBytes[byNumRepresentation]++;
        }
    }
    return NULL;
}