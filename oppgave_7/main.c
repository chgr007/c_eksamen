
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#define OK 1

int FormatLine(char *szLineToFormat, char *pszFormattedString) {
    int iFoundForLoop = 0;
    int iFormattingForLoop = 0;
    char *pcLoopStart = NULL;
    int i;
    size_t ulLineSize = strlen(szLineToFormat);

    // 1. Sjekk etter løkke
    // 2. Finn variabelnavn, verdi, og flytt de til en linje over. Ta vare på pointer

    for (i = 0; i < ulLineSize; i++) {
        char cCurrentChar = szLineToFormat[i];
        char szCurrentChar[2];

        // Replacing TAB with 3 spaces
        if (cCurrentChar == 9) {
            strcat(pszFormattedString, "   ");
            continue;
        }
        sprintf(szCurrentChar, "%c", cCurrentChar);
        strcat(pszFormattedString, szCurrentChar);
    }

    return OK;
}

int main(int iArgC, char *pszArgV[]) {

    FILE *fpOriginalFile = fopen("testfile.c", "r");

    // get file size
    fseek(fpOriginalFile, 0, SEEK_END);
    long lFileSize = ftell(fpOriginalFile);
    fseek(fpOriginalFile, 0, SEEK_SET);

    char *pszFormattedString = (char *) malloc(lFileSize + (lFileSize / 2));
    bzero(pszFormattedString, lFileSize);


    char * szLine = NULL;
    size_t ulBufLen = 0;
    ssize_t iReadBytes;

    while((iReadBytes = getline(&szLine, &ulBufLen, fpOriginalFile)) != -1) {
        FormatLine(szLine, pszFormattedString);
    }

    printf("%s", pszFormattedString);
    fclose(fpOriginalFile);
    FILE *fpBeautifiedFile = fopen("testfile_beautified.c", "w");
    fwrite(pszFormattedString, 1, strlen(pszFormattedString), fpOriginalFile);
    free(pszFormattedString);
    fclose(fpBeautifiedFile);
    return 0;
}
