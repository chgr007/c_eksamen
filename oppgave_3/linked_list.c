#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "./include/linked_list.h"


NODE *CreateNode (BYTE *pData, int iSize)
{
    NODE *pThis = malloc (sizeof (NODE));
    if (pThis != NULL) {
        memset (pThis, 0, sizeof (NODE));
        pThis->pNext = NULL;
        pThis->pPrev = NULL;
        pThis->pData = malloc (iSize);
        if (pThis->pData != NULL) {
            memcpy (pThis->pData, pData, iSize);
        } else {
            printf ("ERROR: Could not allocate memory for the nodes data\n");
            free (pThis);
            pThis = NULL;
        }
    }
    return pThis;
}

NODE *GetLastElement(LIST *pList)
{
    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode->pNext != NULL) {
        pCurrentNode = pCurrentNode->pNext;
    }
    return pCurrentNode;
}

int AddNodeToList (LIST *pList, NODE *pNode)
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

