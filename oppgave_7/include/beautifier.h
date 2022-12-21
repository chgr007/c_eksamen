#ifndef C_EKSAMEN_BEAUTIFIER_H
#define C_EKSAMEN_BEAUTIFIER_H
#define OK 1

typedef struct _ANALYZER_STATE {
    char szLoopVariables[128];
    char szIncrementors[128];
    char szConditions[128];
    char szWhiteSpace[32];
    int iWorkingWithLoop;
    int iFoundNestedLoop;
    int iNumOpenBrackets;
    int iNumCloseBrackets;
} ANALYZER_STATE;

/*
 * Takes one string as the first argument and a pointer to a string buffer as the second
 * Replaces all occurrences of TAB with three spaces
 * */
int FormatWhiteSpace(char *pzFormattedString, char *pzFormattedWhiteSpaceString);

/*
 * Runs one time per line. Takes the unformatted line as first argument,
 * The output string as second, and the state of the application as the third argument
 * */
int FormatLine(char *szLineToFormat, char *pszFormattedString, ANALYZER_STATE *pstruAnalyzerState);

/*
 * If everything's gone fine so far, I only need to find the ")"
 * Takes the start of the iteration part of the loop as argument, as well as a buffer for the result
 *
 * Returns NULL on error.
 * */
char *FindIteration(char *szIterationStart, char *szIterator);

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
char *FindLoopVariables(char *pszLoopStart, char *pszLoopVariables);

char *FindLoopCondition(char *pszConditionStart, char *pszLoopCondition);

/*
 * Initializes the program
 * */
int StartFormatting();

/*
 * Takes the start line with the discovered loop as the first parameter,
 * the second param is the offset to the start of the loop, and the third is the application state
 *
 * Copy the white space on the line before the loop
 * Making an assumption that the loop starts and ends on new lines
 * Of course this is not always the case, but I think it's out of scope to
 * handle every possible case.
 * */
int FindWhiteSpaces(char *pszLine, long iLoopStartOffset, ANALYZER_STATE *pState);

/*
 * First argument is the line read, the second is the app state
 *
 * counts and increments in state any brackets found
 *
 * returns OK
 * */
int CheckForBrackets(char *pszLine, ANALYZER_STATE *pState);

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
                    char *szFormattedString);

#endif //C_EKSAMEN_BEAUTIFIER_H
