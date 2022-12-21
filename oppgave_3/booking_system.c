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
        if (AddNodeToList(pList, pThis) == OK) {
            printf("Added booking to list\n");
            return OK;
        } else {
            printf("ERROR: Could not add node to list\n");
            return ERROR;
        }
    } else {
        printf("ERROR: Could not allocate memory for the nodes data\n");
        free(pThis);
    }
    return ERROR;
}


BOOKING *CreateBooking(char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice) {
    // Calculate the size for the BOOKING struct

    BOOKING *pBooking = (BOOKING *) malloc(sizeof(BOOKING));

    if (pBooking != NULL) {
        // Allocate space for the BOOKING struct
        memset(pBooking, 0, sizeof(BOOKING));
        printf("INSIDE CREATE BOOKING\n");
        printf("price: %f\n", fPrice);
        printf("Date: %s\n", ctime((time_t*)&iDate));
        // set values
        pBooking->iSize = sizeof(BOOKING);
        pBooking->fPrice = fPrice;
        pBooking->iDate = iDate;
        pBooking->numberOfDays = iDays;
        strcpy(pBooking->szName, szName);
        strcpy(pBooking->szRoomNumber, szRoom);

        printf("Booking created\n");
        printf("Name: %s\n", pBooking->szName);
        printf("Room number: %s\n", pBooking->szRoomNumber);
        printf("Date: %s\n", ctime((time_t*)&pBooking->iDate));
        printf("Number of days: %d\n", pBooking->numberOfDays);
        printf("Price: %f\n", pBooking->fPrice);

    }
    return pBooking;
}

BOOKING *FindBookingByName(LIST *pList, char *szName) {
    char szNameToCompareLower[256] = {0};
    unsigned long int i , j ;

    strcpy(szNameToCompareLower, szName);
    NODE *pCurrentNode = pList->pHead;

    for (j = 0; j < strlen(szNameToCompareLower); j++) {
        szNameToCompareLower[j] = tolower(szNameToCompareLower[j]);
    }

    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        char szNameLower[256];

        strcpy(szNameLower, pBooking->szName);
        for (i = 0; i < strlen(szNameLower); i++) {
            szNameLower[i] = tolower(szNameLower[i]);
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
    if (pNode == NULL) {
        printf("ERROR: Node is NULL\n");
        return ERROR;
    }
    if (pList->pTail == pList->pHead) {
        // Only one node in the list
        pList->pHead = NULL;
        pList->pTail = NULL;
    } else if (pNode->pPrev == NULL) {
        // This element is the first in the list
        pList->pHead = pNode->pNext;
    } else if (pNode->pNext == NULL) {
        // Element is last in the list
        pNode->pPrev->pNext = pNode->pNext;
    } else {
        // This element is somewhere in the middle of the list
        pNode->pPrev->pNext = pNode->pNext;
        pNode->pNext->pPrev = pNode->pPrev;
    }

    // Free the node
    printf("Freeing node\n");
    free(pNode->pData);
    free(pNode);
    return OK;
}

int RemoveOldBookings(LIST *pList) {
    NODE *pCurrentNode = pList->pHead;

    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        printf("Date: %s\n", ctime((time_t*)&pBooking->iDate));
        time_t iSecondsWhenBookingEnds = pBooking->iDate + (pBooking->numberOfDays * 24 * 60 * 60);

        if (iSecondsWhenBookingEnds < time(NULL)) {
            NODE *nodeToDelete = pCurrentNode;

            if (pCurrentNode->pNext != NULL) {
                pCurrentNode = nodeToDelete->pNext;
            } else {
                pCurrentNode = NULL;
            }
            RemoveNodeFromList(pList, nodeToDelete);
            continue;
        }
        pCurrentNode = pCurrentNode->pNext;
    }
    return OK;
}

