#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/main.h"

int ConvertStringToHex(char *szInputString, char *szOutputString) {
    unsigned long i = 0, ulStrLen = 0;
    int iRetVal = 1;
    /* Loop through each char, print it to hex with sprintfs %X option */
    ulStrLen = strlen(szInputString) - 1;
    char *szTemp = (char *) malloc(24);
    memset(szTemp, 0, 24);

    if (szTemp != NULL) {
        if (szInputString != NULL) {
            for (i = 0; i < ulStrLen; i++) {
                if (szInputString[i] == '\0') {
                    break;
                }
                memset(szTemp, 0, 20);
                sprintf((char *) szTemp, "%02X", szInputString[i]);
                strcat(szOutputString, szTemp);
            }
        } else {
            printf("Input string is NULL\n");
            iRetVal = 0;
        }
        free(szTemp);
    }

    return iRetVal;
}


unsigned long GetFileSize(FILE *pFile) {
    unsigned long ulFileSize;
    fseek(pFile, 0, SEEK_END);
    ulFileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    return ulFileSize;
}



unsigned long GetFileContent(FILE *fdFile, char *szFileContent, unsigned long ulFileSize) {
    unsigned long iReturnVal = 1;

    printf("Allocated %lu bytes for file content\n", ulFileSize);

    fread(szFileContent, sizeof(char), ulFileSize, fdFile);
    // Just to be sure it's null terminated
    szFileContent[ulFileSize] = '\0';
    if (ferror(fdFile) != 0) {
        printf("ERROR reading file\n");
        iReturnVal = 0;
    }

    printf("Read %lu bytes from file\n", ulFileSize);

    return iReturnVal;
}


int ConvertToHex(FILE *fdFile) {
    char *szFileContent;
    size_t lFileBytes;
    int iReturnVal = 0;

    lFileBytes = GetFileSize(fdFile);

    if (lFileBytes > 0) {
        szFileContent = (char *) malloc(lFileBytes * sizeof(char) + 2);
        if (szFileContent == NULL) {
            printf("ERROR allocating space to: %s\n", INPUT_FILE);
        } else {
            memset(szFileContent, 0, lFileBytes * sizeof(char) + 2);
            if (GetFileContent(fdFile, szFileContent, lFileBytes)) {
                // Need a bigger allocation for the hex representation
                char *szAsciiFormatedText = (char *) malloc((lFileBytes * sizeof(char) * 5));
                memset(szAsciiFormatedText, 0, (lFileBytes * sizeof(char) * 5));

                if (szAsciiFormatedText != NULL) {
                    if (ConvertStringToHex(szFileContent, szAsciiFormatedText)) {
                        // Write the hex text to a file
                        FILE *fdOutputFile = fopen(OUTPUT_FILE, "w");
                        if (fdOutputFile == NULL) {
                            printf("ERROR opening file %s\n", OUTPUT_FILE);
                            iReturnVal = 1;
                        }
                        fwrite(szAsciiFormatedText, sizeof(char), strlen(szAsciiFormatedText), fdOutputFile);
                        fclose(fdOutputFile);
                    } else {
                        printf("ERROR converting string to hex\n");
                        iReturnVal = 1;
                    }
                    free(szAsciiFormatedText);
                } else {
                    printf("ERROR allocating memory\n");
                    iReturnVal = 1;
                }
            } else {
                printf("ERROR reading file\n");
                iReturnVal = 1;
            }
        }
        free(szFileContent);
    } else {
        printf("ERROR getting file content\n");
        iReturnVal = 1;
    }
    return iReturnVal;
}

int main(int iArgC, char *pszArgV[]) {
    int iRetVal = 0;
    FILE *fdFile = fopen(INPUT_FILE, "r");
    if (fdFile == NULL) {
        printf("ERROR opening file\n");
        return 0;
    }
    if (ConvertToHex(fdFile)) {
        printf("ERROR converting to hex\n");
        iRetVal = 1;
    }
    fclose(fdFile);
    return iRetVal;
}
