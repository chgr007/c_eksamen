#include "include/beautifier.h"

#include <unistd.h>
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
        regfree(&regexConditionEnd);
        return pcCondEnd;
    }
    regfree(&regexConditionEnd);
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

/*
 * Takes the start line with the discovered loop as the first parameter,
 * the second param is the offset to the start of the loop, and the third is the application state
 *
 * Copy the white space on the line before the loop
 * Making an assumption that the loop starts and ends on new lines
 * Of course this is not always the case, but I think it's out of scope to
 * handle every possible case.
 * */
int FindWhiteSpaces(char *pszLine, long iLoopStartOffset, ANALYZER_STATE *pState) {
    int iWhiteSpaceOk;
    regex_t regexWhiteSpace;
    regmatch_t matchWhiteSpace;
    char *szWhiteSpaceExpr = "[[:space:]]*for[[:space:]]*\\(.+\\)";

    iWhiteSpaceOk = regcomp(&regexWhiteSpace, szWhiteSpaceExpr, REG_EXTENDED);
    iWhiteSpaceOk = regexec(&regexWhiteSpace, pszLine, 1, &matchWhiteSpace, 0);

    if (iWhiteSpaceOk == 0) {
        char *pcWhiteSpaceStart = matchWhiteSpace.rm_so + pszLine;
        char *pcWhiteSpaceEnd = iLoopStartOffset + pszLine - 1;
        int iSize = pcWhiteSpaceEnd - pcWhiteSpaceStart;
        strncpy(pState->szWhiteSpace, pcWhiteSpaceStart, iSize);
    } else {
        strncpy(pState->szWhiteSpace, "\0", 2);
    }
    regfree(&regexWhiteSpace);
    return OK;
}

int CheckForBrackets(char *pszLine, ANALYZER_STATE *pState) {
    if (strstr(pszLine, "{")) {
        pState->iNumOpenBrackets++;
    }
    if (strstr(pszLine, "}")) {
        pState->iNumCloseBrackets++;
    }
    return OK;
}

/*
 * Runs when a loop is discovered, and analyzes the first line of the for loop.
 *
 * Takes the line as first argument, the byte offset to where the loop starts as the second,
 * and the application state as the third argument
 *
 * (p): szLineToFormat      p + iLoopStartOffset                    p + ulLineSize
 *          v                   v                                         v
 *          |-------------------|---------------------------------------- |
 *                              for (i ......)                     { .....
 */

int HandleLoopMatch(char *szLineToFormat, long iLoopStartOffset, ANALYZER_STATE *pstruAnalyzerState,
                    char *szFormattedString) {
    char *pcStartOfCondition;
    char *pcStartOfIteration;
    char *pcLoopStart = NULL;
    size_t ulLineSize = strlen(szLineToFormat);


    CheckForBrackets(szLineToFormat, pstruAnalyzerState);
    FindWhiteSpaces(szLineToFormat, iLoopStartOffset, pstruAnalyzerState);

    /* Got the pointer to the start of loop */
    pcLoopStart = szLineToFormat + iLoopStartOffset;

    if ((pcStartOfCondition = FindLoopVariables(pcLoopStart, pstruAnalyzerState->szLoopVariables)) != NULL) {
        /* Replace start of loop */
        if ((pcStartOfIteration = FindLoopCondition(pcStartOfCondition, pstruAnalyzerState->szConditions)) !=
            NULL) {
            char *pzWhileLoopPattern = "\n%s%s;\n%swhile (%s) {\n";
            // Make a little extra space for the while loop
            char *pzWhileLoop = (char *) malloc(sizeof(char) * ulLineSize * 2);
            memset(pzWhileLoop, 0, ulLineSize * 2);

            if (FindIteration(pcStartOfIteration, pstruAnalyzerState->szIncrementors)) {
                printf("White space:%s%lu\n", pstruAnalyzerState->szWhiteSpace, strlen(pstruAnalyzerState->szWhiteSpace));
                sprintf(
                        pzWhileLoop, pzWhileLoopPattern,
                        pstruAnalyzerState->szWhiteSpace, pstruAnalyzerState->szLoopVariables,
                        pstruAnalyzerState->szWhiteSpace, pstruAnalyzerState->szConditions
                );

                printf(pzWhileLoopPattern, pstruAnalyzerState->szWhiteSpace, pstruAnalyzerState->szLoopVariables,
                       pstruAnalyzerState->szWhiteSpace, pstruAnalyzerState->szConditions);
                strcat(szFormattedString, pzWhileLoop);
                free(pzWhileLoop);
            } else {
                printf("Error finding iteration\n");
                free(pzWhileLoop);
                return 0;
            }
        } else {
            printf("Error finding loop condition\n");
            return 0;
        }
        pcStartOfCondition = NULL;
    } else {
        printf("Error finding loop variables\n");
        return 0;
    }

    return OK;
}

