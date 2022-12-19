
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

    // Skip preceding whitespace
    while (*pcCondStart == ' ') {
        pcCondStart++;
    }


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

/*
 * If everything's gone fine so far, I only need to find the ")"
 * Takes the start of the iteration part of the loop as argument, as well as a buffer for the result
 *
 * Returns NULL on error.
 * */
char *FindIteration(char *szIterationStart, char *szIterator) {
    char *pcItEnd, *pcItStart = szIterationStart;
    while (*pcItStart == ' ' || *pcItStart == ';') {
        pcItStart++;
    }
    pcItEnd = strstr(pcItStart, ")");
    if (pcItEnd != NULL) {
        strncpy(szIterator, pcItStart, pcItEnd - pcItStart);
        szIterator[pcItEnd - pcItStart] = '\0';
        return pcItEnd;
    }
    return NULL;
}

int FormatLine(char *szLineToFormat, char *pszFormattedString) {
    static int iFormattingForLoop = 0, iNumOfOpenBrackets = 0, iNumOfCloseBrackets = 0;;
    char *pcLoopStart = NULL;
    int i;
    size_t ulLineSize = strlen(szLineToFormat);
    regex_t regexLoop;
    regmatch_t match;
    static char szWhiteSpace[128];
    static char szLoopIterator[512];
    int iRegextVal;
    iRegextVal = regcomp(&regexLoop, "for[[:space:]]*\\(.+\\)", REG_EXTENDED);
    iRegextVal = regexec(&regexLoop, szLineToFormat, 1, &match, 0);
    if (iRegextVal == 0 && iFormattingForLoop == 0) {
        if (strstr(szLineToFormat, "{")) {
            printf("Found a opening bracket");
            iNumOfOpenBrackets++;
        }
        if (strstr(szLineToFormat, "}")) {
            iNumOfCloseBrackets++;
        }
        iFormattingForLoop = 1;
        int iWhiteSpaceOk;
        regex_t regexWhiteSpace;
        regmatch_t matchWhiteSpace;
        char *szWhiteSpaceExpr = "[[:space:]]*for[[:space:]]*\\(.+\\)";

        /*
         * Copy the white space on the line before the loop
         * Making an assumption that the loop starts and ends on new lines
         * Of course this is not always the case, but I think it's out of scope to
         * handle every possible case.
         * */
        iWhiteSpaceOk = regcomp(&regexWhiteSpace, szWhiteSpaceExpr, REG_EXTENDED);
        iWhiteSpaceOk = regexec(&regexWhiteSpace, szLineToFormat, 1, &matchWhiteSpace, 0);


        //  p: szLineToFormat      p + loopStart                        p + ulLineSize
        //          v                   v                                         v
        //          |-------------------|---------------------------------------- |
        //                              for (i ......)                     { .....

        if (iWhiteSpaceOk == 0) {
            bzero(szWhiteSpace, 128);
            char *pcWhiteSpaceStart = matchWhiteSpace.rm_so + szLineToFormat;
            char *pcWhiteSpaceEnd = match.rm_so + szLineToFormat - 1;
            strncpy(szWhiteSpace, pcWhiteSpaceStart, pcWhiteSpaceEnd - pcWhiteSpaceStart);
        }

        /* Got the pointer to the start of loop */
        pcLoopStart = szLineToFormat + match.rm_so;
        iFormattingForLoop = 1;
        char *szLoopCondition = (char *) malloc(sizeof(char) * 512);
        char *szLoopVariables = (char *) malloc(sizeof(char) * 512);
        bzero(szLoopCondition, 512);
        bzero(szLoopVariables, 512);
        char *pcStartOfCondition;
        char *pcStartOfIteration;

        if ((pcStartOfCondition = FindLoopVariables(pcLoopStart, szLoopVariables)) != NULL) {
            printf("Found loop variables: %s\n", szLoopVariables);

            if ((pcStartOfIteration = FindLoopCondition(pcStartOfCondition, szLoopCondition)) != NULL) {
                bzero(szLoopIterator, 512);
                char *pzWhileLoopPattern = "\n%s%s;\n%swhile (%s) {\n";
                char *pzWhileLoop = (char *) malloc(sizeof(char) * ulLineSize);
                bzero(pzWhileLoop, ulLineSize);
                printf("Found loop condition: %s\n", szLoopCondition);
                FindIteration(pcStartOfIteration, (char *) &szLoopIterator);
                printf("Found loop iterator: %s\n", szLoopIterator);


                sprintf(pzWhileLoop, pzWhileLoopPattern, szWhiteSpace, szLoopVariables, szWhiteSpace, szLoopCondition);
                printf("White space: %s\n", szWhiteSpace);
                printf(pzWhileLoopPattern, szWhiteSpace, szLoopVariables, szWhiteSpace, szLoopCondition);
                strcat(pszFormattedString, pzWhileLoop);
                free(pzWhileLoop);
            }

            pcStartOfCondition = NULL;
        }

        regfree(&regexLoop);
        free(szLoopCondition);
        free(szLoopVariables);
        szLoopCondition = NULL;
        szLoopVariables = NULL;
    } else if (iRegextVal == 1 && iFormattingForLoop == 0) {
        strcat(pszFormattedString, szLineToFormat);
    } else if (iRegextVal == 0 && iFormattingForLoop == 1) {
        // Oh shit, nested loops!
        if (strstr(szLineToFormat, "{")) {
            iNumOfOpenBrackets++;
        }
        if (strstr(szLineToFormat, "}")) {
            iNumOfCloseBrackets++;
        }
        strcat(pszFormattedString, szLineToFormat);
    }
    else if (iRegextVal == 1 && iFormattingForLoop == 1) {
        char *pcClosingBracket;
        if (strstr(szLineToFormat, "{")) {
            iNumOfOpenBrackets++;
        }
        if ((pcClosingBracket = strstr(szLineToFormat, "}"))) {
            iNumOfCloseBrackets++;
        }
        printf("Open brackets: %d, Close brackets: %d\n", iNumOfOpenBrackets, iNumOfCloseBrackets);
        if (iNumOfOpenBrackets == iNumOfCloseBrackets) {
            char *szIteratorPattern = "%s   %s;\n%s}\n";
            char *szIterator = (char *) malloc(sizeof(char) * 512);
            printf("Loop iterator: %s\n", szLoopIterator);
            bzero(szIterator, 512);
            sprintf(szIterator, szIteratorPattern, szWhiteSpace, szLoopIterator, szWhiteSpace);
            strcat(pszFormattedString, szIterator);
            iFormattingForLoop = 0;
            iNumOfOpenBrackets = 0;
            iNumOfCloseBrackets = 0;
            pcClosingBracket = NULL;
        } else {
            strcat(pszFormattedString, szLineToFormat);
        }
    }


    return OK;
}

