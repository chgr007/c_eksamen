#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/http_utils.h"


static int SetResponseContentType(HTTP_REQUEST *structRequest, HTTP_RESPONSE *structResponse) {
    char szTextPlain[] = "Content-Type: text/plain; charset=UTF-8\r\n";

    if (structRequest->szFileExt == HTML) {
        strcpy(structResponse->szContentType, "Content-Type: text/html; charset=UTF-8\r\n");
    } else if (structRequest->szFileExt == TXT) {
        strcpy(structResponse->szContentType, szTextPlain);
    } else if (structRequest->szFileExt == C) {
        strcpy(structResponse->szContentType, szTextPlain);
    } else if (structRequest->szFileExt == H) {
        strcpy(structResponse->szContentType, szTextPlain);
    } else if (structRequest->szFileExt == O) {
        strcpy(structResponse->szContentType, szTextPlain);
    } else if (structRequest->szFileExt == JPG) {
        strcpy(structResponse->szContentType, "Content-Type: image/jpeg\r\n");
    } else {
        strcpy(structResponse->szContentType, "Content-Type: application/octet-stream\r\n");
    }
    return OK;
}

static int ParseRequestHeaders(int sockFd, HTTP_REQUEST *structRequest) {
    char *szReqLine = (char *) malloc(sizeof(char) * 2048);
    bzero(szReqLine, 2048);
    /* Get the first line  */
    ReadLine(sockFd, szReqLine);

    if (ParseRequestLine(szReqLine, structRequest) != OK) {
        printf("Error parsing request line\n");
        free(szReqLine);
        return ERROR;
    }

    if ((structRequest->szFileExt = GetFileExtension(structRequest->szFilePath)) == -1) {
        free(szReqLine);
        return ERROR;
    }
    // print the fileExt
    printf("File extension: %d\n", structRequest->szFileExt);
    free(szReqLine);
    szReqLine = NULL;
    if (SplitHeaders(structRequest, sockFd) != OK) {
        return ERROR;
    }

    return OK;
}

static int GetFileExtension(char *szFileName) {
    char szTmp[256];
    printf("FILENAME %s\n", szFileName);

    if (szFileName == NULL || strlen(szFileName) == 0) {
        return -1;
    }

    strcpy(szTmp, szFileName);

    char *szTok = strtok(szTmp, ".");

    /*
     * Expected input format is "./file.ext"
     * strtok is wacky. Noticed that there's newer functions,
     * but they don't comply with C89, and I try to stick with that as far as possible.
     * */
    szTok = strtok(NULL, ".");

    if (szTok == NULL) {
        return UNKNOWN;
    }

    printf("Line 79: File extension: %s\n", szTok);
    if (strcmp(szTok, "html") == 0) {
        return HTML;
    } else if (strcmp(szTok, "txt") == 0) {
        return TXT;
    } else if (strcmp(szTok, "c") == 0) {
        return C;
    } else if (strcmp(szTok, "h") == 0) {
        return H;
    } else if (strcmp(szTok, "o") == 0) {
        return O;
    } else if (strcmp(szTok, "jpg") == 0){
        return JPG;
    } else {
        return UNKNOWN;
    }
}

static int ParseRequestLine(char *szRequestLine, HTTP_REQUEST *structReq) {
    char *szToken;
    szToken = strtok(szRequestLine, " ");

    // Crude validity check. TODO: Regex pattern matching here if time
    if (szToken == NULL) {
        printf("ERROR: Unexpected format\n");
        return ERROR;
    }

    strcpy(structReq->szMethod, szToken);
    szToken = strtok(NULL, " ");

    if (strlen(szToken) > 255) {
        printf("ERROR: Filename too long. Path and name must be less than 255 characters\n");
        return ERROR;
    }
    /* appending . so that it will be "./filename" (current folder). */
    strcpy(structReq->szFilePath, ".");
    strcat(structReq->szFilePath, szToken);

    if ((structReq->szFileExt = GetFileExtension(structReq->szFilePath)) == -1) {
        printf("ERROR: File extension not supported\n");
        return ERROR;
    }
    return OK;
}

static int SetResponseHeaders(HTTP_REQUEST *structRequest, HTTP_RESPONSE *structResponse, long iFileSize) {
    /* Weird bug occurs when using \r\n in sprintf
     * Work around..
     * */
    char szContentLength[256];
    bzero(szContentLength, 256);
    sprintf(szContentLength, "Content-Length: %ld", iFileSize);
    strcat(szContentLength, "\r\n");

    strcpy(structResponse->szContentLength, szContentLength);
    strcpy(structResponse->szStatusMessage, "HTTP/1.1 200 OK\r\n");
    strcpy(structResponse->szVersion, "1.1");
    strcpy(structResponse->szServer, SERVER);
    SetResponseContentType(structRequest, structResponse);

    printf("Content length: %s\n", structResponse->szContentLength);

    return 1;
}

static int SendResponseHeaders(int sockFd, HTTP_RESPONSE *structResponse) {
    char szResponse[2048];
    strcpy(szResponse, structResponse->szStatusMessage);
    strcat(szResponse, structResponse->szServer);
    strcat(szResponse, structResponse->szContentLength);
    strcat(szResponse, structResponse->szContentType);
    strcat(szResponse, "\r\n");

    if (write(sockFd, szResponse, strlen(szResponse)) == -1) {
        printf("Error sending response headers\n");
        return ERROR;
    }
    return OK;
}

