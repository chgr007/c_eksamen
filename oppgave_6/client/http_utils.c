#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

#include "include/http_utils.h"


int SendMessage(int sockFd, struct URL *structUrl) {
    char szRequestPattern[] = "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n";
    char *szRequest = (char *) malloc(strlen(szRequestPattern) + strlen(structUrl->szPath) + strlen(structUrl->szHost));
    /* Format the request and send it */
    sprintf(szRequest, szRequestPattern, structUrl->szPath, structUrl->szHost);
    int n = send(sockFd, szRequest, strlen(szRequest), 0);
    if (n < 0) {
        printf("ERROR writing to socket\n");
        return ERROR;
    }
    printf("Request sent: %s\n", szRequest);
    free(szRequest);
    return OK;
}

struct HTTP_RESPONSE *GetHeaders(int sockFd) {
    char *szLineBuffer, *szToken;
    struct HTTP_RESPONSE *structHttpResponse;
    szLineBuffer = (char *) malloc(sizeof(char) * 256);
    structHttpResponse = malloc(sizeof(struct HTTP_RESPONSE));
    memset(structHttpResponse, 0, sizeof(struct HTTP_RESPONSE));
    memset(szLineBuffer, 0, 256);

    /* Get the first line / status line */
    ReadLine(sockFd, szLineBuffer);
    szToken = strtok(szLineBuffer, " ");
    strcpy(structHttpResponse->szVersion, szToken);
    szToken = strtok(NULL, " ");
    structHttpResponse->iStatusCode = atoi(szToken);
    printf("Status code: %d\n", structHttpResponse->iStatusCode);
    /* Check if the first line is formatted as expected, could be more elaborated (could use RegEx if C has support for it) */
    if (structHttpResponse->iStatusCode == 0) {
        printf("ERROR: Unexpected format\n");
        return NULL;
    }
    szToken = strtok(NULL, " ");
    strcpy(structHttpResponse->szStatusMessage, szToken);

    /* Get header fields */
    SplitHeaders(szLineBuffer, structHttpResponse, sockFd);

    free(szLineBuffer);
    szLineBuffer = NULL;
    return structHttpResponse;
}

int GetPayload(struct HTTP_RESPONSE *structHttpResponse, int sockFd) {
    int iContentLength = structHttpResponse->iContentLength;
    char *szPayloadBuffer = (char *) malloc(sizeof(char) * 1500);
    if (iContentLength <= 0) {
        printf("ERROR: Unexpected length on payload: %d\n", iContentLength);
        return ERROR;
    }
    structHttpResponse->szPayload = (char *) malloc(sizeof(char) * iContentLength);
    memset(structHttpResponse->szPayload, 0, iContentLength);
    // While there is data to read, read it
    printf("Content-length: %d\n", iContentLength);
    long mBytes = 0, totBytes = 0;
    do {
        memset(szPayloadBuffer, 0, 1500);
        mBytes = recv(sockFd, szPayloadBuffer, 1500, MSG_BATCH);

        if (mBytes < 0) {
            printf("mBytes: %ld", mBytes);
            printf("ERROR: Error on socket\n");
            return ERROR;
        } else {
            memcpy(structHttpResponse->szPayload + totBytes, szPayloadBuffer, (size_t) mBytes);
            printf("Received %ld bytes of payload\n", mBytes);
            totBytes += mBytes;
        }
    } while (mBytes > 0);

    if (totBytes != iContentLength) {
        printf("Unexpected length on payload: %ld\n", totBytes);
    }
    free(szPayloadBuffer);
    return totBytes;
}

int SavePayload(struct HTTP_RESPONSE *structHttpResponse, char *szFilename, int iNumBytes) {
    FILE *fpFile;

    if (strcmp(structHttpResponse->szContentType, "text/html;") == 0) {
        fpFile = fopen(szFilename, "w");
    } else if (strcmp(structHttpResponse->szContentType, "text/plain;") == 0) {
        fpFile = fopen(szFilename, "w");
    } else if (strcmp(structHttpResponse->szContentType, "application/octet-stream") == 0) {
        fpFile = fopen(szFilename, "wb");
    } else if (strcmp(structHttpResponse->szContentType, "image/jpeg") == 0) {
        fpFile = fopen(szFilename, "wb");
    } else {
        printf("ERROR: Unknown content type: %s", structHttpResponse->szContentType);
        return ERROR;
    }
    if (fpFile == NULL) {
        printf("ERROR: Could not open file for writing\n");
        return ERROR;
    }
    fwrite(structHttpResponse->szPayload, 1, iNumBytes, fpFile);
    fclose(fpFile);
    return OK;
}

int SplitHeaders(char *szLineBuffer, struct HTTP_RESPONSE *structHttpResponse, int sockFd) {
    char *szToken;
    bzero(szLineBuffer, 256);

    /* Loops through one by one line of the response, until it receives a blank NULL terminated line */
    ReadLine(sockFd, szLineBuffer);
    while (strcmp(szLineBuffer, "\0") != 0) {
        szToken = strtok(szLineBuffer, ": ");

        if (strcmp(szToken, "Server") == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpResponse->szServer, szToken);
        } else if (strcmp(szToken, "Content-Type") == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpResponse->szContentType, szToken);
        } else if (strcmp(szToken, "Content-Length") == 0) {
            szToken = strtok(NULL, ": ");
            structHttpResponse->iContentLength = atoi(szToken);
        }
        bzero(szLineBuffer, 256);
        ReadLine(sockFd, szLineBuffer);
    }
    return OK;
}

int ReadLine(int sockFd, char *szLineBuffer) {
    char *szReceivedMessageBuffer = (char *) malloc(sizeof(char) * 2);
    ssize_t m = 0;
    while (TRUE) {
        bzero(szReceivedMessageBuffer, 2);
        m = recv(sockFd, szReceivedMessageBuffer, 1, MSG_DONTWAIT);

        /* Lines are terminated with \r\n, so we have a new line at \n. We don't want to keep the \r neither, so we return \0 to find the blank line. */
        if (m >= 0) {
            if (*szReceivedMessageBuffer == '\n') {
                break;
            } else if (*szReceivedMessageBuffer != '\r') {
                strncat(szLineBuffer, szReceivedMessageBuffer, 1);
            } else strncat(szLineBuffer, "\0", 1);
        }
    }
    free(szReceivedMessageBuffer);
}