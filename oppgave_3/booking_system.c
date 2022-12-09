#include "include/linked_list.h"
#include "include/booking_system.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int AddBookingToList(BOOKING *pBooking, LIST *pList) {
    NODE *pThis = CreateNode((BYTE *) pBooking, sizeof(BOOKING));

    // If the malloc works, we can continue to create the BOOKING struct
    if (pThis != NULL) {
            AddNodeToList(pList, pThis);
            return OK;
        } else {
            printf("ERROR: Could not allocate memory for the nodes data\n");
            free(pThis);
        }
    return ERROR;
}

BOOKING *CreateBooking (char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice) {
    // Calculate the size for the BOOKING struct
    int iBookingSize = sizeof(BOOKING)
                    + (strlen(szName) + 1)
                    + (strlen(szRoom) + 1)
                    + (sizeof(int) * 2)
                    + sizeof (float);

        BOOKING *pBooking = (BOOKING *) malloc (iBookingSize);

        if (pBooking != NULL) {
            // Allocate space for the BOOKING struct
            memset (pBooking, 0, iBookingSize);
            pBooking->szName = (char *) malloc(strlen(szName) + 1);
            pBooking->szRoomNumber = (char *) malloc(strlen(szRoom) + 1);

            // set values
            pBooking->iSize = iBookingSize;
            pBooking->fPrice = fPrice;
            pBooking->iDate = (time_t) iDate;
            pBooking->numberOfDays = iDays;
            memcpy (pBooking->szName, szName, strlen(szName) + 1);
            memcpy(pBooking->szRoomNumber, szRoom, strlen(szRoom)+ 1);
        }
    return pBooking;
}

static int FreeBooking (BOOKING *pBooking) {
    if (pBooking != NULL) {
        free(pBooking->szName);
        free(pBooking->szRoomNumber);
        free(pBooking);
        return OK;
    }
    return ERROR;
}

int RemoveLastBooking (LIST *pList) {

    // There is no elements in the list, so just return 0
    if (pList->pHead == NULL) {
        return ERROR;
    }

    // There is only one element int the list. Free it and its data from memory and set the head and tail to NULL
    if (pList->pHead == pList->pTail) {
        BOOKING *pBooking = (BOOKING *) pList->pHead->pData;
        FreeBooking(pBooking);
        free(pList->pHead);
        pList->pHead = NULL;
        pList->pTail = NULL;
        return OK;
    }

    NODE *pTail = pList->pTail;
    pList->pTail = pTail->pPrev;
    pList->pTail->pNext = NULL;
    BOOKING *pBooking = (BOOKING *) pTail->pData;
    FreeBooking(pBooking);
    free(pTail);
    return OK;
}