int HandleConnection(int sockClientFd) {
    char *szRequestLine;
    char *szFileReadOption;
    HTTP_REQUEST *structReq;

    printf("Accepted connection\n");

    if (sockClientFd > -1) {
        szRequestLine = (char *) malloc(1024 * sizeof(char));
        szFileReadOption = malloc(sizeof(char) * 3);
        structReq = (HTTP_REQUEST *) malloc(sizeof(HTTP_REQUEST));
        long lFileSize;

        bzero(szRequestLine, 1024);
        bzero(szFileReadOption, 3);
        memset(structReq, 0, sizeof(HTTP_REQUEST));
        printf("Reading request line\n");
        if (!(ParseRequestHeaders(sockClientFd, structReq))) {
            printf("ERROR: Could not parse request headers\n");
            free(szRequestLine);
            free(szFileReadOption);
            free(structReq);
            return ERROR;
        }

        printf("Method: %s\n", structReq->szMethod);
        if (strcmp(structReq->szMethod, "GET") != 0) {
            printf("ERROR: Method not supported\n");
            free(szRequestLine);
            free(szFileReadOption);
            free(structReq);
            return ERROR;
        }

        /* Check if we got a supported text format, otherwise read the file as binary */
        bzero(szFileReadOption, 3);
        if (structReq->szFileExt == HTML || structReq->szFileExt == TXT || structReq->szFileExt == C ||
            structReq->szFileExt == H) {
            strcpy(szFileReadOption, "r");
        } else {
            strcpy(szFileReadOption, "rb");
        }

        FILE *fdFile = fopen(structReq->szFilePath, szFileReadOption);

        if (fdFile != NULL) {
            HTTP_RESPONSE *structResp = (HTTP_RESPONSE *) malloc(sizeof(HTTP_RESPONSE));
            memset(structResp, 0, sizeof(HTTP_RESPONSE));
            printf("Found the file: %s\n", structReq->szFilePath);
            fseek(fdFile, 0L, SEEK_END);
            lFileSize = ftell(fdFile);
            fseek(fdFile, 0L, SEEK_SET);
            printf("File size: %ld\n", lFileSize);
            printf("File size content type: %s\n", structResp->szContentLength);
            SetResponseHeaders(structReq, structResp, lFileSize);
            if (SendResponseHeaders(sockClientFd, structResp)) {
                WriteFileToSocket(fdFile, sockClientFd, lFileSize);
            } else {
                printf("ERROR: Could not send response headers\n");
            }
            // close the socket
            fclose(fdFile);
            free(structResp);
            structResp = NULL;
        } else {
            printf("ERROR: Could not open file\n");
        }
    } else {
        printf("ERROR on accept\n");
        return 1;
    }
    free(szRequestLine);
    free(szFileReadOption);
    free(structReq);
}

static int WriteFileToSocket(FILE *fdFile, int sockClientFd, long iFileSize) {
    /*
     * Size of an Ethernet frame
     * It might not be of any use to do it this way unless I write an algorithm to send a chunk of the file with the headers
     * if it buffers 1500B on the network layer then this will be out of sync
     */
    char *byFileBuffer = malloc(sizeof (char) * iFileSize);
    memset(byFileBuffer, 0, iFileSize);
    int iBytesRead;
    //const char *responseHeader = "HTTP/1.1 200 OK\r\n\r\n";
    printf("Writing to socket!\n");
    //write(sockClientFd, responseHeader, strlen(responseHeader));

    while (!feof(fdFile)) {

        iBytesRead = fread(byFileBuffer, 1, 1500, fdFile);
        printf("Bytes read: %d\n", iBytesRead);
        // 500 internal server error or something. Got to read up on how to gracefully exit during transmission.
        // Guess this would be more of a network layer thing since the http-header is already sent?
        if (ferror(fdFile)) {
            printf("Error reading file\n");
            free(byFileBuffer);
            return ERROR;
        }
        if (write(sockClientFd, byFileBuffer, iBytesRead) < 0) {
            printf("Error writing to socket\n");
            free(byFileBuffer);
            return ERROR;
        } else {
            printf("Wrote to socket\n");
        }
    }
    free(byFileBuffer);
    printf("Done writing to socket!\n");
    return OK;
}

static int SplitHeaders(HTTP_REQUEST *structHttpRequest, int sockFd) {
    char *szToken;
    char *szLineBuffer = (char *) malloc(1024 * sizeof(char));
    bzero(szLineBuffer, 1024);

    /* Loops through one by one line of the response, until it receives a blank NULL terminated line */
    ReadLine(sockFd, szLineBuffer);
    while (strcmp(szLineBuffer, "\0") != 0) {
        szToken = strtok(szLineBuffer, ": ");

        if (strcmp(szToken, "Content-Type") == 0) {
            szToken = strtok(NULL, ": ");
            strcpy(structHttpRequest->structHeaders->szContentType, szToken);
        } else if (strcmp(szToken, "Content-Length") == 0) {
            szToken = strtok(NULL, ": ");
            structHttpRequest->structHeaders->iContentLength = atoi(szToken);
        }
        bzero(szLineBuffer, 1024);
        ReadLine(sockFd, szLineBuffer);
    }
    free(szLineBuffer);
    return OK;
}

static int ReadLine(int sockFd, char *szLineBuffer) {
    char *szReceivedMessageBuffer = (char *) malloc(sizeof(char));
    bzero(szReceivedMessageBuffer, 1);

    while (TRUE) {
        int m = recv(sockFd, szReceivedMessageBuffer, 1, MSG_DONTWAIT);

        /* Lines are terminated with \r\n, so we have a new line at \n. We don't want to keep the \r neither, so we return \0 to find the blank line. */
        if (m >= 0) {
            if (*szReceivedMessageBuffer == '\n') {
                break;
            } else if (*szReceivedMessageBuffer != '\r') {
                strncat(szLineBuffer, szReceivedMessageBuffer, 1);
            }
        }
    }
    free(szReceivedMessageBuffer);
    return OK;
}
