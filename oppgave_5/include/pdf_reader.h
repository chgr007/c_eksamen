#ifndef C_EKSAMEN_PDF_READER_H
#define C_EKSAMEN_PDF_READER_H
#define ERROR 0
#define OK 1
#include <semaphore.h>

typedef unsigned char BYTE;
typedef struct _PDF_BYTE_BUFFER {
    BYTE byBuffer[4096];
    int iAnalyzedBytes[256];
    int iNumBytes;
    int iaFileBytesAnalyzed[16];
    int iErrorCode;
    int iDoneReading;
    char szFileName[256];
    sem_t semWaitForBuffer;
    sem_t semWaitForProcessing;
} PDF_BYTE_BUFFER;

void *PdfReader(PDF_BYTE_BUFFER *structPdfByteBuffer);
void *PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer);

#endif //C_EKSAMEN_PDF_READER_H