int FormatWhiteSpace(char *pzFormattedString, char *pzFormattedWhiteSpaceString) {
    int i;
    for (i = 0; i < strlen(pzFormattedString); i++) {
        char cCurrentChar = pzFormattedString[i];
        char szCurrentChar[2];
        sprintf(szCurrentChar, "%c", cCurrentChar);
        // Replacing TAB with 3 spaces
        if (cCurrentChar == 9) {
            strcat(pzFormattedWhiteSpaceString, "   ");
            printf("Applying 3 spaces\n");
            continue;
        }
        strcat(pzFormattedWhiteSpaceString, szCurrentChar);
    }
}

int main(int iArgC, char *pszArgV[]) {

    FILE *fpOriginalFile = fopen("testfile.c", "r");

    // get file size
    fseek(fpOriginalFile, 0, SEEK_END);
    long lFileSize = ftell(fpOriginalFile);
    fseek(fpOriginalFile, 0, SEEK_SET);

    char *pszFormattedString = (char *) malloc(lFileSize + (lFileSize / 2));
    bzero(pszFormattedString, lFileSize);
    char *pszFormattedWhiteSpaceString = (char *) malloc(lFileSize + (lFileSize / 2));
    bzero(pszFormattedWhiteSpaceString, lFileSize + (lFileSize / 2));
    char *szLine = NULL;
    size_t ulBufLen = 0;
    ssize_t iReadBytes;

    while ((iReadBytes = getline(&szLine, &ulBufLen, fpOriginalFile)) != -1) {
        FormatLine(szLine, pszFormattedString);
    }
    FormatWhiteSpace(pszFormattedString, pszFormattedWhiteSpaceString);
    printf("Formated string: %s", pszFormattedWhiteSpaceString);
    fclose(fpOriginalFile);
    FILE *fpBeautifiedFile = fopen("testfile_beautified.c", "w");
    fwrite(pszFormattedWhiteSpaceString, 1, strlen(pszFormattedWhiteSpaceString), fpBeautifiedFile);
    free(pszFormattedString);
    fclose(fpBeautifiedFile);
    return 0;
}
