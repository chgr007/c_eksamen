#include "include/linked_list.h"
#include "include/booking_system.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Takes a BOOKING and a LIST as parameters.
 * Adds the BOOKING to a node in the list.
 *
 * Returns OK if the booking was successfully added to the list, otherwise it returns ERROR.
 *
 * */
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

/*
 * Takes the customer name, the room number, the date, number of days and price as parameters.
 *
 * Returns a BOOKING struct
 * Returns NULL on error.
 * */
BOOKING *CreateBooking(char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice) {
    // Calculate the size for the BOOKING struct
    int iBookingSize = sizeof(BOOKING)
                       + (strlen(szName) + 1)
                       + (strlen(szRoom) + 1)
                       + (sizeof(int) * 2)
                       + sizeof(float);

    BOOKING *pBooking = (BOOKING *) malloc(iBookingSize);

    if (pBooking != NULL) {
        // Allocate space for the BOOKING struct
        memset(pBooking, 0, iBookingSize);
        pBooking->szName = (char *) malloc(strlen(szName) + 1);
        pBooking->szRoomNumber = (char *) malloc(strlen(szRoom) + 1);

        // set values
        pBooking->iSize = iBookingSize;
        pBooking->fPrice = fPrice;
        pBooking->iDate = (time_t) iDate;
        pBooking->numberOfDays = iDays;
        memcpy(pBooking->szName, szName, strlen(szName) + 1);
        memcpy(pBooking->szRoomNumber, szRoom, strlen(szRoom) + 1);
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
        for (; i < strlen(szNameLower); i++) {
            szNameLower[i] = tolower(szNameLower[i]);
        }
        for (; j < strlen(szNameToCompareLower); j++) {
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

        if (iSecondsWhenBookingEnds < time(NULL)) {
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

float SummarizeBookingForOneDay(LIST *pList, unsigned int iDate) {
    // First convert iDate to struct tm, then find bookings for that day in the pList
    // Then sum up the prices for all bookings for that day
    // Then return the sum
    struct tm *pDate = localtime((time_t *) &iDate);
    float fSumOfBookings = 0.0;

    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        struct tm *pBookingDate = localtime((time_t *) &pBooking->iDate);
        if (pDate->tm_year == pBookingDate->tm_year && pDate->tm_mon == pBookingDate->tm_mon &&
            pDate->tm_mday == pBookingDate->tm_mday) {
            printf("Found booking for that day\n");
            fSumOfBookings += pBooking->fPrice;
        }
        pCurrentNode = pCurrentNode->pNext;
    }

    return fSumOfBookings;
}


/* Free memory in the booking struct, then the memory for the struct itself */
static int FreeBooking(BOOKING *pBooking) {
    if (pBooking != NULL) {
        free(pBooking->szName);
        free(pBooking->szRoomNumber);
        free(pBooking);
        return OK;
    }
    return ERROR;
}

int RemoveLastBooking(LIST *pList) {

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
int AddBooking(LIST *pList) {
    int iRetVal = 0;
    char szName[256];
    char szRoom[56];
    char sNumOfDays[56];
    char sPrice[56];
    int iNumOfDays;
    float fPrice;

    /* Time variables */
    struct tm structTm;
    char sDay[56];
    char sMonth[56];
    char sYear[56];
    char sHour[56];
    char sMinute[56];

    /* Get the booking info */
    printf("Enter name: \n");
    fgets(szName, 255, stdin);
    szName[strlen(szName)] = '\0';
    printf("Enter room number: \n");
    fgets(szRoom, 55, stdin);
    szRoom[strlen(szRoom)] = '\0';
    printf("Enter the price: \n");
    fgets(sPrice, 55, stdin);
    sPrice[strlen(sPrice)] = '\0';
    fPrice = atof(sPrice);
    printf("Enter number of days: \n");
    fgets(sNumOfDays, 55, stdin);
    iNumOfDays = atoi(sNumOfDays);
    printf("Enter day (1-31): \n");
    fgets(sDay, 55, stdin);
    printf("Enter month (1-12): \n");
    fgets(sMonth, 55, stdin);
    printf("Enter year: \n");
    fgets(sYear, 55, stdin);
    printf("Enter hour (0-23): \n");
    fgets(sHour, 55, stdin);
    printf("Enter minute (0-59): \n");
    fgets(sMinute, 55, stdin);

    /* Convert the datetime to a tm struct */
    structTm.tm_mday = atoi(sDay);
    structTm.tm_mon = atoi(sMonth) - 1;
    structTm.tm_year = atoi(sYear) - 1900;
    structTm.tm_hour = atoi(sHour);
    structTm.tm_min = atoi(sMinute);


    /* Create the booking */
    BOOKING *pBooking = (BOOKING *) malloc(sizeof(BOOKING));
    memset(pBooking, 0, sizeof(BOOKING));
    pBooking->szName = (char *) malloc(strlen(szName) + 1);
    strcpy(pBooking->szName, szName);
    pBooking->szRoomNumber = (char *) malloc(strlen(szRoom) + 1);
    strcpy(pBooking->szRoomNumber, szRoom);
    pBooking->numberOfDays = iNumOfDays;
    pBooking->iDate = mktime(&structTm);
    pBooking->fPrice = fPrice;

    /* Add the booking to the list */
    if (AddBookingToList(pBooking, pList)) {
        printf("Booking added\n");
        iRetVal = OK;
    }


    free(pBooking->szName);
    free(pBooking->szRoomNumber);
    free(pBooking);
    return iRetVal;
}

int FindGuestByName(LIST *pList) {
    char szName[256];
    printf("Enter name: \n");
    fgets(szName, 255, stdin);
    szName[strlen(szName)] = '\0';
    BOOKING *pBooking;

    if ((pBooking = FindBookingByName(pList, szName)) != NULL) {
        printf("Found booking\n");
        printf("Name: %s\n", pBooking->szName);
        printf("Room number: %s\n", pBooking->szRoomNumber);
        printf("Price: %f\n", pBooking->fPrice);
        printf("Number of days: %d\n", pBooking->numberOfDays);
        printf("Date: %s\n", ctime((time_t *) &pBooking->iDate));

        return OK;
    } else {
        printf("No booking found\n");
        return 0;
    }
}

int BookingMenu(LIST *pList) {
    int iChoice = 0;
    int iRunning = 1;
    PrintMainMenu();
    iChoice = GetChoice();
    printf("You chose %d\n", iChoice);

    switch (iChoice) {
        case 1:
            AddBooking(pList);
            break;
        case 2:
            RemoveLastBooking(pList);
            break;
        case 3:
            RemoveOldBookings(pList);
            break;
        case 4:
            FindGuestByName(pList);
            break;
        default:
            printf("Invalid choice\n");
            break;
    }
}

int GetChoice() {
    int iChoice = 0;
    char szInput[10];

    fgets(szInput, 10, stdin);
    iChoice = atoi(szInput);

    return iChoice;
}

int PrintMainMenu() {
    printf("1. Add booking\n");
    printf("2. Undo booking\n");
    printf("3. Delete old bookings\n");
    printf("4. Find guest by name\n");
    printf("5. Summarize total value for bookings in a day\n");
    printf("5. Print reservation list\n");
}