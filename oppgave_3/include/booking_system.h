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

int AddBookingToList(BOOKING *pBooking, LIST *pList);

int RemoveLastBooking(LIST *pList);

static int FreeBooking(BOOKING *pBooking);

BOOKING *CreateBooking(char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice);


#endif //C_EKSAMEN_BOOKING_SYSTEM_H
