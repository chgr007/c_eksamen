#include "./include/linked_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    LIST *pList = malloc(sizeof(LIST));
    pList->pHead = NULL;
    pList->pTail = NULL;
    NODE *testNode = CreateNode("Test", "123", 8122022, 123, 123.0);
    AddToList(pList, testNode);

    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        printf("Name: %s", pList->pHead->szName);
        pCurrentNode = pCurrentNode->pNext;
    }
    return 0;
}

