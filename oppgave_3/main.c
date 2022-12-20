#include "include/linked_list.h"
#include "include/booking_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    LIST *pList = malloc(sizeof(LIST));
    pList->pHead = NULL;
    pList->pTail = NULL;
    BookingMenu(pList);

    // Remove everything from the list
    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        NODE *pNextNode = pCurrentNode->pNext;
        free(pCurrentNode->pData);
        free(pCurrentNode);
        pCurrentNode = pNextNode;
    }
    return 0;
}

