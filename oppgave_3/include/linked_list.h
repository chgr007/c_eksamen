#ifndef C_EKSAMEN_LINKED_LIST_H
#define C_EKSAMEN_LINKED_LIST_H

#define OK      0
#define ERROR   1

// The list type:

typedef unsigned char BYTE;
// TODO: Trenger man pragma pack på alle structs, eller holder det å skrive det som en blokk?
#pragma pack(1)

typedef struct _NODE {
    struct _NODE *pNext;
    struct _NODE *pPrev;
    int iSize;
    unsigned int iDate;
    int numberOfDays;
    float fPrice;
    char *szName;
    char *szRoomNumber;
} NODE;

#pragma pack()

#pragma pack(1)
typedef struct _LIST {
    NODE *pHead;
    NODE *pTail;
} LIST;
#pragma pack()

NODE *CreateNode (char *szName, char *szRoom, unsigned int iDate, int iDays, float fPrice);
int AddToList (LIST *pList, NODE *pNode);
int RemoveLastElement (LIST *pList);
int RemoveOldElements (LIST *pList, unsigned int iDate);
#endif //C_EKSAMEN_LINKED_LIST_H