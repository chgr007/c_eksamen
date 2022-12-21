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

/*
 * Takes a PDF_BYTE_BUFFER as input.
 *
 * Reads a file and puts the content in the buffer.
 * Signals PdfAnalyzer that the buffer is ready to be processed.
 *
 * sets iDoneReading in the struct to 1 when there's no more bytes to read.
 *
 * returns (void *) OK
 *
 * */
void *PdfReader(PDF_BYTE_BUFFER *structPdfByteBuffer);
/*
 * Takes a PDF_BYTE_BUFFER as input.
 *
 * Reads the buffer and counts the bytes, then increments the index in iAnalyzedBytes that corresponds
 * to that bytes value.
 *
 * Signals PdfReader when done processing the buffer.
 * Runs until iDoneReading is set to 1.
 * */
void *PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer);

#endif //C_EKSAMEN_PDF_READER_H
