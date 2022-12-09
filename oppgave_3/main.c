#include "include/linked_list.h"
#include "include/booking_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    LIST *pList = malloc(sizeof(LIST));
    pList->pHead = NULL;
    pList->pTail = NULL;
    struct tm timeInfo = {0};
    // Antall år siden 1900. Måned fra 0-11. Resten rimelig selvforklarende
    timeInfo.tm_year = 2022 - 1900;
    timeInfo.tm_mon = 12 - 1;
    timeInfo.tm_mday = 8;
    timeInfo.tm_hour = 10 - 1;
    timeInfo.tm_min = 53 - 1;

    unsigned int time = mktime(&timeInfo);

    BOOKING *testBooking = CreateBooking("Test", "1235", time, 1, 123.0);
    AddBookingToList(testBooking, pList);
    AddBookingToList(CreateBooking("Test2", "1235", time - 1000, 1, 123.0), pList);
    NODE *pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        printf("Name: %s\n", pBooking->szName);
        pCurrentNode = pCurrentNode->pNext;
    }
    RemoveOldBookings(pList);
    printf("After removal:\n");
    pCurrentNode = pList->pHead;
    while (pCurrentNode != NULL) {
        BOOKING *pBooking = (BOOKING *) pCurrentNode->pData;
        printf("Name: %s\n", pBooking->szName);
        pCurrentNode = pCurrentNode->pNext;
    }

    BOOKING *pBookingByName = FindBookingByName(pList, "Testers");
    if(pBookingByName != NULL) {
        printf("Found booking by name: %s\n", pBookingByName->szName);
    } else {
        printf("Could not find booking by name\n");
    }
    SummerizeBookingForOneDay(pList, testBooking->iDate);
    return 0;
}

