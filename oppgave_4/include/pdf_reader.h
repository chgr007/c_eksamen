#ifndef C_EKSAMEN_PDF_READER_H
#define C_EKSAMEN_PDF_READER_H
#define ERROR 0
#define OK 1

typedef unsigned char BYTE;
typedef struct _PDF_BYTE_BUFFER {
    BYTE byBuffer[4096];
    int iFileSize;
    int iaFileBytesAnalyzed[16];
    sem_t semWaitForBuffer;
    sem_t semWaitForProcessing;

} PDF_BYTE_BUFFER;

int PdfReader(char *pszFileName, PDF_BYTE_BUFFER *structPdfByteBuffer);
int PdfAnalyzer(PDF_BYTE_BUFFER *pdfByteBuffer);

#endif //C_EKSAMEN_PDF_READER_H