/*
 * Runs one time per line. Takes the unformatted line as first argument,
 * The output string as second, and the state of the application as the third argument
 * */
int FormatLine(char *szLineToFormat, char *pszFormattedString, ANALYZER_STATE *pstruAnalyzerState) {
    printf("Inside format line\n");

    regex_t regexLoop;
    regmatch_t match;

    int iRegextVal;
    char *szLineBuffer = (char *) malloc(sizeof(char) * strlen(szLineToFormat) + 1);
    strcpy(szLineBuffer, szLineToFormat);
    printf("Line to format: %s", szLineToFormat);
    iRegextVal = regcomp(&regexLoop, "for[[:space:]]*\\(.+\\)", REG_EXTENDED);
    printf("regcomp\n");
    iRegextVal = regexec(&regexLoop, szLineBuffer, 1, &match, 0);
    printf("regex done\n");
    if (iRegextVal == 0 && pstruAnalyzerState->iWorkingWithLoop == 0) {
        pstruAnalyzerState->iWorkingWithLoop = 1;
            printf("Found loop\n");
        if (HandleLoopMatch(szLineToFormat, match.rm_so, pstruAnalyzerState, pszFormattedString)) {
        } else {
            return 0;
        }
    } else if (iRegextVal == 1 && pstruAnalyzerState->iWorkingWithLoop == 0) {
        /* Not working with a loop, just concat the line */
        printf("Not working with loop\n");
        strcat(pszFormattedString, szLineToFormat);
    } else if (iRegextVal == 0 && pstruAnalyzerState->iWorkingWithLoop == 1) {
        /* Got a nested loop. Just concat it, will handle it on next iteration  */
        printf("Got a nested loop\n");
        pstruAnalyzerState->iFoundNestedLoop = 1;
        CheckForBrackets(szLineToFormat, pstruAnalyzerState);
        strcat(pszFormattedString, szLineToFormat);
    } else if (iRegextVal == 1 && pstruAnalyzerState->iWorkingWithLoop == 1) {
        /* Working with the contents of a loop. */
        printf("Working with loop content\n");

        CheckForBrackets(szLineToFormat, pstruAnalyzerState);

        /* Found a matching number of closing and opening brackets. This indicates that the program
         * is on the last line of the loop
         */
        if (pstruAnalyzerState->iNumOpenBrackets == pstruAnalyzerState->iNumCloseBrackets) {
            printf("Found matching number of brackets\n");
            char *szIteratorPattern = "%s   %s;\n%s}\n";
            char *szIterator = (char *) malloc(sizeof(char) * 512);
            printf("Loop iterator: %s\n", pstruAnalyzerState->szIncrementors);
            bzero(szIterator, 512);
            sprintf(szIterator, szIteratorPattern, pstruAnalyzerState->szWhiteSpace, pstruAnalyzerState->szIncrementors,
                    pstruAnalyzerState->szWhiteSpace);
            strcat(pszFormattedString, szIterator);
            pstruAnalyzerState->iWorkingWithLoop = 0;
            pstruAnalyzerState->iNumOpenBrackets = 0;
            pstruAnalyzerState->iNumCloseBrackets = 0;
            free(szIterator);

        } else {
            /* Not on the last line of the loop. Just concat the content */
            printf("Not on the last line of the loop\n");
            strcat(pszFormattedString, szLineToFormat);
        }
    }
    free(szLineBuffer);
    regfree(&regexLoop);
    return OK;
}

/*
 * Takes one string as the first argument and a pointer to a string buffer as the second
 * Replaces all occurrences of TAB with three spaces
 * */
