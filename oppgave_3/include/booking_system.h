#ifndef C_EKSAMEN_BOOKING_SYSTEM_H
#define C_EKSAMEN_BOOKING_SYSTEM_H

#pragma pack(1)
typedef struct _BOOKING {
    char szName[256];
    char szRoomNumber[128];
    unsigned long int iDate;
    int numberOfDays;
    long iSize;
    float fPrice;
} BOOKING;
#pragma pack()

/*
 * Takes LIST as parameter.
 *
 * removes bookings from list that
 * are older or equal than iDate + the seconds in the iDays
 * (if equal, it expires in one second..)
 *
 * Returns OK
 * */
int RemoveOldBookings(LIST *pList);

/*
 * Takes a LIST as a parameter and the node to remove.
 *
 * Returns OK if the node was successfully removed, otherwise it returns ERROR.
 * */
int RemoveNodeFromList(LIST *pList, NODE *pNode);

/*
 * Takes a LIST as the first parameter and the name to search for as the second
 *
 * First convert the strings to lower case. Note that this will only work with ASCII characters and not UTF-8.
 * Then I compare the two resulting strings
 * */
BOOKING *FindBookingByName(LIST *pList, char *szName);

/*
 * Takes a BOOKING and a LIST as parameters.
 * Adds the BOOKING to a node in the list.
 *
 * Returns OK if the booking was successfully added to the list, otherwise it returns ERROR.
 *
 * */
int AddBookingToList(BOOKING *pBooking, LIST *pList);

/*
 * Takes a LIST and an unsigned int (time_t) representation of a date as parameters.
 *
 * Checks all dates in the list against the given date.
 *
 * First convert iDate to struct tm, then find bookings for that day in the pList
 * Then sum up the prices for all bookings for that day
 * Then return the sum
 *
 * Returns 0.00 if error or there are no bookings for that day, or they have no price set.
 * it otherwise returns the sum of all prices for that day.
 */
float SummarizeBookingForOneDay(LIST *pList, unsigned int iDate);

/*
 * Takes a LIST as parameter
 *
 * Checks if the list is empty, if the element is the only one, first or last one,
 * Then it links any nodes next to it, accordingly and frees the node
 *
 * returns OK on success, and error on failure
 * */
int RemoveLastBooking(LIST *pList);

/*
 * Takes a LIST as parameter.
 *
 * Handles the menu and calls functions upon user inputs.
 * */
int BookingMenu(LIST *pLIST);

/*
 * Takes a LIST as parameter
 *
 * Takes user input and formats it before creating a BOOKING to add to the list
 *
 * returns OK on success, and ERROR if not.
 * */
int AddBooking(LIST *pList);

/*
 * Takes a LIST as parameter
 * Takes a name from stdin and checks if it's in the list
 *
 * returns OK if found, 0 if not.
 * */
int FindGuestByName(LIST *pList);

/*
 * Takes a LIST as parameter
 *
 * Takes user input from stdin, formats a date from it
 * and calls SummarizeBookingForOneDay() with the date.
 *
 * Prints the result of SummarizeBookingForOneDay()
 *
 * Could be considered void, always returns OK.
 * */
int SumBookingsMenu(LIST *pList);

/*
 * Takes a list as parameter.
 *
 * Prints all reservations it can find.
 *
 * Always returns OK
 * */
int PrintReservationList(LIST *pList);

/*
 * Prints main menu options
 *
 * Always returns OK
 * */
int PrintMainMenu();

/*
 * Takes input from stdin and returns it as an int.
 *
 * returns 0 if invalid input.
 * */
int GetChoice();

/*
 * Takes the customer name, the room number, the date, number of days and price as parameters.
 *
 * Returns a BOOKING struct
 * Returns NULL on error.
 * */
BOOKING *CreateBooking(char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice);


#endif //C_EKSAMEN_BOOKING_SYSTEM_H
