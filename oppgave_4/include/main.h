#ifndef C_EKSAMEN_MAIN_H
#define C_EKSAMEN_MAIN_H
#define ERROR 0
#define OK 1

typedef unsigned char BYTE;
typedef struct _PDF_BYTE_BUFFER {
    BYTE byBuffer[4096];
    int iFileSize;
    int iaFileBytesAnalyzed[16];
    int iErrorCode;
    int iDoneAnalyzing;
    sem_t semWaitForBuffer;
    sem_t semWaitForProcessing;
} PDF_BYTE_BUFFER;

void *PdfReader(PDF_BYTE_BUFFER *structPdfByteBuffer);
void *PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer);

#endif //C_EKSAMEN_MAIN_H
