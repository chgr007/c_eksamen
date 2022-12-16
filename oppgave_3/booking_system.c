#include "include/linked_list.h"
#include "include/booking_system.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

/*
 *  First I want to convert the strings to lower case. Note that this will only work with ASCII characters and not UTF-8.
 *  As with everything in C, I've got to do most of the work here and iterate over every letter.
 *  Then I compare the two resulting strings
 * */
BOOKING *FindBookingByName(LIST *pList, char *szName) {


    NODE *pCurrentNode = pList->pHead;

    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        char *szNameLower = (char *) malloc(strlen(pBooking->szName) + 1);
        char *szNameToCompareLower = (char *) malloc(strlen(szName) + 1);

        strcpy(szNameLower, pBooking->szName);
        strcpy(szNameToCompareLower, szName);

        int i = 0, j = 0;
        for ( ; i < strlen(szNameLower); i++) {
            szNameLower[i] = tolower(szNameLower[i]);
        }
        for ( ; j < strlen(szNameToCompareLower); j++) {
            szNameToCompareLower[j] = tolower(szNameToCompareLower[j]);
        }

        if (strcmp(szNameLower, szNameToCompareLower) == 0) {
            return pBooking;
        }
        pCurrentNode = pCurrentNode->pNext;
    }
    return NULL;
}

int RemoveNodeFromList(LIST *pList, NODE *pNode) {
    // Check if element is first or last in the list before deleting
    if (pNode->pPrev == NULL) {
        pList->pHead = pNode->pNext;
    } else {
        pNode->pPrev->pNext = pNode->pNext;
    }
    if (pNode->pNext == NULL) {
        pList->pTail = pNode->pPrev;
    } else {
        pNode->pNext->pPrev = pNode->pPrev;
    }
    // Free the node
    FreeBooking((BOOKING *) pNode->pData);
    free(pNode);
    return OK;
}

// Remove bookings from list that are older or equal than iDate + the seconds in the iDays (if equal, it expires in one second..)
int RemoveOldBookings(LIST *pList) {
    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        int iSecondsWhenBookingEnds = pBooking->iDate + (pBooking->numberOfDays * 24 * 60 * 60);

        if (iSecondsWhenBookingEnds< time(NULL)) {
            printf("Removing");
            NODE *nodeToDelete = pCurrentNode->pNext;
            pCurrentNode = nodeToDelete->pNext;
            RemoveNodeFromList(pList, nodeToDelete);
            continue;
        }
        pCurrentNode = pCurrentNode->pNext;
    }
    return OK;
}
int SummerizeBookingForOneDay(LIST *pList, unsigned int iDate) {
    // First convert iDate to struct tm, then find bookings for that day in the pList
    // Then sum up the prices for all bookings for that day
    // Then return the sum
    struct tm *pDate = localtime((time_t *) &iDate);

    printf("Date: %d-%d-%d", pDate->tm_mday, pDate->tm_mon, pDate->tm_year);
}


/* Free memory in the booking struct, then the memory for the struct itself */
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