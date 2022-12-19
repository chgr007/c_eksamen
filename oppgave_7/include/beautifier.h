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

int FormatWhiteSpace(char *pzFormattedString, char *pzFormattedWhiteSpaceString);
int FormatLine(char *szLineToFormat, char *pszFormattedString, ANALYZER_STATE *pstruAnalyzerState);
char *FindIteration(char *szIterationStart, char *szIterator);
char *FindLoopVariables(char *pszLoopStart, char *pszLoopVariables);
char *FindLoopCondition(char *pszConditionStart, char *pszLoopCondition);
int StartFormatting();
int FindWhiteSpaces(char *pszLine, long iLoopStartOffset, ANALYZER_STATE *pState);
int CheckForBrackets(char *pszLine, ANALYZER_STATE *pState);
#endif //C_EKSAMEN_BEAUTIFIER_H
