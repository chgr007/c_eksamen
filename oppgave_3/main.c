#include "./include/linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    LIST *pList = malloc(sizeof(LIST));
    pList->pHead = NULL;
    pList->pTail = NULL;
    struct tm timeInfo = {0};
    // Antall år siden 1900. Resten rimelig selvforklarende
    timeInfo.tm_year = 2022 - 1900;
    timeInfo.tm_mon = 8 - 1;
    timeInfo.tm_mday = 12;
    timeInfo.tm_hour = 12;
    timeInfo.tm_min = 12;

    unsigned int time = mktime(&timeInfo);

    NODE *testNode = CreateNode("Test", "123", time, 123, 123.0);
    AddToList(pList, testNode);

    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        printf("Name: %s", pList->pHead->szName);
        pCurrentNode = pCurrentNode->pNext;
    }
    return 0;
}

