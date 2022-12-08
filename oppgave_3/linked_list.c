#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/linked_list.h"


// Creates and allocates space for a new node + its content, then returns its pointer
NODE *CreateNode (char *szName, char *szRoom, int iDate, int iDays, float fPrice)
{
    int iListSize = sizeof(NODE)
            + (strlen(szName) + 1)
            + (strlen(szRoom) + 1)
            + (sizeof(int) * 2)
             + sizeof (float);

    NODE *pThis = malloc (iListSize);
    if (pThis != NULL) {
        memset (pThis, 0, iListSize);
        pThis->pNext = NULL;
        pThis->pPrev = NULL;

        pThis->iSize = iListSize;
        pThis->fPrice = fPrice;
        pThis->iDate = iDate;
        pThis->numberOfDays = iDays;
        pThis->szName = (char *) malloc(strlen(szName) + 1);
        pThis->szRoomNumber = (char *) malloc(strlen(szRoom) + 1);
        memcpy (pThis->szName, szName, strlen(szName) + 1);
        memcpy(pThis->szRoomNumber, szRoom, strlen(szRoom)+ 1);
    }
    return pThis;
}
int RemoveLastElement (LIST *pList) {

    // There is no elements in the list, so just return 0
    if (pList->pHead == NULL) {
        return ERROR;
    }

    // There is only one element int the list. Free it from memory and set the head and tail to NULL
    if (pList->pHead == pList->pTail) {
        free(pList->pHead);
        pList->pHead = NULL;
        pList->pTail = NULL;
        return OK;
    }

    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode->pNext != pList->pTail) {
        pCurrentNode = pCurrentNode->pNext;
    }
    free(pList->pTail);
    pList->pTail = pCurrentNode;
    pList->pTail->pNext = NULL;
    return OK;
}

int AddToList (LIST *pList, NODE *pNode)
{
    if (pList->pHead == NULL) {
        pList->pHead = pNode;
        pList->pTail = pNode;
    } else {
        NODE *pOldTail = pList->pTail;
        pOldTail->pNext = pNode;
        pNode->pPrev = pOldTail;
        pList->pTail = pNode;
    }
    return OK;
}

