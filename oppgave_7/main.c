
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#define OK 1

int FindLoopCondition(char *pszLoopStart, char *pszLoopCondition) {
    regex_t regexCondition;
    regmatch_t *match;
    char *pcCondStart;
    char *pcCondEnd;
    char *szCondStartExpr = ";[[:space:]]*.+;)";

    return OK;
}

int FormatLine(char *szLineToFormat, char *pszFormattedString) {
    static int iFormattingForLoop = 0;
    char *pcLoopStart = NULL;
    int i;
    size_t ulLineSize = strlen(szLineToFormat);
    regex_t regexLoop;
    regmatch_t match;

    // 1. Sjekk etter løkke, ta vare på pointer til hvor den starter
    // 2. Finn variabelnavn, verdi, og flytt de til en linje over.
    // 3. Finn test statementet i løkken, og spar på det
    // 4. Finn inkrement/dekrement, og spar på det
    // 5. Generer while (test) { ... } og erstatt den gamle løkken med den nye
    // 6. Sett inkrement på en ny linje rett før }
/*
 *
 * typedef struct {
    regoff_t    rm_so;  Byte offset from start of string to start of substring
    regoff_t    rm_eo; Byte offset from start of string of the first character after the end of substring
} regmatch_t;
 */

    printf("FormatLine: %s\n", szLineToFormat);
    int iRegextVal;
    iRegextVal = regcomp(&regexLoop, "for[[:space:]]*\\(.+\\)", REG_EXTENDED);
    iRegextVal = regexec(&regexLoop, szLineToFormat, 1, &match, 0);
    if (iRegextVal == 0) {
        /* Got the pointer to the end of the */
        pcLoopStart = szLineToFormat + match.rm_so;
        iFormattingForLoop = 1;
        char *szLoopCondition = (char *) malloc(sizeof (char) * 512);
        //FindLoopCondition(pcLoopStart, szLoopCondition);
        free(szLoopCondition);

    }

    printf("\n\niRegextVal: %d\n\n", iRegextVal);
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
