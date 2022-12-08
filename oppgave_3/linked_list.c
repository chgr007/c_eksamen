#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/linked_list.h"

int main(int argc, char *argv[]) {
    LIST *pList = malloc(sizeof(LIST));
    pList->pHead = NULL;
    pList->pTail = NULL;
}

static NODE *CreateNode (char *szName, char *szRoom, int iDate, int iDays, float fPrice)
{
    int iListSize = sizeof(NODE)
            + (strlen(szName) + 1)
            + (strlen(szRoom) + 1)
            + (sizeof(int) * 2)
             + sizeof (float);

    NODE *pThis = malloc (iListSize);
    if (pThis != NULL) {
        memset (pThis, 0, iListSize);
        pThis->iSize = iListSize;
        pThis->fPrice = fPrice;
        pThis->iDate = iDate;
        pThis->numberOfDays = iDays;
        memcpy (pThis->szName, szName, strlen(szName) + 1);
        memcpy(pThis->szRoomNumber, szRoom, strlen(szRoom) + 1);
    }
    return pThis;
}
