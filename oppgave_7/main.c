
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define OK 1

char *FindLoopCondition(char *pszConditionStart, char *pszLoopCondition) {
    regex_t regexConditionEnd;
    regmatch_t match;
    int iRegOk;
    // The input starts with ";", so here I'll just skip to the next byte
    if (pszConditionStart[0] != ';') {
        printf("ERROR: Unexpected format. Expected string to start with \";\"!\n");
        return NULL;
    }
    char *pcCondStart = pszConditionStart + 1;
    char *pcCondEnd;
    char *szCondEndExpr = ";";
    // Find the end of the condition
    iRegOk = regcomp(&regexConditionEnd, szCondEndExpr, REG_EXTENDED);
    iRegOk = regexec(&regexConditionEnd, pcCondStart, 1, &match, 0);

    if (iRegOk == 0) {
        pcCondEnd = pcCondStart + match.rm_so;
        // Copy the condition to pszLoopCondition
        strncpy(pszLoopCondition, pcCondStart, pcCondEnd - pcCondStart);
        pszLoopCondition[pcCondEnd - pcCondStart] = '\0';
        return pcCondEnd;
    }
    return NULL;
}

/*
 * Takes the pointer to the start of the loop as argument, and sets the
 * Loop variable pointers. Returns the pointer to the end of the variables for further use
 * Returns NULL on error
 *
 * There is some pointer arithmetic going on here:
 * 1: Find the start of the loop variables,
 * 2: Using that pointer, find the end of the loop variables.
 * 3: Calculate the difference in bytes to find the length of the string we want to extract
 * 4: set pszLoopVariables if it was successful
 * */
char *FindLoopVariables(char *pszLoopStart, char *pszLoopVariables) {
    int iRegStartOk, iRegEndOk;
    regex_t regexStartVariables, regexEndVariables;
    regmatch_t matchStart, matchEnd;
    // set this to VarStart match _END_
    char *pcVarStart;
    // set this to VarEnd match _START_
    char *pcVarEnd;
    char *szVarStartExpr = "for[[:space:]]*\\(";
    // use the pointer to the start of variables, as a starting point.
    // Note, loops don't have to initialize variables, so we need to check for empty at some point.
    char *szVarEndExpr = ";";

    iRegStartOk = regcomp(&regexStartVariables, szVarStartExpr, REG_EXTENDED);
    if ((iRegStartOk = regexec(&regexStartVariables, pszLoopStart, 1, &matchStart, 0)) == 0) {
        printf("Found start of variables\n");
        pcVarStart = matchStart.rm_eo + pszLoopStart;
        regfree(&regexStartVariables);
        iRegEndOk = regcomp(&regexEndVariables, szVarEndExpr, REG_EXTENDED);
        iRegEndOk = regexec(&regexEndVariables, pcVarStart, 1, &matchEnd, 0);
        if (iRegEndOk == 0) {
            printf("Found a matching for end of vars\n");
            pcVarEnd = matchEnd.rm_so + pcVarStart;
            strncpy(pszLoopVariables, pcVarStart, pcVarEnd - pcVarStart);
            pszLoopVariables[pcVarEnd - pcVarStart] = '\0';
            regfree(&regexEndVariables);
            return pcVarEnd;
        }
    }
    return NULL;
}

char *FindIteration(char *szIterationStart, char *szIterator) {
    regex_t regexIterator;
    regmatch_t matchIterator;
    int iRegIteratorOk;
    char *pcItEnd, *pcItStart = szIterationStart + 1;
    char *szIteratorExpr = "\\)";
    iRegIteratorOk = regcomp(&regexIterator, szIteratorExpr, REG_EXTENDED);
    iRegIteratorOk = regexec(&regexIterator, szIterationStart, 1, &matchIterator, 0);
    if (iRegIteratorOk == 0) {
        pcItEnd = pcItStart + matchIterator.rm_so - 1;
        strncpy(szIterator, pcItStart, pcItEnd - pcItStart);
        szIterator[pcItEnd - pcItStart] = '\0';
        return pcItEnd;
    }
    return NULL;
}

int FormatLine(char *szLineToFormat, char *pszFormattedString) {
    static int iFormattingForLoop = 0;
    char *pcLoopStart = NULL;
    int i;
    size_t ulLineSize = strlen(szLineToFormat);
    regex_t regexLoop;
    regmatch_t match;

    // 5. Generer while (test) { ... } og erstatt den gamle løkken med den nye
    // 6. Sett inkrement på en ny linje rett før }
/*
 *
 * typedef struct {
    regoff_t    rm_so;  Byte offset from start of string to start of substring
    regoff_t    rm_eo; Byte offset from start of string of the first character after the end of substring
} regmatch_t;
 */

    int iRegextVal;
    iRegextVal = regcomp(&regexLoop, "for[[:space:]]*\\(.+\\)", REG_EXTENDED);
    iRegextVal = regexec(&regexLoop, szLineToFormat, 1, &match, 0);
    if (iRegextVal == 0) {
        /* Got the pointer to the end of the */
        pcLoopStart = szLineToFormat + match.rm_so;
        iFormattingForLoop = 1;
        char *szLoopCondition = (char *) malloc(sizeof(char) * 512);
        char *szLoopVariables = (char *) malloc(sizeof(char) * 512);
        char *szLoopIterator = (char *) malloc(sizeof(char) * 512);
        bzero(szLoopCondition, 512);
        bzero(szLoopVariables, 512);
        char *pcStartOfCondition;
        char *pcStartOfIteration;
        if ((pcStartOfCondition = FindLoopVariables(pcLoopStart, szLoopVariables)) != NULL) {
            printf("Found loop variables: %s\n", szLoopVariables);
            if((pcStartOfIteration = FindLoopCondition(pcStartOfCondition, szLoopCondition)) != NULL) {
                printf("Found loop condition: %s\n", szLoopCondition);
                FindIteration(pcStartOfIteration, szLoopIterator);
                printf("Found loop iterator: %s\n", szLoopIterator);
            }
            pcStartOfCondition = NULL;
        }
        regfree(&regexLoop);
        free(szLoopCondition);
        free(szLoopVariables);
        free(szLoopIterator);
        szLoopIterator = NULL;
        szLoopCondition = NULL;
        szLoopVariables = NULL;
    }

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


    char *szLine = NULL;
    size_t ulBufLen = 0;
    ssize_t iReadBytes;

    while ((iReadBytes = getline(&szLine, &ulBufLen, fpOriginalFile)) != -1) {
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
