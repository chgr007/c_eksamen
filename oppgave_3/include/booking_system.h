#ifndef C_EKSAMEN_BOOKING_SYSTEM_H
#define C_EKSAMEN_BOOKING_SYSTEM_H

#pragma pack(1)
typedef struct _BOOKING {
    char *szName;
    char *szRoomNumber;
    unsigned int iDate;
    int numberOfDays;
    int iSize;
    float fPrice;
} BOOKING;
#pragma pack()

int RemoveOldBookings(LIST *pList);

int RemoveNodeFromList(LIST *pList, NODE *pNode);
BOOKING *FindBookingByName(LIST *pList, char *szName);
int AddBookingToList(BOOKING *pBooking, LIST *pList);
float SummarizeBookingForOneDay(LIST *pList, unsigned int iDate);
int RemoveLastBooking(LIST *pList);
int BookingMenu(LIST *pLIST);
static int FreeBooking(BOOKING *pBooking);
int AddBooking(LIST *pList);
int FindGuestByName(LIST *pList);
/* Prints the main menu */
int PrintMainMenu();

/* Get the users choice from stdin, parse it to int and return it
 *
 * returns 0 on unexpected formats.
 * */
int GetChoice();

int AddBooking();
BOOKING *CreateBooking(char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice);


#endif //C_EKSAMEN_BOOKING_SYSTEM_H
