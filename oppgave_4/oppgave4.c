#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include "include/oppgave4.h"


MYHTTP *ProcessHttpHeader(char *pszHttp) {
    printf("pzHttp: %s\n", pszHttp);
    char *pszPtr;
    // Feil 1, skal være sizeof(MYHTTP)
    MYHTTP *pHttp = (MYHTTP *) malloc(sizeof(MYHTTP));
    if (!pHttp) return NULL;
    memset(pHttp, 0, sizeof(MYHTTP));

    // Sette httpVersion
    pHttp->fHttpVersion = atof(strstr(pszHttp, "/") + 1);
    printf("HTTP version: %.1f\n", pHttp->fHttpVersion);

    // Read from the byte after the http version
    pHttp->iHttpCode = atoi(pszHttp + strlen("HTTP/1.x "));
    if (pHttp->iHttpCode == 200) {
        pHttp->bIsSuccess = true;
    }
    printf("HTTP code: %d\n", pHttp->iHttpCode);


    pszPtr = strstr(pszHttp, "Server");
    if (pszPtr) {
        pszPtr += 6;
        while (!isalpha(pszPtr[0]))pszPtr++;
        strchr(pszPtr, '\n')[0] = 0;
        // Feil 2: skal være strcpy for å få med ev. langt servernavn
        strcpy(pHttp->szServer, pszPtr);
        pszPtr[strlen(pHttp->szServer)] = '\n';
    }
    printf("Server: %s\n", pHttp->szServer);

    pszPtr = strstr(pszHttp, "Content-Type");
    if (pszPtr) {
        pszPtr += 12;
        while (!isalpha(pszPtr[0]))pszPtr++;
        strchr(pszPtr, '\n')[0] = 0;
        strcpy(pHttp->szContentType, pszPtr);
        pszPtr[strlen(pHttp->szContentType)] = '\n';
    }
    printf("Content-Type: %s\n", pHttp->szContentType);

    // Feil 2: Returnerer pointer til starten av "Content-Length", men legger til tegnet '0', som i ASCII er tallet 48.
    pszPtr = strstr(pszHttp, "Content-Length");
    if (pszPtr) {
        pszPtr += 14;
        while (!isdigit(pszPtr[0])) pszPtr++;
        pHttp->iContentLength = atoi(pszPtr);
    }
    printf("Content-Length: %d\n", pHttp->iContentLength);

    return pHttp;
}