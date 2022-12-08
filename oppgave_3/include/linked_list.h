#ifndef C_EKSAMEN_LINKED_LIST_H
#define C_EKSAMEN_LINKED_LIST_H

#define OK      0
#define ERROR   1

// The list type:

typedef unsigned char BYTE;
// TODO: Trenger man pragma pack på alle structs, eller holder det å skrive det som en blokk?
#pragma pack(1)

typedef struct _NODE {
    struct _LIST *pNext;
    struct _LIST *pPrev;
    int iSize;
    int iDate;
    int numberOfDays;
    float fPrice;
    char *szName;
    char *szRoomNumber;
} NODE;

#pragma pack()

#pragma pack(1)
typedef struct _LIST {
    struct NODE *pHead;
    struct NODE *pTail;
} LIST;
#pragma pack()

#endif //C_EKSAMEN_LINKED_LIST_H