float SummarizeBookingForOneDay(LIST *pList, unsigned int iDate) {

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

int RemoveLastBooking(LIST *pList) {
    // There is no elements in the list, so just return 0
    if (pList->pHead == NULL) {
        return ERROR;
    }

    // There is only one element int the list. Free it and its data from memory and set the head and tail to NULL
    if (pList->pHead == pList->pTail) {
        BOOKING *pBooking = (BOOKING *) pList->pHead->pData;
        free(pBooking);
        free(pList->pHead);
        pList->pHead = NULL;
        pList->pTail = NULL;
        return OK;
    }

    NODE *pTail = pList->pTail;
    pList->pTail = pTail->pPrev;
    pList->pTail->pNext = NULL;
    BOOKING *pBooking = (BOOKING *) pTail->pData;
    free(pBooking);
    free(pTail);
    return OK;
}

int AddBooking(LIST *pList) {
    int iRetVal = 0;
    char szName[256] = {0};
    char szRoom[128] = {0};
    char sNumOfDays[56] = {0};
    char sPrice[128] = {0};
    int iNumOfDays = 0;
    float fPrice = 0.0;

    /* Time variables */
    time_t iDateNow = time(NULL);
    struct tm *currentTime = localtime((time_t *) &iDateNow);
    struct tm structTm = {0};
    char sDay[56];
    char sMonth[56];
    char sYear[56];
    char sHour[56];
    char sMinute[56];

    /* Get the booking info */
    printf("Enter name: \n");
    fgets(szName, 255, stdin);
    szName[strlen(szName) - 1] = '\0';
    printf("Enter room number: \n");
    fgets(szRoom, 128, stdin);
    szRoom[strlen(szRoom) - 1] = '\0';
    printf("Enter the price: \n");
    fgets(sPrice, 128, stdin);
    sPrice[strlen(sPrice) - 1] = '\0';
    fPrice = atof(sPrice);
    printf("Price: %f\n", fPrice);
    printf("Enter number of days: \n");
    fgets(sNumOfDays, 56, stdin);
    iNumOfDays = atoi(sNumOfDays);
    printf("Enter day (1-31): \n");
    fgets(sDay, 56, stdin);
    printf("Enter month (1-12): \n");
    fgets(sMonth, 56, stdin);
    printf("Enter year: (enter for this year)\n");
    fgets(sYear, 56, stdin);
    printf("Enter hour (0-23): \n");
    fgets(sHour, 56, stdin);
    printf("Enter minute (0-59): \n");
    fgets(sMinute, 56, stdin);

    /* Convert the datetime to a tm struct, also do some format checking */
    if (atoi(sDay) < 1 || atoi(sDay) > 31) {
        printf("Using today\n");
        structTm.tm_mday = currentTime->tm_mday;
    } else {
        structTm.tm_mday = atoi(sDay);
    }
    if (atoi(sMonth) <= 0) {
        printf("Using this month\n");
        structTm.tm_mon = currentTime->tm_mon;
    } else {
        structTm.tm_mon = atoi(sMonth) - 1;
    }

    if (atoi(sYear) < currentTime->tm_year) {
        printf("Using this year\n");
        structTm.tm_year = currentTime->tm_year;
    } else {
        structTm.tm_year = atoi(sYear) - 1900;
    }

    if (atoi(sHour) < 0 || atoi(sHour) > 23) {
        printf("Using this hour\n");
        structTm.tm_hour = currentTime->tm_hour;
    } else {
        structTm.tm_hour = atoi(sHour);
    }

    if (atoi(sMinute) < 0 || atoi(sMinute) > 59) {
        printf("Using this minute\n");
        structTm.tm_min = currentTime->tm_min;
    } else {
        structTm.tm_min = atoi(sMinute);
    }

    /* Create the booking */
    BOOKING *pBooking = CreateBooking(szName, szRoom, mktime(&structTm), iNumOfDays, fPrice);

    /* Add the booking to the list */
    if (AddBookingToList(pBooking, pList) == OK) {
        printf("Booking added\n");
        iRetVal = OK;
    } else {
        printf("Booking not added\n");
        iRetVal = ERROR;
    }

    free(pBooking);
    return iRetVal;
}

int FindGuestByName(LIST *pList) {
    char szName[256];
    printf("Enter name: \n");
    fgets(szName, 255, stdin);
    szName[strlen(szName) - 1] = '\0';
    BOOKING *pBooking;

    if ((pBooking = FindBookingByName(pList, szName)) != NULL) {
        printf("Found booking!: \n");
        printf("Name: %s\n", pBooking->szName);
        printf("Room number: %s\n", pBooking->szRoomNumber);
        printf("Price: %f\n", pBooking->fPrice);
        printf("Number of days: %d\n", pBooking->numberOfDays);
        printf("Date: %s\n", ctime((time_t*) &pBooking->iDate));

        return OK;
    } else {
        printf("No booking found\n");
        return 0;
    }
}

int SumBookingsMenu(LIST *pList) {
    float fTot = 0;
    char szDay[56];
    char szMonth[56];
    char szYear[56];
    struct tm structTm = {0};

    printf("Enter day (1-31): \n");
    fgets(szDay, 56, stdin);
    printf("Enter month (1-12): \n");
    fgets(szMonth, 56, stdin);
    printf("Enter year: \n");
    fgets(szYear, 56, stdin);

    structTm.tm_mday = atoi(szDay);
    structTm.tm_mon = atoi(szMonth) - 1;
    structTm.tm_year = atoi(szYear) - 1900;


    fTot = SummarizeBookingForOneDay(pList, mktime(&structTm));
    printf("Total amount for that day: %.2f\n", fTot);

    return 1;
}

int PrintReservationList(LIST *pList) {
    NODE *pNode = pList->pHead;
    BOOKING *pBooking;
    if (pNode == NULL) {
        printf("No bookings in the system\n");
        return 0;
    }
    while (pNode != NULL) {
        pBooking = (BOOKING *) pNode->pData;
        printf("Name: %s\n", pBooking->szName);
        printf("Room number: %s\n", pBooking->szRoomNumber);
        printf("Price: %.2f\n", pBooking->fPrice);
        printf("Number of days: %d\n", pBooking->numberOfDays);
        printf("Date: %s\n", ctime((time_t *) &pBooking->iDate));
        pNode = pNode->pNext;
    }
    return 1;
}

int BookingMenu(LIST *pList) {
    int iChoice = 0;
    int iRunning = 1;

    do {
        // Clear the screen
        PrintMainMenu();
        iChoice = GetChoice();
        printf("You chose %d\n", iChoice);
        switch (iChoice) {
            case 1:
                system("clear");
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
            case 5:
                SumBookingsMenu(pList);
                break;
            case 6:
                PrintReservationList(pList);
                break;
            case 7:
                iRunning = 0;
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    } while (iRunning);
    return OK;
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
    printf("6. Print reservation list\n");
    printf("7. Exit\n");
    return OK;
}