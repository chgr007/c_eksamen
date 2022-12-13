#ifndef C_EKSAMEN_PDF_READER_H
#define C_EKSAMEN_PDF_READER_H

typedef unsigned char BYTE;
typedef struct _PDF_BYTE_BUFFER {
    BYTE byBuffer[4096];
    int iFileSize;
    int iaFileBytesAnalyzed[16];
    sem_t semWaitForBuffer;
    sem_t semWaitForProcessing;

} PDF_BYTE_BUFFER;

int PdfReader(char *pszFileName);
int PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer);

#endif //C_EKSAMEN_PDF_READER_H
