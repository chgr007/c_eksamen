#include <stdio.h>
#include <stdlib.h>
#include "include/pdf_reader.h"

int main(int iArgC, char *pszArgV[]) {
    long lFileBytes;
    char *sFileContent;
    FILE *fpFile = fopen("test.pdf", "r");

    PDF_BYTE_BUFFER *pdfByteBuffer = (PDF_BYTE_BUFFER*) malloc(sizeof (PDF_BYTE_BUFFER));

    if (iArgC < 2) {
        printf("Please specify a file to read, ex. %s filename\n", pszArgV[0]);
        return 1;
    }

    // open a file called text_to_read.txt, return 1 if there's an error opening the file
    FILE *fdFile = fopen(pszArgV[1], "r");
    if (fdFile == NULL) {
        printf("Error opening file");
        return 1;
    }

    // Rewind the cursor to the end of the file, count it's size and rewind it again. Then allocate
    // memory for the file content
    fseek(fdFile, 0L, SEEK_END);
    lFileBytes = ftell(fdFile);
    fseek(fdFile, 0L, SEEK_SET);
    sFileContent = (char *) malloc(lFileBytes * sizeof(char *));

    // Read the file content into the allocated memory
    int iReadSuccess = fread(sFileContent, sizeof(char), lFileBytes, fdFile);
    if(ferror(fdFile) && !feof(fdFile)) {
        printf("Error reading the file!\n");
        return 1;
    }
    // Close the file and free the memory
    fclose(fdFile);

    return 0;
}