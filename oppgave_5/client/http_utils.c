#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

#include "include/http_utils.h"

/* Send a request to an open socket. */
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
/*
 * Function to parse a URL to get the protocol + host + path.
 * */
struct URL* ParseURL(char *szUrl) {
    char *szToken, *szPartlyParsedUrl, szParams[256] = {0};
    struct URL *structUrl = malloc(sizeof(struct URL));

    szPartlyParsedUrl = (char *) malloc(sizeof(char) * 1024);
    bzero(szPartlyParsedUrl, 1024);
    /* Copy the URL so strtok can manipulate it safely */
    strcpy(szPartlyParsedUrl, szUrl);

    /* get HTTP or HTTPS */
    szToken = strtok(szPartlyParsedUrl, "://");
    strcpy(structUrl->szProtocol, szToken);

    /* Gets the host */
    szToken = strtok(NULL, "://");
    strcpy(structUrl->szHost, szToken);

    /* Gets the path */
    while ((szToken = strtok(NULL,"/")) != NULL) {
        strncat(szParams, "/", 2);
        strncat(szParams, szToken, strlen(szToken) + 1);
    }
    strcpy(structUrl->szPath, szParams);

    free(szPartlyParsedUrl);
    return structUrl;
}

/*
 * Parses the response from the server, and extracts the header
 *
 * The response expected from the server should be in the pattern of
 * HTTP/1.1 200 OK
 * Header-Field1: Header-Value1
 * Header-Field2: Header-Value2
 * Content-Length: n
 *
 * n size of payload
 * */
struct HTTP_RESPONSE* GetHeaders(int sockFd) {
    char *szLineBuffer, *szToken;
    struct HTTP_RESPONSE *structHttpResponse;
    szLineBuffer = (char *) malloc(sizeof(char) * 256);
    structHttpResponse = malloc(sizeof(struct HTTP_RESPONSE));

    /* Get the first line / status line */
    ReadLine(sockFd, szLineBuffer);
    szToken = strtok(szLineBuffer,  " ");
    strcpy(structHttpResponse->szVersion, szToken);
    szToken = strtok(NULL, " ");
    structHttpResponse->iStatusCode = atoi(szToken);
    printf("Status code: %d", structHttpResponse->iStatusCode);
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
    if (iContentLength <= 0) {
        printf("ERROR: Unexpected length on payload: %d\n", iContentLength);
        return ERROR;
    }
    structHttpResponse->szPayload = (char *) malloc(sizeof(char) * iContentLength);

    // While there is data to read, read it
    printf("Content-length: %d", iContentLength);
    int mBytes, totBytes = 0;
    while ((mBytes = recv(sockFd, structHttpResponse->szPayload, iContentLength, MSG_DONTWAIT)) > 0) {
        printf("Received %d bytes of payload");
        totBytes += mBytes;
    }

    if (totBytes != iContentLength) {
        printf("ERROR: Unexpected length on payload: %d\n", totBytes);
        return ERROR;
    }

    if (mBytes < 0) {
        printf("ERROR: fetching payload\n");
        return ERROR;
    }
    return OK;
}
int SavePayload(struct HTTP_RESPONSE *structHttpResponse) {
    FILE *fp;
    if (strcmp(structHttpResponse->szContentType, "text/html") == 0) {
        fp = fopen("index.html", "w");
    } else if (strcmp(structHttpResponse->szContentType, "text/plain") == 0) {
        fp = fopen("index.txt", "w");
    } else {
        printf("ERROR: Unknown content type: %s", structHttpResponse->szContentType);
        return ERROR;
    }
    if (fp == NULL) {
        printf("ERROR: Could not open file for writing\n");
        return ERROR;
    }
    fprintf(fp, "%s", structHttpResponse->szPayload);
    fclose(fp);
    return OK;
}
/* Gets the header fields value and sets the HTTP_RESPONSE pointers fields accordingly */
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
        } else if (strcmp(szToken, "Content-Type")  == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpResponse->szContentType, szToken);
        }
        else if(strcmp(szToken, "Content-Length")  == 0) {
            szToken = strtok(NULL, ": ");
            structHttpResponse->iContentLength = atoi(szToken);
        }
        bzero(szLineBuffer, 256);
        ReadLine(sockFd, szLineBuffer);
    }
}

/* Reads one line of the response and updates a buffer */
int ReadLine(int sockFd,char *szLineBuffer) {
    char *szReceivedMessageBuffer = (char *) malloc(sizeof(char));
    bzero(szReceivedMessageBuffer, 1);

    while (TRUE) {
        int m = recv(sockFd, szReceivedMessageBuffer, 1, MSG_DONTWAIT);

        /* Lines are terminated with \r\n, so we have a new line at \n. We don't want to keep the \r neither, so we return \0 to find the blank line. */
        if (m >= 0) {
            if (*szReceivedMessageBuffer == '\n') {
                break;
            }
            else if (*szReceivedMessageBuffer != '\r') {
                strncat(szLineBuffer, szReceivedMessageBuffer, 1);
            }
            else strncat(szLineBuffer, "\0", 1);
        }
    }
    free(szReceivedMessageBuffer);
}