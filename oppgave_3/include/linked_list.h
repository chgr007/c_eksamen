#ifndef C_EKSAMEN_LINKED_LIST_H
#define C_EKSAMEN_LINKED_LIST_H

#define OK      0
#define ERROR   1

typedef unsigned char BYTE;
#pragma pack(1)

typedef struct _NODE {
    struct _NODE *pNext;
    struct _NODE *pPrev;
    BYTE *pData;
} NODE;

#pragma pack()

#pragma pack(1)
typedef struct _LIST {
    NODE *pHead;
    NODE *pTail;
} LIST;
#pragma pack()

/*
 * Takes data in the form of BYTE, and the size of the data, as params
 *
 * Creates and allocates space for a new node + its content, then returns its pointer.
 * The node itself is made generic, so it uses the struct BYTE (unsigned char) type to hold the data. In newer versions of C we could've used a void pointer instead.
 *
 * Returns the node on success, or null on error
 */
NODE *CreateNode (BYTE *pData, int iSize);

/*
 * Takes a LIST as parameter, and returns the last element in it
 *
 * Loops through every element untill there is no next element.
 * Redundant function, since the list has a tail.
 *
 * Returns the last element in the list, or NULL if the list is empty
 */
NODE *GetLastElement(LIST *pList);
/*
 * Takes a LIST and a NODE as params
 *
 * Checks if the list is empty, otherwise add the node to the end of the list
 *
 * Returns OK
 * */
int AddNodeToList (LIST *pList, NODE *pNode);

#endif //C_EKSAMEN_LINKED_LIST_H