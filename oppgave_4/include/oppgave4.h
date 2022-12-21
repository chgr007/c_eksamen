#ifndef C_EKSAMEN_OPPGAVE4_H
#define C_EKSAMEN_OPPGAVE4_H
#include <stdbool.h>

// Mer plass til content type og server
typedef struct _MYHTTP {
    int iHttpCode;
    int iContentLength;
    bool bIsSuccess;
    // Oppskalert fra 16 til 32 byte
    char szServer[32];
    char szContentType[32];
    float fHttpVersion;
} MYHTTP, *PMYHTTP;

MYHTTP *ProcessHttpHeader(char *pszHttp);

#endif //C_EKSAMEN_OPPGAVE4_H