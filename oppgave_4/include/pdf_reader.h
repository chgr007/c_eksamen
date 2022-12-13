#ifndef C_EKSAMEN_PDF_READER_H
#define C_EKSAMEN_PDF_READER_H

typedef unsigned char BYTE;
typedef struct _PDF_BYTE_BUFFER {
    BYTE byBuffer[4096];
    int iFileSize;
    int iaFileBytesAnalyzed[16];
} PDF_BYTE_BUFFER;
#endif //C_EKSAMEN_PDF_READER_H