int FormatWhiteSpace(char *pzFormattedString, char *pzFormattedWhiteSpaceString) {
    size_t i;
    for (i = 0; i < strlen(pzFormattedString); i++) {
        char cCurrentChar = pzFormattedString[i];
        char szCurrentChar[3];
        sprintf(szCurrentChar, "%c", cCurrentChar);
        // Replacing TAB with 3 spaces
        if (cCurrentChar == 9) {
            strcat(pzFormattedWhiteSpaceString, "   ");
            printf("Applying 3 spaces\n");
            continue;
        }
        strcat(pzFormattedWhiteSpaceString, szCurrentChar);
    }
    return OK;
}

// Copy a line \n from pszString into pszLine until reaching \0
int ReadOneLineFromString(char **pszString, char *pszLine) {
    char currentChar[2];

    size_t i;
    for (i = 0; i < strlen(*pszString); i++) {
        sprintf(currentChar, "%c", *pszString[i]);
        strcat(pszLine, currentChar);

        if (*pszString[i + 1] == '\n') {
            *pszString += i;
            return OK;
        } else if (*pszString[i + 1] == '\0') {
            *pszString += i;
            return 0;
        }
    }

    return 0;
}

int StartFormatting() {
    long lBufferSize;
    FILE *fpOriginalFile = fopen("testfile.c", "r");
    ANALYZER_STATE *structAnalyzerState = malloc(sizeof(ANALYZER_STATE));
    memset(structAnalyzerState, 0, sizeof(ANALYZER_STATE));

    // get file size
    fseek(fpOriginalFile, 0, SEEK_END);
    long lFileSize = ftell(fpOriginalFile);
    fseek(fpOriginalFile, 0, SEEK_SET);
    lBufferSize = lFileSize + (lFileSize / 2);
    char *pszFormattedString = (char *) malloc(lBufferSize);
    bzero(pszFormattedString, lFileSize);
    char *pszFormattedWhiteSpaceString = (char *) malloc(lBufferSize);
    bzero(pszFormattedWhiteSpaceString, lBufferSize);
    char *szLine = NULL;
    size_t ulBufLen = 2048;
    ssize_t iReadBytes;

    while ((iReadBytes = getline(&szLine, &ulBufLen, fpOriginalFile)) != -1) {
        FormatLine(szLine, pszFormattedString, structAnalyzerState);
    }
    FormatWhiteSpace(pszFormattedString, pszFormattedWhiteSpaceString);

    /*
     * This one feels a bit hacky. If I had more time I'd rewrite the whole program
     * to just read the file in one chunk and iterate over the string
     *
     * A tempfile lets me re-use the existing code without having to write an algorithm for
     * splitting the string by lines, which could be more prone to errors than just using existing functionality
     * in tmpfile().
     * */
    if (structAnalyzerState->iFoundNestedLoop) {
        FILE *tmpFile = tmpfile();
        if (tmpFile != NULL) {
            memset(structAnalyzerState, 0, sizeof(ANALYZER_STATE));
            bzero(pszFormattedString, lBufferSize);
            printf("iFoundNestedLoop: %d\n", structAnalyzerState->iFoundNestedLoop);
            ulBufLen = 0;
            iReadBytes = 0;
            fwrite(pszFormattedWhiteSpaceString, strlen(pszFormattedWhiteSpaceString), 1, tmpFile);
            // Rewind tmpFile
            fseek(tmpFile, 0, SEEK_SET);

            while ((iReadBytes = getline(&szLine, &ulBufLen, tmpFile)) != -1) {
                FormatLine(szLine, pszFormattedString, structAnalyzerState);
                printf("Reading next line\n");
            }
            printf("End of nested loop\n");

            bzero(pszFormattedWhiteSpaceString, lBufferSize);
            strcpy(pszFormattedWhiteSpaceString, pszFormattedString);
        }
        fclose(tmpFile);
        tmpFile = NULL;
    }

    //printf("Formated string: %s", pszFormattedWhiteSpaceString);
    fclose(fpOriginalFile);
    FILE *fpBeautifiedFile = fopen("testfile_beautified.c", "w");
    fwrite(pszFormattedWhiteSpaceString, 1, strlen(pszFormattedWhiteSpaceString), fpBeautifiedFile);
    free(pszFormattedString);
    fclose(fpBeautifiedFile);
    return 0;
}

