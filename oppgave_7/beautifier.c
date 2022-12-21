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


int StartFormatting(char *szInputFile) {
    long lBufferSize;
    int iRetVal = OK;
    char szOutputFile[256] = {0};
    char szInputFileBuff[256] = {0};
    strcpy(szInputFileBuff, szInputFile);
    // Get filename without extention
    char *szFileName = strtok(szInputFileBuff, ".");
    // Get file extension
    char *szFileExt = strtok(NULL, ".");
    // Make output file name
    sprintf(szOutputFile, "%s_beautified.%s", szFileName, szFileExt);

    FILE *fpOriginalFile = fopen(szInputFile, "r");
    if (fpOriginalFile == NULL) {
        printf("Error opening file\n");
        return 0;
    }
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
    size_t ulBufLen = 0;
    ssize_t iReadBytes;

    while ((iReadBytes = getline(&szLine, &ulBufLen, fpOriginalFile)) != -1) {
        if (FormatLine(szLine, pszFormattedString, structAnalyzerState) != OK) {
            iRetVal = 0;
            break;
        }
    }
    free(szLine);
    szLine = NULL;
    FormatWhiteSpace(pszFormattedString, pszFormattedWhiteSpaceString);

    /*
     * Checks for discovered nested loops, and handles them.
     *
     * This one feels a bit hacky. If I had more time I'd rewrite the whole program
     * to just read the file in one chunk and iterate over the whole string
     *
     * A tempfile lets me re-use the existing code without having to write an algorithm for
     * splitting the string by lines, which could be more prone to errors than just using the existing functionality
     * in tmpfile().
     * */
    while (structAnalyzerState->iFoundNestedLoop && iRetVal == OK) {
        printf("iFoundNestedLoop: %d\n", structAnalyzerState->iFoundNestedLoop);
        memset(structAnalyzerState, 0, sizeof(ANALYZER_STATE));

        FILE *tmpFile = tmpfile();
        if (tmpFile != NULL) {
            bzero(pszFormattedString, lBufferSize);
            ulBufLen = 0;
            iReadBytes = 0;
            fwrite(pszFormattedWhiteSpaceString, strlen(pszFormattedWhiteSpaceString), 1, tmpFile);
            // Rewind tmpFile
            fseek(tmpFile, 0, SEEK_SET);

            while ((iReadBytes = getline(&szLine, &ulBufLen, tmpFile)) != -1) {
                if (FormatLine(szLine, pszFormattedString, structAnalyzerState) != OK) {
                    iRetVal = 0;
                    break;
                }
            }
            printf("End of nested loop\n");

            bzero(pszFormattedWhiteSpaceString, lBufferSize);
            strcpy(pszFormattedWhiteSpaceString, pszFormattedString);
        } else {
            iRetVal = 0;
        }
        fclose(tmpFile);
        tmpFile = NULL;
    }


    fclose(fpOriginalFile);
    FILE *fpBeautifiedFile = fopen(szOutputFile, "w");
    fwrite(pszFormattedWhiteSpaceString, 1, strlen(pszFormattedWhiteSpaceString), fpBeautifiedFile);
    free(pszFormattedString);
    fclose(fpBeautifiedFile);
    free(structAnalyzerState);
    free(pszFormattedWhiteSpaceString);
    free(szLine);
    return iRetVal;
}

